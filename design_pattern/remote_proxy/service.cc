#include "service.h"

MyService::MyService() {}

MyService::~MyService() {}

void MyService::add(std::shared_ptr<RpcMessage> msg) {
  const std::vector<uint8_t>& payload = msg->get_payload();
  int a = 0;
  int b = 0;
  msg->read(a, payload.data());
  msg->read(b, payload.data() + sizeof(int));
  int result = a + b;
  std::cout << " add " << a << ", " << b << std::endl;

  // response
  std::shared_ptr<RpcMessage> response = RpcMessage::create_response_message(
      msg, RpcCallStatus::RPC_CALL_STATUS_OK);
  response->set_client_id(msg->get_client_id());
  std::vector<uint8_t> resp_payload;
  response->write(result, resp_payload);
  response->set_payload(resp_payload);
  send_response(response);
}

void MyService::minus(std::shared_ptr<RpcMessage> msg) {
  const std::vector<uint8_t>& payload = msg->get_payload();
  int a = 0;
  int b = 0;
  msg->read(a, payload.data());
  msg->read(b, payload.data() + sizeof(int));
  int result = a - b;
  std::cout << " add " << a << ", " << b << std::endl;

  // response
  std::shared_ptr<RpcMessage> response = RpcMessage::create_response_message(
      msg, RpcCallStatus::RPC_CALL_STATUS_OK);
  response->set_client_id(msg->get_client_id());
  std::vector<uint8_t> resp_payload;
  response->write(result, resp_payload);
  response->set_payload(resp_payload);
  send_response(response);
}

void MyService::on_message_received(std::shared_ptr<RpcMessage> msg) {
  std::cout << "received message id : " << msg->get_header().message_id
            << std::endl;
  switch (msg->get_header().method_id) {
    case RpcMethod::RPC_METHOD_ADD:
      /* code */
      add(msg);
      break;
    case RpcMethod::RPC_METHOD_MINUS:
      minus(msg);
      break;
    case RpcMethod::RPC_METHOD_ECHO:
      break;
    default:
      break;
  }
}