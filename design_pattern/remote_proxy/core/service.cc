#include "service.h"

RpcService::RpcService()
    : connection_(
          Runtime::get()->create_service_connection("socket_connection")) {}

RpcService::~RpcService() {}

void RpcService::init(std::string ip, uint16_t port) {
  connection_->set_service_addr(ip);
  connection_->set_service_port(port);
}

bool RpcService::start() {
  connection_->register_msg_received_callback(
      std::bind(&RpcService::on_message_received, this, std::placeholders::_1));
  return connection_->start_service();
}

bool RpcService::stop() { return connection_->stop_service(); }

std::shared_ptr<RpcMessage> RpcService::send_message(
    std::shared_ptr<RpcMessage> message) {
  return connection_->rpc_call_block(message);
}

bool RpcService::send_response(
    std::shared_ptr<RpcMessage> message) {
  return connection_->send_message(message);
}

void RpcService::register_on_msg_received_callback(
    Connection::msg_received_func callback) {
  connection_->register_msg_received_callback(std::move(callback));
}