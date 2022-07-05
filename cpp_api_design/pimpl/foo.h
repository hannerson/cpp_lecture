#include <memory>

class impl;

class foo {
 public:
  foo();
  ~foo();
  foo& operator=(const foo&) = default;

 private:
  std::unique_ptr<impl> pimpl;
  // impl impl_member; // 编译错误， impl_member是不完整的类型，此时需要包含impl的声明
  int modify_data = 0;
};