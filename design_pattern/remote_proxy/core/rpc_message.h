#pragma once
#include <iostream>
#include <memory>
#include <vector>

enum class RpcMessageType : uint8_t {
  RPC_REQUEST = 0,
  RPC_RESPONSE = 1,
  RPC_NOTIFY = 2,
  RPC_UNKNOWN = 99,
};

enum class RpcCallStatus : uint8_t {
  RPC_CALL_STATUS_OK = 0,
  RPC_CALL_STATUS_FAIL = 1,
  RPC_CALL_STATUS_ERROR = 2,
  RPC_CALL_STATUS_TIMEOUT = 3,
  RPC_CALL_STATUS_UNKNOWN = 99,
};

enum class RpcMethod : uint8_t {
  RPC_METHOD_ADD = 0,
  RPC_METHOD_MINUS = 1,
  RPC_METHOD_ECHO = 2,
};

#pragma pack(push, 1)
// #pragma pack(1)
struct RpcHeader {
  uint64_t message_id;
  RpcMessageType type;
  RpcCallStatus status;
  RpcMethod method_id;
  uint32_t length;
};
#pragma pack(pop)
// #pragma pack()

class RpcMessage {
 public:
  static std::shared_ptr<RpcMessage> create_request_message(
      const RpcMessageType& type, const RpcMethod& method_id);
  static std::shared_ptr<RpcMessage> create_response_message(
      const std::shared_ptr<RpcMessage> request, const RpcCallStatus& status);
  static std::shared_ptr<RpcMessage> create_notify_message(
      const RpcMethod& method_id);

  template <class Type>
  void read(Type& value, const uint8_t* data) {
    union Type_ {
      Type value;
      uint8_t data[sizeof(Type)];
    };
    Type_ union_value;
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
    for (int i = 0; i < sizeof(Type); i++) {
      union_value.data[i] = *(data + sizeof(Type) - i - 1);
    }
#else
    memcpy(&union_value, data, sizeof(Type));
#endif
    value = union_value.value;
  }

  template <class Type>
  bool write(const Type& value, std::vector<uint8_t>& buffer) {
    union Type_ {
      Type value;
      uint8_t data[sizeof(Type)];
    };
    Type_ union_value;
    union_value.value = value;
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
    for (int i = 0; i < sizeof(Type); i++) {
      buffer.push_back(union_value.data[sizeof(Type) - i - 1]);
    }
#else
    for (int i = 0; i < sizeof(Type); i++) {
      buffer.push_back(union_value.data[i]);
    }
#endif
  }

  template <class Type>
  bool write(const Type& value, uint32_t position,
             std::vector<uint8_t>& buffer) {
    union Type_ {
      Type value;
      uint8_t data[sizeof(Type)];
    };
    Type_ union_value;
    union_value.value = value;
    if (buffer.size() < position + sizeof(Type)) {
      // error
      return false;
    }
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
    for (int i = 0; i < sizeof(Type); i++) {
      buffer[position + i] = union_value.data[sizeof(Type) - i - 1];
    }
#else
    for (int i = 0; i < sizeof(Type); i++) {
      buffer[position + i] = union_value.data[i];
    }
#endif
  }

  RpcMessage();
  RpcMessage(const RpcMessageType& type, const RpcMethod& method_id,
             const RpcCallStatus& status, const uint64_t& message_id);
  ~RpcMessage();
  void set_payload(const std::vector<uint8_t>& payload);
  const std::vector<uint8_t>& get_payload();
  void set_header(const RpcHeader& header);
  const RpcHeader& get_header();
  bool set_header_buffer(const std::vector<uint8_t>& data);
  const std::vector<uint8_t>& get_header_buffer();
  void set_client_id(const uint32_t& client_id);
  const uint32_t get_client_id();

 private:
  RpcHeader header_;
  std::vector<uint8_t> header_buffer_;
  // payload 不包含header
  std::vector<uint8_t> payload_;
  int32_t client_id_;
};
