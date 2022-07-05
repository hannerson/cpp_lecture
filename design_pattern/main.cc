#include "factory.hpp"
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
}