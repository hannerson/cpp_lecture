#pragma once

#include <map>
#include <memory>
#include <mutex>
#include <thread>

#include "connection.h"
#include "connection_factory.h"

#ifdef WIN32
#define INIT_RUNTIME
#define DESTROY_RUNTIME
#else
#define INIT_RUNTIME __attribute__((constructor))
#define DESTROY_RUNTIME __attribute__((destructor))
#endif

class Runtime {
 public:
  ~Runtime();
  static std::shared_ptr<Runtime> get();
  void register_connection_factory(std::string name,
                                   std::shared_ptr<ConnectionFactory> factory);
  std::shared_ptr<ConnectionFactory> get_connection_factory(std::string name);

  std::shared_ptr<Connection> create_service_connection(std::string name);
  std::shared_ptr<Connection> create_proxy_connection(std::string name);

 private:
  Runtime();
  static std::shared_ptr<Runtime> ptr_;
  static std::mutex mutex_;
  std::map<std::string, std::shared_ptr<ConnectionFactory>> factories_map_;
};