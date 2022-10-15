#include "adapter.hpp"
#include "factory.hpp"
#include "observer.hpp"
#include "proxy.hpp"
#include "singleton.hpp"

int main() {
  // 单例
  { auto singleton = MySingleton::get(); }
  { auto singleton = MySingleton::get(); }
  // 工厂
  { auto factory = MyFactory::create("apple"); }
  { auto factory = MyFactory::create("banana"); }

  { auto apple = std::make_shared<Apple>(); }

  {
    MyFactoryEx::register_fruit("apple", Apple::create);
    MyFactoryEx::register_fruit("banana", Banana::create);

    { auto apple = MyFactoryEx::get("apple"); }
    { auto apple = MyFactoryEx::get("apple"); }
    { auto banana = MyFactoryEx::get("banana"); }
  }

  // 适配器模式
  {
    RectangleAdapter1 rect_adapt(1, 2, 6, 7);
    rect_adapt.print();
    rect_adapt.move(3, 4);
    rect_adapt.print();
  }
  {
    RectangleAdapter2 rect_adapt(1, 2, 6, 7);
    rect_adapt.print();
    rect_adapt.move(3, 4);
    rect_adapt.print();
  }

  // 观察者模式
  {
    std::shared_ptr<Observer> observer = std::make_shared<Observer>();
    MySubject sub;
    sub.subscribe(MySubject::EVENT_1, observer);
    sub.subscribe(MySubject::EVENT_3, observer);

    sub.notify(MySubject::EVENT_3);
    sub.notify(MySubject::EVENT_1);
    sub.unsubscribe(MySubject::EVENT_1, observer);

    sub.notify(MySubject::EVENT_3);
    sub.notify(MySubject::EVENT_1);
  }

  WeatherService* service = new WeatherService();
  WeatherProxy* proxy = new WeatherProxy(service);
  proxy->get_temperature();
  proxy->get_wind();
}