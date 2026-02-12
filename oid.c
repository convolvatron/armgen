#include <runtime.h>

static boolean equal_oid(value a, value b) {
    return a==b?true:false;
}

static utf8 print_oid(region r, value a){
    return 0;
}

struct representation oid_representation = {
    print_oid,
    equal_oid,
    0,
    0
};
