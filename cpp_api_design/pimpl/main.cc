#include "foo.h"
#include <memory>

class foo;

int main(){
    std::shared_ptr<foo> pf(new foo());
    return 0;
}