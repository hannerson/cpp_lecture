#pragma once

#include <memory>
#include <mutex>
#include "connection_factory.h"

class SocketConnectionFactory : public ConnectionFactory{
 public:
  ~SocketConnectionFactory();
  static std::shared_ptr<ConnectionFactory> get();
  std::shared_ptr<Connection> create_proxy_connection() override;
  std::shared_ptr<Connection> create_service_connection() override;

 private:
  SocketConnectionFactory();
  static std::shared_ptr<ConnectionFactory> ptr_;
  static std::mutex mutex_;
};