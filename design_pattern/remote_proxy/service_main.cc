#include "service.h"

int main() {
  std::cout << " --- create service --- " << std::endl;
  std::shared_ptr<MyService> service = std::make_shared<MyService>();
  std::cout << " --- start service --- " << std::endl;
  service->init("10.0.24.3", 2345);
  service->start();
  //
  while (1) {
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
  }
  service->stop();
  return 0;
}