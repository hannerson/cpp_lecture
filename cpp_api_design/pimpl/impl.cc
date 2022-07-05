#include "impl.h"

#include <iostream>

void impl::do_internal_work() {
  std::cout << "internal work: data=" << inter_data << std::endl;
}