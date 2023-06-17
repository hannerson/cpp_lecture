#include "person.pb.h"
#include <memory>

using google::protobuf::Reflection;
using google::protobuf::Descriptor;

int main(){
    std::shared_ptr<test::Person> ptr = std::make_shared<test::Person>();
    ptr->set_name("Hanson");
    ptr->set_age(20);

    std::vector<uint8_t> data(ptr->ByteSize(), 0);
    ptr->SerializeToArray(data.data(), ptr->ByteSize());

    Descriptor* desc = const_cast<Descriptor*>(ptr->GetDescriptor());
    Reflection* refl = const_cast<Reflection*>(ptr->GetReflection());
    const google::protobuf::FieldDescriptor *field = desc->FindFieldByName("header");

    test::Head *f = (test::Head*)refl->MutableMessage((google::protobuf::Message*)ptr.get(), field);
    if(f == nullptr){
        std::cout << "null---" << std::endl;
    }
    std::cout << f->ByteSize() << std::endl;
    f->a();
    return 0;
}