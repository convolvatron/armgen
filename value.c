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

value mapvv_equal(value m, value key);
value vector_equal(value m, value key);
value number_equal(value m, value key);

boolean equal(value a, value b) {
    if (tag_of(a)  == tag_of(b)){
        switch (tag_of(a)) {
        case tag_mapvv:  return mapvv_equal(a, b);
        case tag_vector:  return vector_equal(a, b);
        case tag_bitstring:  return number_equal(a, b);                        
        }
        panic("unknown equals");
    }
    panic("missing generalized equality implementation");
}
