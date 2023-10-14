#include "intrusive-ptr.h"

struct object : intrusive_ref_counter<object> {};

int main() {

    intrusive_ptr<object> ptr;

    return 0;
}
