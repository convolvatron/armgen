#include <runtime.h>

value one = ((void *)1);
value zero = ((void *)0);


value get_default(value in, value key, value otherwise) {
    value v = get(in, key);
    if (tag_of(v) == tag_empty) {
        return otherwise;
    }
    return v;
}

value mapvv_get(value m, value key);
value vector_get(value m, value key);
value number_get(value m, value key);

value get(value in, value key) {
    switch (tag_of(in)) {
    case tag_mapvv:  return mapvv_get(in, key);
    case tag_vector:  return vector_get(in, key);
    case tag_bitstring:  return number_get(in, key);                        
    }
    panic("unknown get");
}
