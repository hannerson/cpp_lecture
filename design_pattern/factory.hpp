#include <iostream>
#include <map>
#include <memory>
#include <mutex>

class Fruit {
 public:
  Fruit() : name_("fruit") {
    std::cout << "~ create " << name_ << " ~" << std::endl;
  }
  virtual ~Fruit() { std::cout << "~ delete " << name_ << " ~" << std::endl; }

 protected:
  std::string name_;
};

class Apple : public Fruit {
 public:
  Apple() : name_("apple") {
    std::cout << "~ create " << name_ << " ~" << std::endl;
  }
  ~Apple() { std::cout << "~ delete " << name_ << " ~" << std::endl; }

  static std::shared_ptr<Fruit> create(){
    return std::make_shared<Apple>();
  }

 private:
  std::string name_;
};

class Banana : public Fruit {
 public:
  Banana() : name_("banana") {
    std::cout << "~ create " << name_ << " ~" << std::endl;
  }
  ~Banana() { std::cout << "~ delete " << name_ << " ~" << std::endl; }

  static std::shared_ptr<Fruit> create(){
    return std::make_shared<Banana>();
  }

 private:
  std::string name_;
};

// 基本工厂：每增加一个类型，都需要修改create函数，增加对应类型的创建分支
// 工厂模式
class MyFactory {
 public:
  static std::shared_ptr<Fruit> create(const std::string& name) {
    if (name == "apple") {
      return std::make_shared<Apple>();
    } else if (name == "banana") {
      return std::make_shared<Banana>();
    }
  }
};

// 扩展工厂
class MyFactoryEx {
 public:
  typedef std::function<std::shared_ptr<Fruit>()> callback_t;
  static void register_fruit(const std::string& name,
                                               callback_t callback) {
    std::lock_guard<std::mutex> lock(mutex_);
    create_map_.insert({name, callback});
  }
  static void unregister_fruit(const std::string& name) {
    std::lock_guard<std::mutex> lock(mutex_);
    create_map_.erase(name);
  }

  static std::shared_ptr<Fruit> get(const std::string& name){
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = create_map_.find(name);
    if(it != create_map_.end()){
      return it->second();
    }
    return nullptr;
  }

 private:
  static std::mutex mutex_;
  static std::map<std::string, callback_t> create_map_;
};

std::mutex MyFactoryEx::mutex_;
std::map<std::string, MyFactoryEx::callback_t> MyFactoryEx::create_map_;

// 抽象工厂模式：创建工厂的工厂