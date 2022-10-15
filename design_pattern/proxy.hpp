#include <iostream>
#include <random>

class ServiceInterface {
 public:
  virtual void get_temperature() = 0;
  virtual void get_wind() = 0;
};

class WeatherService : public ServiceInterface {
 public:
  void get_temperature() { std::cout << "get temperature" << std::endl; }
  void get_wind() { std::cout << "get wind" << std::endl; }
};

class WeatherProxy : public ServiceInterface {
 public:
  explicit WeatherProxy(WeatherService* service) : service_(service) {
    std::default_random_engine r(time(0));
    num_ = r();
  }
  ~WeatherProxy() {}
  bool check_access() {
    std::cout << "check access: " << num_ << std::endl;
    return (num_ & 0x1) == 0 ? true : false;
  }
  void get_temperature() {
    if (!check_access()) {
      return;
    }
    service_->get_temperature();
  }
  void get_wind() {
    if (!check_access()) {
      return;
    }
    service_->get_wind();
  }

 private:
  ServiceInterface* service_;
  int num_;
};
