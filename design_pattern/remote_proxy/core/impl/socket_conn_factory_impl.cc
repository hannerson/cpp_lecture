#include "socket_conn_factory_impl.h"

#include "socket_connection_impl.h"

std::shared_ptr<ConnectionFactory> SocketConnectionFactory::ptr_ = nullptr;

std::mutex SocketConnectionFactory::mutex_;

SocketConnectionFactory::SocketConnectionFactory() {}
SocketConnectionFactory::~SocketConnectionFactory() {}

std::shared_ptr<ConnectionFactory> SocketConnectionFactory::get() {
  std::lock_guard<std::mutex> lock(mutex_);
  if (ptr_ == nullptr) {
    ptr_ =
        std::shared_ptr<SocketConnectionFactory>(new SocketConnectionFactory());
  }
  return ptr_;
}

std::shared_ptr<Connection> SocketConnectionFactory::create_proxy_connection() {
  return std::make_shared<SocketConnectionImpl>(false);
}

std::shared_ptr<Connection>
SocketConnectionFactory::create_service_connection() {
  return std::make_shared<SocketConnectionImpl>(true);
}
