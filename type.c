#include <runtime.h>

typedef type value;

type vector_of(region r, type t) {
    return map(r);
}


vector vector_internal(region r, ...) {
    int args = argcount(r);
    vector v = sallocate(r, args*bitsizeof(value));
    int count = 1;
    valargs(r, i) v[count++] = i;
    return v;
}

