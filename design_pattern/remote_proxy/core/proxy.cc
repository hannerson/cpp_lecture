#include "proxy.h"

RpcProxy::RpcProxy()
    : connection_(
          Runtime::get()->create_proxy_connection("socket_connection")) {}

RpcProxy::~RpcProxy() {}

void RpcProxy::init(std::string ip, uint16_t port) {
  connection_->set_service_addr(ip);
  connection_->set_service_port(port);
}

bool RpcProxy::connect() {
  connection_->register_msg_received_callback(
      std::bind(&RpcProxy::on_message_received, this, std::placeholders::_1));
  return connection_->connect();
}

bool RpcProxy::disconnect() { return connection_->disconnect(); }

std::shared_ptr<RpcMessage> RpcProxy::send_message(
    std::shared_ptr<RpcMessage> message) {
  return connection_->rpc_call_block(message);
}

void RpcProxy::register_on_msg_received_callback(
    Connection::msg_received_func callback) {
  connection_->register_msg_received_callback(callback);
}
