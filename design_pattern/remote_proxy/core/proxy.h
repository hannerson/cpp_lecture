#pragma once

#include "connection.h"
#include "rpc_message.h"
#include "runtime.h"

class RpcProxy {
 public:
  RpcProxy();
  virtual ~RpcProxy();
  void init(std::string ip, uint16_t port);
  bool connect();
  bool disconnect();
  virtual void on_message_received(std::shared_ptr<RpcMessage> message) = 0;
  std::shared_ptr<RpcMessage> send_message(std::shared_ptr<RpcMessage> message);
  void register_on_msg_received_callback(Connection::msg_received_func callback);

 private:
  std::shared_ptr<Connection> connection_;
};