#include <iostream>

template <typename T>
class ObjectCounter {
    public:
        ObjectCounter(){
            count_ ++;
        }
        ObjectCounter(const ObjectCounter<T>& other){
            count_ ++;
        }
        ObjectCounter<T>& operator=(const ObjectCounter<T>& other){
            count_ ++;
        }
        ~ObjectCounter(){
            count_ --;
        }

        int counter(){
            return count_;
        }
    private:
        static int count_;
};

template <typename T>
int ObjectCounter<T>::count_ = 0;

class A{

};


int main(){
    ObjectCounter<int> a, b;
    {
        auto c = a;
        auto d(b);
        std::cout << a.counter() << std::endl;
    }
    std::cout << a.counter() << std::endl;

    ObjectCounter<float> h,j;
    std::cout << h.counter() << std::endl;
    return 0;
}