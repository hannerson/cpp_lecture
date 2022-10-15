#pragma once

#include <atomic>
#include <condition_variable>
#include <iostream>
#include <map>
#include <memory>
#include <mutex>
#include <queue>
#include <thread>

#include "rpc_message.h"

class Connection {
 public:
 typedef std::function<void(std::shared_ptr<RpcMessage>)>
      msg_received_func;
  Connection(bool is_service, std::string ip = "", uint16_t port = 0);
  ~Connection();
  bool connect();
  bool disconnect();
  bool start_service();
  bool stop_service();

  virtual bool do_connect_service() = 0;
  virtual bool do_disconnect_service() = 0;
  virtual bool do_start_service() = 0;
  virtual bool do_stop_service() = 0;
  virtual bool is_service_available() = 0;

  virtual void set_service_addr(std::string ip);
  virtual void set_service_port(uint16_t port);

  virtual bool send_message(std::shared_ptr<RpcMessage> request) = 0;

  std::shared_ptr<RpcMessage> rpc_call_block(
      std::shared_ptr<RpcMessage> request);
  bool rpc_call_async(std::shared_ptr<RpcMessage> request);
  void dispatch_worker();

  // virtual void on_service_data_received() = 0;
  // virtual void on_proxy_data_received() = 0;
  void on_service_msg_received(std::shared_ptr<RpcMessage> message);
  void on_proxy_msg_received(std::shared_ptr<RpcMessage> message);

  void on_message_received(std::shared_ptr<RpcMessage> message);
  void register_msg_received_callback(msg_received_func callback);

 protected:
  virtual void service_worker() = 0;
  virtual void proxy_worker() = 0;

 private:
  std::atomic<bool> is_service_;
  std::atomic<bool> is_running_;
  std::mutex running_mutex_;
  std::shared_ptr<std::thread> dispatch_thread_;

  // request block
  std::condition_variable block_cv_;
  std::mutex block_mutex_;
  std::map<uint64_t, std::shared_ptr<RpcMessage>> block_reply_;

  // msg queue condition_variable
  std::condition_variable msg_dispatch_cv_;
  std::mutex msg_dispatch_mutex_;
  std::queue<std::shared_ptr<RpcMessage>> msg_queue_;

  // message received callback
  std::mutex msg_received_mutex_;
  msg_received_func msg_received_callback_;

  std::string ip_;
  uint16_t port_;
};