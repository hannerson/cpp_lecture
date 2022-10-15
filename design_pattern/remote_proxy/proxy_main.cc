#include "proxy.h"

int main() {
  std::cout << " --- begin proxy --- " << std::endl;
  std::shared_ptr<MyProxy> proxy = std::make_shared<MyProxy>();
  proxy->init("10.0.24.3", 2345);
  proxy->connect();
  // rpc request
  std::cout << "add : 1 + 2 = " << proxy->add(1, 2) << std::endl;
  std::cout << "add : 5 + 6 = " << proxy->add(5, 6) << std::endl; 
  std::cout << "add : 1234 + 2345 = " << proxy->add(1234, 2345) << std::endl; 
  proxy->disconnect();
  return 0;
}