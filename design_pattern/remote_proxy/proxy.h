#pragma once
#include "core/proxy.h"

class MyProxy : public RpcProxy {
 public:
  MyProxy();
  ~MyProxy();
  int add(int a, int b);
  int minus(int a, int b);
  std::string echo(std::string info);
  void on_message_received(std::shared_ptr<RpcMessage> msg) override;
};