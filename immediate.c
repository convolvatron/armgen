#include <runtime.h>
boolean equal_immediate(value a, value b) {
    return a==b?true:false;
}

utf8 print_immediate(region r, value a) {
    int count = 0;
    u64 v = (u64)a;    
    for (; v ; v/=10, count++);
    bitstring out = sallocate(r, count*8);
    char *target = (char *)string_contents(out);
    u64 v2 = (u64)a;
    for (int i = count; i>0 ; v2/=10, i--) target[i-1] = v2%10 + '0';
    return set_tag(out, tag_utf8);
}

struct representation immediate_representation = {
    "immediate",
    print_immediate,
    equal_immediate,
    0,
    0
};


    
