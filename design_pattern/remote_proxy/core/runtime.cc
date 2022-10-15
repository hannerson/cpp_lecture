#include "runtime.h"

#include "impl/socket_conn_factory_impl.h"

INIT_RUNTIME int runtime_init() {
  Runtime::get()->register_connection_factory("socket_connection",
                                              SocketConnectionFactory::get());
  std::cout << "register socket connection" << std::endl;
}

DESTROY_RUNTIME int runtime_deinit() {}

std::shared_ptr<Runtime> Runtime::ptr_;  // 在constructor后执行,注意这里不可再进行初始化,
                                         // 否则ptr被赋值为nullptr
std::mutex Runtime::mutex_;

Runtime::Runtime() {}

Runtime::~Runtime() {}

std::shared_ptr<Runtime> Runtime::get() {
  std::lock_guard<std::mutex> lock(mutex_);
  if (ptr_ == nullptr) {
    ptr_ = std::shared_ptr<Runtime>(new Runtime());
  }
  return ptr_;
}

void Runtime::register_connection_factory(
    std::string name, std::shared_ptr<ConnectionFactory> factory) {
  std::lock_guard<std::mutex> lock(mutex_);
  factories_map_.emplace(name, factory);
}

std::shared_ptr<ConnectionFactory> Runtime::get_connection_factory(
    std::string name) {
  std::lock_guard<std::mutex> lock(mutex_);
  auto factory = factories_map_.find(name);
  if (factory == factories_map_.end()) {
    return nullptr;
  }
  return factory->second;
}

std::shared_ptr<Connection> Runtime::create_proxy_connection(std::string name) {
  auto factory = get_connection_factory(name);
  if (factory == nullptr) {
    return nullptr;
  }
  return factory->create_proxy_connection();
}

std::shared_ptr<Connection> Runtime::create_service_connection(
    std::string name) {
  auto factory = get_connection_factory(name);
  if (factory == nullptr) {
    return nullptr;
  }
  return factory->create_service_connection();
}
