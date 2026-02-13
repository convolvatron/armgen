#include <runtime.h>
boolean equal_immediate(value a, value b) {
    return a==b?true:false;
}

utf8 print_immediate(region r, value a){
    return 0;
}

struct representation immediate_representation = {
    "immediate",
    print_immediate,
    equal_immediate,
    0,
    0
};


    
