#pragma once
#include "core/service.h"

class MyService : public RpcService {
 public:
  MyService();
  ~MyService();
  void add(std::shared_ptr<RpcMessage> msg);
  void minus(std::shared_ptr<RpcMessage> msg);
  void on_message_received(std::shared_ptr<RpcMessage> msg) override;
};
