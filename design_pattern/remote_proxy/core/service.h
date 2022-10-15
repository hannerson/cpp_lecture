#pragma once

#include "connection.h"
#include "rpc_message.h"
#include "runtime.h"

class RpcService {
 public:
  RpcService();
  virtual ~RpcService();
  void init(std::string ip, uint16_t port);
  bool start();
  bool stop();
  virtual void on_message_received(std::shared_ptr<RpcMessage> message) = 0;
  void register_on_msg_received_callback(Connection::msg_received_func callback);
  std::shared_ptr<RpcMessage> send_message(std::shared_ptr<RpcMessage> message);
  bool send_response(std::shared_ptr<RpcMessage> message);

 private:
  std::shared_ptr<Connection> connection_;
};