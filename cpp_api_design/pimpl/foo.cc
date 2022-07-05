#include "foo.h"

#include <iostream>

#include "impl.h"

foo::foo() : pimpl(new impl()) {
  pimpl->do_internal_work();
  std::cout << "modify func:" << modify_data << std::endl;
}

foo::~foo() = default;