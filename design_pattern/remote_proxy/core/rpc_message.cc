#include "rpc_message.h"

#include "utils.h"

RpcMessage::RpcMessage() {}
RpcMessage::~RpcMessage() {}

RpcMessage::RpcMessage(const RpcMessageType& type, const RpcMethod& method_id,
                       const RpcCallStatus& status,
                       const uint64_t& message_id = get_timestamps()) {
  header_.type = type;
  header_.method_id = method_id;
  header_.status = status;
  header_.length = 0;
  header_.message_id = message_id;
  // write
  write(header_.message_id, header_buffer_);
  write(static_cast<uint8_t>(header_.type), header_buffer_);
  write(static_cast<uint8_t>(header_.method_id), header_buffer_);
  write(static_cast<uint8_t>(header_.status), header_buffer_);
  write(header_.length, header_buffer_);
}

std::shared_ptr<RpcMessage> RpcMessage::create_request_message(
    const RpcMessageType& type, const RpcMethod& method_id) {
  return std::make_shared<RpcMessage>(type, method_id,
                                      RpcCallStatus::RPC_CALL_STATUS_OK);
}

std::shared_ptr<RpcMessage> RpcMessage::create_response_message(
    const std::shared_ptr<RpcMessage> request, const RpcCallStatus& status) {
  RpcHeader header = request->get_header();
  return std::make_shared<RpcMessage>(RpcMessageType::RPC_RESPONSE,
                                      header.method_id, status,
                                      header.message_id);
}

std::shared_ptr<RpcMessage> RpcMessage::create_notify_message(
    const RpcMethod& method_id) {
  return std::make_shared<RpcMessage>(RpcMessageType::RPC_NOTIFY, method_id,
                                      RpcCallStatus::RPC_CALL_STATUS_OK);
}

const RpcHeader& RpcMessage::get_header() { return header_; }

void RpcMessage::set_header(const RpcHeader& header) {
  header_.message_id = header.message_id;
  header_.type = header.type;
  header_.method_id = header.method_id;
  header_.status = header.status;
  header_.length = header.length;

  // write
  write(header_.message_id, header_buffer_);
  write(static_cast<uint8_t>(header_.type), header_buffer_);
  write(static_cast<uint8_t>(header_.method_id), header_buffer_);
  write(static_cast<uint8_t>(header_.status), header_buffer_);
  write(header_.length, header_buffer_);
}

const std::vector<uint8_t>& RpcMessage::get_header_buffer() {
  return header_buffer_;
}

bool RpcMessage::set_header_buffer(const std::vector<uint8_t>& data) {
  if (data.size() < sizeof(RpcHeader)) {
    return false;
  }
  header_buffer_.assign(data.begin(), data.end());
  // parse header
  read(header_.message_id, header_buffer_.data());
  read(header_.type, header_buffer_.data() + 8);
  read(header_.method_id, header_buffer_.data() + 9);
  read(header_.status, header_buffer_.data() + 10);
  read(header_.length, header_buffer_.data() + 11);
}

const std::vector<uint8_t>& RpcMessage::get_payload() { return payload_; }

void RpcMessage::set_payload(const std::vector<uint8_t>& payload) {
  payload_.assign(payload.begin(), payload.end());
  header_.length = payload_.size();
  write(header_.length, 11, header_buffer_);
}

void RpcMessage::set_client_id(const uint32_t& client_id) {
  client_id_ = client_id;
}

const uint32_t RpcMessage::get_client_id() { return client_id_; }