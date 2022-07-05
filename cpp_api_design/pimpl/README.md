# pimpl idiom
## pimpl advantage
```
1. Remove compilation dependencies on internal class implementations and improve compile times.
2. Hide the details of implementation.
```
## the example of pimpl idiom
```
如果文件没有使用到相关类的成员函数或者成员变量，那么可以使用前置声明，不需要include相关类的头文件。
files:
    pimpl.h:
    pimpl.cc:
    foo.h:
    foo.cc:

build:
    g++ -std=c++11 -fPIC -shared foo.cc impl.cc -o libfoo.so
    g++ -std=c++11 -fPIC -L./ -lfoo main.cc -o test
    export LD_LIBRARY_PATH=./

```