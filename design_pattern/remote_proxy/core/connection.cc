#include "connection.h"

#include <chrono>

Connection::Connection(bool is_service, std::string ip, uint16_t port)
    : is_service_(is_service), is_running_(false), ip_(ip), port_(port) {}

Connection::~Connection() {}

bool Connection::start_service() {
  if (!is_service_) {
    return false;
  }
  // start dispatch thread
  {
    std::lock_guard<std::mutex> lock(running_mutex_);
    if (is_running_) {
      return true;
    }
    is_running_ = true;
  }
  dispatch_thread_ = std::make_shared<std::thread>(
      std::bind(&Connection::dispatch_worker, this));
  return do_start_service();
}

bool Connection::stop_service() {
  if (!is_service_) {
    return false;
  }
  // stop dispatch thread
  {
    std::lock_guard<std::mutex> lock(running_mutex_);
    if (!is_running_) {
      return true;
    }
    is_running_ = false;
  }
  if (dispatch_thread_ && dispatch_thread_->joinable()) {
    dispatch_thread_->join();
  }
  return do_stop_service();
}

void Connection::set_service_addr(std::string ip) { ip_ = ip; }
void Connection::set_service_port(uint16_t port) { port_ = port; }

bool Connection::connect() {
  if (is_service_) {
    return false;
  }
  // start dispatch thread
  {
    std::lock_guard<std::mutex> lock(running_mutex_);
    if (is_running_) {
      return true;
    }
    is_running_ = true;
  }
  dispatch_thread_ = std::make_shared<std::thread>(
      std::bind(&Connection::dispatch_worker, this));
  return do_connect_service();
}

bool Connection::disconnect() {
  if (is_service_) {
    return false;
  }
  // stop dispatch thread
  {
    std::lock_guard<std::mutex> lock(running_mutex_);
    if (!is_running_) {
      return true;
    }
    is_running_ = false;
  }
  if (dispatch_thread_ && dispatch_thread_->joinable()) {
    dispatch_thread_->join();
  }
  return do_disconnect_service();
}

void Connection::dispatch_worker() {
  std::cout << __PRETTY_FUNCTION__ << std::endl;
  bool is_running = false;
  {
    std::lock_guard<std::mutex> lock(running_mutex_);
    is_running = is_running_;
  }
  while (is_running) {
    {
      std::lock_guard<std::mutex> lock(running_mutex_);
      is_running = is_running_;
    }
    std::shared_ptr<RpcMessage> message = nullptr;
    {
      std::unique_lock<std::mutex> lock(msg_dispatch_mutex_);
      if (msg_queue_.empty()) {
        msg_dispatch_cv_.wait(lock);
      }
      message = msg_queue_.front();
      msg_queue_.pop();
    }
    if (!message) {
      continue;
    }
    RpcHeader header = message->get_header();
    std::cout << "process message id: " << header.message_id << std::endl;
    // proxy response message
    if (header.type == RpcMessageType::RPC_RESPONSE) {
      // block reply
      std::cout << "process response msg" << std::endl;
      {
        std::unique_lock<std::mutex> lock(block_mutex_);
        auto found = block_reply_.find(header.message_id);
        if (found == block_reply_.end()) {
          std::cout << "error: no such msg " << header.message_id << std::endl;
          continue;
        }
        found->second = message;
        block_cv_.notify_one();
      }
      /// TODO: async reply
    } else {
      /// TODO: proxy notify message or service request message
      on_message_received(message);
    }
  }
}

std::shared_ptr<RpcMessage> Connection::rpc_call_block(
    std::shared_ptr<RpcMessage> request) {
  RpcHeader header = request->get_header();
  std::cout << "send request: " << header.message_id << std::endl;
  std::unique_lock<std::mutex> lock(block_mutex_);
  if (!send_message(request)) {
    return nullptr;
  }
  auto reply_iter = block_reply_.emplace(header.message_id, nullptr);
  block_cv_.wait_for(lock, std::chrono::seconds(5), [reply_iter] {
    return reply_iter.first->second == nullptr ? false : true;
  });
  if (!reply_iter.first->second) {
    /// TODO: clean up timeout message
    std::cout << "error : rpc call return nullptr response" << std::endl;
    return nullptr;
  }
  std::shared_ptr<RpcMessage> reply = reply_iter.first->second;
  block_reply_.erase(header.message_id);
  return reply;
}

bool Connection::rpc_call_async(std::shared_ptr<RpcMessage> request) {
  return true;
}

void Connection::on_service_msg_received(std::shared_ptr<RpcMessage> message) {
  std::cout << __PRETTY_FUNCTION__ << std::endl;
  std::unique_lock<std::mutex> lock(msg_dispatch_mutex_);
  msg_queue_.push(message);
  msg_dispatch_cv_.notify_one();
}

void Connection::on_proxy_msg_received(std::shared_ptr<RpcMessage> message) {
  std::unique_lock<std::mutex> lock(msg_dispatch_mutex_);
  msg_queue_.push(message);
  msg_dispatch_cv_.notify_one();
}

void Connection::on_message_received(std::shared_ptr<RpcMessage> message) {
  std::lock_guard<std::mutex> lock(msg_received_mutex_);
  if (msg_received_callback_) {
    msg_received_callback_(message);
  }
}

void Connection::register_msg_received_callback(msg_received_func callback) {
  std::lock_guard<std::mutex> lock(msg_received_mutex_);
  msg_received_callback_ = std::move(callback);
}
