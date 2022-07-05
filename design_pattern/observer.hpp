/*
 *              观察者模式
 * 场景：一个对象A想要调用另一个对象B的方法（包含B，持有B的一个实例，造成A和B的紧耦合）
 * 1.不持有被观察对象
 * 2.支持组件解耦，避免循环依赖
 * 3.Subject和Observer（发布和订阅者），一个或者多个观察者注册subject中感兴趣的事件，
 *    之后subject会在自身状态发生变化时，通知所有注册的观察者。
 *
 */

#include <iostream>
#include <map>
#include <memory>
#include <mutex>
#include <vector>

class IObserver {
 public:
  virtual ~IObserver() {}
  virtual void update(int event) = 0;
};

class ISubject {
 public:
  virtual ~ISubject() {}
  virtual void subscribe(int event, std::shared_ptr<IObserver> observer) {
    std::lock_guard<std::mutex> lock(mutex_);
    // observer_map_.emplace(event, observer);
    if(observer_map_.count(event) == 0){
        observer_map_.emplace(event, observer_list_t());
    }
    observer_map_[event].push_back(observer);
  }

  virtual void unsubscribe(int event, std::shared_ptr<IObserver> observer) {
    std::lock_guard<std::mutex> lock(mutex_);
    observer_map_.erase(event);
  };

  virtual void notify(int event) {
    observer_list_t observer_list;
    {
      std::lock_guard<std::mutex> lock(mutex_);
      if (observer_map_.count(event) > 0) {
        observer_list = observer_map_[event];
      }
    }
    for (auto observer : observer_list) {
      observer->update(event);
    }
  };

 private:
  typedef std::vector<std::shared_ptr<IObserver>> observer_list_t;
  typedef std::map<int, observer_list_t> observer_map_t;
  observer_map_t observer_map_;
  std::mutex mutex_;
};

class MySubject : public ISubject {
 public:
  enum MyEvent { EVENT_1 = 0, EVENT_2, EVENT_3 };
  MySubject() {}
  ~MySubject() {}
};

class Observer : public IObserver {
 public:
  Observer() {}
  ~Observer() {}
  void update(int event) override {
    std::cout << "receive event: " << event << std::endl;
  }
};
