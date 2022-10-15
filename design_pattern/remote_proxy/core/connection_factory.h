#pragma once

#include <memory>
#include <mutex>

#include "connection.h"

class ConnectionFactory {
 public:
  virtual ~ConnectionFactory() {};
  virtual std::shared_ptr<Connection> create_proxy_connection() = 0;
  virtual std::shared_ptr<Connection> create_service_connection() = 0;
};