#include <runtime.h>

vector vector_internal(region r, ...) {
    u64 args = argcount(r);
    value *v = allocate(r, args*sizeof(value)+1);
    v[0] = imm(args);    
    int count = 1;
    valargs(r, i) v[count++] = i;
    return v;
}


u64 vector_length(vector v) {
    return *(u64 *)v;
}

vector vector_get(value v, value k) {
    panic("vector get not implement");
}

boolean vector_equal(value a, value b)
{
    value *ap = pointer_of(a);
    value *bp = pointer_of(a);    
    u64 len = *(u64 *)ap;
    
    if (*(u64 *)ap == *(u64 *)bp) {
        for (int i; i < len; i++){
            value v1 = *++ap;
            value v2 = *++bp;            
            if (!equal(v1, v2)) return false;
        }
    }
    return true;
}
