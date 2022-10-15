#include "socket_connection_impl.h"

#include <arpa/inet.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <memory>
#include <thread>

SocketConnectionImpl::SocketConnectionImpl(bool is_service, std::string ip,
                                           uint16_t port)
    : is_service_(is_service),
      is_running_(false),
      ip_(ip),
      port_(port),
      Connection(is_service) {
  epoll_fd_ = epoll_create(10);
}

SocketConnectionImpl::~SocketConnectionImpl() {}

void SocketConnectionImpl::set_service_addr(std::string ip) { ip_ = ip; }
void SocketConnectionImpl::set_service_port(uint16_t port) { port_ = port; }

bool SocketConnectionImpl::do_connect_service() {
  if (ip_ == "" || port_ == 0) {
    std::cout << "error: not set ip or port" << std::endl;
    return false;
  }
  if (is_service_) {
    return false;
  }
  {
    std::lock_guard<std::mutex> lock(running_mutex_);
    if (is_running_) {
      return true;
    }
    is_running_ = true;
  }

  io_thread_ = std::make_shared<std::thread>(
      std::bind(&SocketConnectionImpl::proxy_worker, this));

  // wait worker ok
  {
    std::unique_lock<std::mutex> lock(worker_mutex_);
    cv_worker_.wait(lock);
  }
  return true;
}

bool SocketConnectionImpl::do_disconnect_service() {
  if (is_service_) {
    return false;
  }
  {
    std::lock_guard<std::mutex> lock(running_mutex_);
    is_running_ = false;
  }
  if (io_thread_ && io_thread_->joinable()) {
    io_thread_->join();
  }
  return true;
}

bool SocketConnectionImpl::do_start_service() {
  if (ip_ == "" || port_ == 0) {
    std::cout << "error: not set ip or port" << std::endl;
    return false;
  }
  if (!is_service_) {
    return false;
  }
  {
    std::lock_guard<std::mutex> lock(running_mutex_);
    if (is_running_) {
      return true;
    }
    is_running_ = true;
  }

  io_thread_ = std::make_shared<std::thread>(
      std::bind(&SocketConnectionImpl::service_worker, this));
  // wait worker ok
  {
    std::unique_lock<std::mutex> lock(worker_mutex_);
    cv_worker_.wait(lock);
  }
  return true;
}

bool SocketConnectionImpl::do_stop_service() {
  if (!is_service_) {
    return false;
  }
  {
    std::lock_guard<std::mutex> lock(running_mutex_);
    is_running_ = false;
  }
  if (io_thread_ && io_thread_->joinable()) {
    io_thread_->join();
  }
  return true;
}

bool SocketConnectionImpl::is_service_available() { return true; }

bool SocketConnectionImpl::send_message(std::shared_ptr<RpcMessage> message) {
  const std::vector<uint8_t>& header_payload = message->get_header_buffer();
  const std::vector<uint8_t>& payload = message->get_payload();
  std::cout << "header size: " << header_payload.size() << std::endl;
  std::cout << "payload size: " << payload.size() << std::endl;
  std::vector<uint8_t> buffer(header_payload.begin(), header_payload.end());
  buffer.insert(buffer.end(), payload.begin(), payload.end());
  if (is_service_) {
    int32_t client_id = message->get_client_id();
    std::cout << "send response to " << client_id << std::endl;
    if (::write(client_id, buffer.data(), buffer.size()) < 0) {
      return false;
    }
  } else {
    std::cout << "send request to " << socket_fd_ << " " << buffer.size()
              << " bytes." << std::endl;
    if (::write(socket_fd_, buffer.data(), buffer.size()) < 0) {
      return false;
    }
  }
  return true;
}

void SocketConnectionImpl::service_worker() {
  std::cout << __PRETTY_FUNCTION__ << std::endl;
  bool is_running = false;
  {
    std::lock_guard<std::mutex> lock(running_mutex_);
    is_running = is_running_;
  }
  struct sockaddr_in server_addr;
  // create socket
  socket_fd_ = socket(AF_INET, SOCK_STREAM, 0);
  if (socket_fd_ < 0) {
    // error
    goto finally;
  }
  std::cout << ip_ << " " << port_ << std::endl;
  memset(&server_addr, 0, sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = port_;
  server_addr.sin_addr.s_addr = inet_addr(ip_.c_str());
  if (bind(socket_fd_, (struct sockaddr*)&server_addr, sizeof(server_addr)) <
      0) {
    goto finally;
  }

  if (listen(socket_fd_, 10) < 0) {
    // error
    goto finally;
  }

  // epoll add server fd
  struct epoll_event server_event;
  server_event.data.fd = socket_fd_;
  server_event.events = EPOLLIN | EPOLLRDHUP;
  if (epoll_ctl(epoll_fd_, EPOLL_CTL_ADD, socket_fd_, &server_event) < 0) {
    // error
    goto finally;
  }

  {
    std::unique_lock<std::mutex> lock(worker_mutex_);
    cv_worker_.notify_one();
  }

  struct epoll_event event_lists[10];

  while (is_running) {
    {
      std::lock_guard<std::mutex> lock(running_mutex_);
      is_running = is_running_;
    }

    int event_nums = epoll_wait(epoll_fd_, event_lists, 10, 100);
    if (event_nums < 0) {
      // error
      goto finally;
    } else if (event_nums == 0) {
      continue;
    }

    // process events
    for (int i = 0; i < event_nums; i++) {
      if ((event_lists[i].events & EPOLLERR) ||
          (event_lists[i].events & EPOLLHUP)) {
        // error
        goto finally;
      }
      if (event_lists[i].events & EPOLLRDHUP) {
        epoll_ctl(epoll_fd_, EPOLL_CTL_DEL, event_lists[i].data.fd,
                  &event_lists[i]);
        close(event_lists[i].data.fd);
      }

      // accept
      if (event_lists[i].data.fd == socket_fd_) {
        struct sockaddr_in client_addr;
        socklen_t len = sizeof(client_addr);
        int accept_fd =
            accept(socket_fd_, (struct sockaddr*)&client_addr, &len);
        std::cout << "accept client : " << accept_fd << std::endl;
        if (accept_fd < 0) {
          // accept error
          continue;
        }
        struct epoll_event event;
        event.data.fd = accept_fd;
        event.events = EPOLLIN | EPOLLOUT | EPOLLRDHUP;
        if (epoll_ctl(epoll_fd_, EPOLL_CTL_ADD, accept_fd, &event) < 0) {
          continue;
        }
      } else {
        // receive buffer
        std::cout << "receive data from client : " << event_lists[i].data.fd
                  << std::endl;
        std::vector<uint8_t> buffer(BUFFER_READ, 0);
        int read_num = 0;
        do {
          read_num = read(event_lists[i].data.fd, buffer.data(), BUFFER_READ);
          if (read_num <= 0) {
            break;
          }
          std::cout << "read data size: " << read_num << std::endl;
          std::lock_guard<std::mutex> lock(buffer_mutex_);
          buffer_.insert(buffer_.end(), buffer.begin(),
                         buffer.begin() + read_num);
          std::cout << "buffer size: " << buffer_.size() << std::endl;
          std::cout << "rpc header size: " << sizeof(RpcHeader) << std::endl;
          if (buffer_.size() < sizeof(RpcHeader)) {
            break;
          }
          uint32_t msg_len = 0;
          std::shared_ptr<RpcMessage> message = std::make_shared<RpcMessage>();
          message->set_header_buffer(std::move(std::vector<uint8_t>(
              buffer_.begin(), buffer_.begin() + sizeof(RpcHeader))));
          RpcHeader header = message->get_header();
          std::cout << "rpc header size --- : " << sizeof(header.status) << std::endl;
          msg_len = header.length;
          std::cout << "message length: " << msg_len << std::endl;
          if (buffer_.size() < msg_len + sizeof(RpcHeader)) {
            std::cout << "buffer size: " << buffer_.size()
                      << " small than message " << msg_len + sizeof(RpcHeader)
                      << std::endl;
            break;
          }

          message->set_payload(std::move(std::vector<uint8_t>(
              buffer_.begin() + sizeof(RpcHeader),
              buffer_.begin() + sizeof(RpcHeader) + msg_len)));
          message->set_client_id(event_lists[i].data.fd);

          buffer_.erase(buffer_.begin(),
                        buffer_.begin() + sizeof(RpcHeader) + msg_len);

          // dispatch message
          on_service_msg_received(message);

        } while (read_num > 0);
      }
    }
  }
finally:
  // error
  {
    std::lock_guard<std::mutex> lock(running_mutex_);
    is_running_ = false;
  }
  close(epoll_fd_);
  close(socket_fd_);
}

void SocketConnectionImpl::proxy_worker() {
  std::cout << __PRETTY_FUNCTION__ << std::endl;
  bool is_running = false;
  {
    std::lock_guard<std::mutex> lock(running_mutex_);
    is_running = is_running_;
  }
  struct sockaddr_in server_addr;
  // create socket
  socket_fd_ = socket(AF_INET, SOCK_STREAM, 0);
  if (socket_fd_ < 0) {
    // error
    std::cout << "create socket error" << std::endl;
    goto finally;
  }
  memset(&server_addr, 0, sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = port_;
  server_addr.sin_addr.s_addr = inet_addr(ip_.c_str());
  struct epoll_event event_lists[10];
  if (::connect(socket_fd_, (const sockaddr*)&server_addr,
                sizeof(server_addr)) < 0) {
    std::cout << "connect to server failed" << std::endl;
    goto finally;
  }

  // epoll add server fd
  struct epoll_event proxy_event;
  proxy_event.data.fd = socket_fd_;
  proxy_event.events = EPOLLIN | EPOLLRDHUP | EPOLLOUT;
  if (epoll_ctl(epoll_fd_, EPOLL_CTL_ADD, socket_fd_, &proxy_event) < 0) {
    // error
    goto finally;
  }

  {
    std::unique_lock<std::mutex> lock(worker_mutex_);
    cv_worker_.notify_one();
  }

  while (is_running) {
    {
      std::lock_guard<std::mutex> lock(running_mutex_);
      is_running = is_running_;
    }

    int event_nums = epoll_wait(epoll_fd_, event_lists, 10, 100);
    if (event_nums < 0) {
      // error
      goto finally;
    } else if (event_nums == 0) {
      continue;
    }

    // process events
    for (int i = 0; i < event_nums; i++) {
      if ((event_lists[i].events & EPOLLERR) ||
          (event_lists[i].events & EPOLLHUP)) {
        // error
        goto finally;
      }
      if (event_lists[i].events & EPOLLRDHUP) {
        epoll_ctl(epoll_fd_, EPOLL_CTL_DEL, event_lists[i].data.fd,
                  &event_lists[i]);
        close(event_lists[i].data.fd);
      }

      if (event_lists[i].events & EPOLLIN) {
        // receive buffer
        std::vector<uint8_t> buffer(BUFFER_READ, 0);
        int read_num = 0;
        do {
          read_num = read(event_lists[i].data.fd, buffer.data(), BUFFER_READ);
          std::lock_guard<std::mutex> lock(buffer_mutex_);
          buffer_.insert(buffer_.end(), buffer.begin(),
                         buffer.begin() + read_num);
          if (buffer_.size() < sizeof(RpcHeader)) {
            break;
          }
          uint32_t msg_len = 0;
          std::shared_ptr<RpcMessage> message = std::make_shared<RpcMessage>();
          message->set_header_buffer(std::move(std::vector<uint8_t>(
              buffer_.begin(), buffer_.begin() + sizeof(RpcHeader))));
          RpcHeader header = message->get_header();
          msg_len = header.length;
          if (buffer_.size() < msg_len + sizeof(RpcHeader)) {
            break;
          }

          message->set_payload(std::move(std::vector<uint8_t>(
              buffer_.begin() + sizeof(RpcHeader),
              buffer_.begin() + sizeof(RpcHeader) + msg_len)));

          buffer_.erase(buffer_.begin(),
                        buffer_.begin() + sizeof(RpcHeader) + msg_len);

          // dispatch message
          on_proxy_msg_received(message);

        } while (read_num > 0);
      }
    }
  }
finally:
  // error
  {
    std::lock_guard<std::mutex> lock(running_mutex_);
    is_running_ = false;
  }
  close(epoll_fd_);
  close(socket_fd_);
}
