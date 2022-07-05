#include <iostream>
#include <memory>
#include <mutex>

class MySingleton {
 public:
  static std::shared_ptr<MySingleton> get() {
    std::lock_guard<std::mutex> lock(mutex_);
    if (instance_ == nullptr) {
      // 智能指针无法访问私有构造函数, std::make_shared()无法使用,
      // 需要自定义删除器，默认删除器无法使用
      instance_ = std::shared_ptr<MySingleton>(
          new MySingleton(), [](MySingleton* p) { delete p; });
    }
    return instance_;
  }

 private:
  MySingleton() { std::cout << "-- new MySingleton --" << std::endl; }
  ~MySingleton() { std::cout << "-- del MySingleton --" << std::endl; }
  MySingleton& operator=(const MySingleton&);
  MySingleton(const MySingleton&);
  static std::mutex mutex_;
  static std::shared_ptr<MySingleton> instance_;
};

std::shared_ptr<MySingleton> MySingleton::instance_ = nullptr;
std::mutex MySingleton::mutex_;