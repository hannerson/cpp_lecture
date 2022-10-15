#pragma once

#include <sys/socket.h>
#include <sys/types.h>

#include <mutex>

#include "../connection.h"

class SocketConnectionImpl : public Connection {
 public:
  SocketConnectionImpl(bool is_service, std::string ip = "", uint16_t port = 0);
  ~SocketConnectionImpl();
  bool do_connect_service() override;
  bool do_disconnect_service() override;
  bool do_start_service() override;
  bool do_stop_service() override;
  bool is_service_available() override;
  bool send_message(std::shared_ptr<RpcMessage> request) override;

  void set_service_addr(std::string ip) override;
  void set_service_port(uint16_t port) override;

 protected:
  void service_worker();
  void proxy_worker();

 private:
  // socket
  int socket_fd_;
  // epoll fd
  int epoll_fd_;
  std::atomic<bool> is_service_;
  std::mutex running_mutex_;
  bool is_running_;

  // ip address and port
  std::string ip_;
  uint16_t port_;

  // io thread
  std::shared_ptr<std::thread> io_thread_;

  // cv wait for connection ok
  std::mutex worker_mutex_;
  std::condition_variable cv_worker_;

  // client info
  // std::map<int, > client_info_;
  std::mutex buffer_mutex_;
  std::vector<uint8_t> buffer_;
  const uint16_t BUFFER_READ = 1024;
};