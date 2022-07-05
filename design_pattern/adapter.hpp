/*
 * 适配器类使用场景：适配器将接口转换为一个兼容的但不相同的接口
 * 1. 实现相同功能的类接口之间的转换；
 * 2. 或者是风格，使用习惯不同的接口之间的转换
 */

// 示例：矩阵表示，既可以对象的两个点的坐标表示，也可以使用左下角的点的坐标和宽w高h来表示

#include <iostream>
#include <memory>

class Rectangle {
 public:
  Rectangle() {}
  Rectangle(float x1, float y1, float x2, float y2)
      : x1_(x1), y1_(y1), x2_(x2), y2_(y2) {}
  ~Rectangle() {}

  void set_dimensions(float x1, float y1, float x2, float y2) {
    x1_ = x1;
    y1_ = y1;
    x2_ = x2;
    y2_ = y2;
  }

  void move(float right, float up) {
    x1_ += right;
    y1_ += up;
    x2_ += right;
    y2_ += up;
  }

  void print() {
    std::cout << "x1: " << x1_ << std::endl;
    std::cout << "y1: " << y1_ << std::endl;
    std::cout << "x2: " << x2_ << std::endl;
    std::cout << "y2: " << y2_ << std::endl;
  }

 private:
  float x1_;
  float y1_;
  float x2_;
  float y2_;
};

// 使用继承实现adapter
class RectangleAdapter1 : private Rectangle {
 public:
  RectangleAdapter1() {}
  RectangleAdapter1(float x, float y, float w, float h)
      : Rectangle(x, y, x + w, y + h) {}
  ~RectangleAdapter1() {}

  void set(float x, float y, float w, float h) {
    Rectangle::set_dimensions(x, y, x + w, y + h);
  }

  void move(float left, float down) { Rectangle::move(-left, -down); }

  void print() { Rectangle::print(); }
};

// 使用组合关联实现adapter
class RectangleAdapter2 {
 public:
  RectangleAdapter2() : rect_(std::make_shared<Rectangle>()) {}
  RectangleAdapter2(float x, float y, float w, float h)
      : rect_(std::make_shared<Rectangle>(x, y, x + w, y + h)) {}
  ~RectangleAdapter2() {}

  void set(float x, float y, float w, float h) {
    rect_->set_dimensions(x, y, x + w, y + h);
  }

  void move(float left, float down) { rect_->move(-left, -down); }

  void print() { rect_->print(); }

 private:
  std::shared_ptr<Rectangle> rect_;
};
