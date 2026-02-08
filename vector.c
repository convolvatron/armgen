#include <b.h>

vector vector_internal(region r, ...) {
    u64 args = argcount(r);
    value *v = allocate(r, args*sizeof(value)+1);
    v[0] = imm(args);    
    int count = 1;
    valargs(r, i) v[count++] = i;
    return v;
}

