#include <runtime.h>


//typedef type value;

value one = ((void *)1);
value zero = ((void *)0);

/*type vector_of(region r, type t) {
    return map(r);
}
*/

vector vector_internal(region r, ...) {
    u64 args = argcount(r);
    void **v = (void **)sallocate(r, args*bitsizeof(value));
    int count = 1;
    v[count++] = (void *)args;
    valargs(r, i) v[count++] = i;
    return v;
}

