#include <runtime.h>

// we're going to say this is true including zero extension. is this right?
// location has type information
boolean number_equal(value x, value y) {
    // is length static or dynamic? we're gonna have to ask my type
    // (min (length x) (length y))
    //
    return zero;
}

// is this..|keys|?
u64 length (value s) {
    if (tag_of(s) == tag_immediate) {
        if ((s == zero || (s == one))) {
            return 1;
        }
        return 56;
    }
    if (tag_of(s) == tag_bitstring) {
        u64 *x = pointer_of(s);
        return *x;
    }
    panic("length of unknown representation");
}


value number_get(value in, value key) {
    u64 keyv = to_number(key);
    u64 *base;
    
    if (tag_of(in) == tag_immediate) {
        base = (u64 *)&in;
    } else {
        if (tag_of(in) == tag_bitstring) {
            base = string_contents(in);
        }
    }

    if (keyv > length(in)) {
        return set_tag(0, tag_empty);
    }
    return (base[keyv>>6] & (1ull<<(keyv&63ull)))?one:zero;
}
