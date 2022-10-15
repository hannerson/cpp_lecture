#include "proxy.h"

MyProxy::MyProxy() {}

MyProxy::~MyProxy() {}

void MyProxy::on_message_received(std::shared_ptr<RpcMessage> msg) {
  std::cout << "received message : " << msg->get_header().message_id
            << std::endl;
}

int MyProxy::add(int a, int b) {
  std::shared_ptr<RpcMessage> request = RpcMessage::create_request_message(
      RpcMessageType::RPC_REQUEST, RpcMethod::RPC_METHOD_ADD);
  std::vector<uint8_t> payload;
  request->write(a, payload);
  request->write(b, payload);
  request->set_payload(payload);
  // send message
  std::shared_ptr<RpcMessage> response = send_message(request);
  if (!response) {
    return 0;
  }
  std::cout << " response status: " << int(response->get_header().status)
            << std::endl;
  // response
  const std::vector<uint8_t>& resp_payload = response->get_payload();
  int result = 0;
  response->read(result, resp_payload.data());
  return result;
}