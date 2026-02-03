#include <b.h>


// is this..|keys|?
u64 length (value s) {
    if (tag_of(s) == tag_immediate) {
        if ((s == zero || (s == one))) {
            return 1;
        }
        return 56;
    }
    if (tag_of(s) == tag_string) {
        u64 *x = pointer_of(s);
        return *x;
    }
    panic("length of unknown representation");
}

u64 cast_to_number(value x) {
    if (tag_of(x) == tag_immediate) {
        return (u64)x;
    }
    if (tag_of(x) == tag_string) {
        u64 *p = pointer_of(x);
        if (length(x)> 63)  {
            panic("string integer coersion too big");
        }
        return (*string_contents(p));
    }
    panic ("unhandled tag in cast to number");
}

// cps this?
value get(value in, value key) {
    u64 keyv = cast_to_number(key);
    u64 *base;
    
    if (tag_of(in) == tag_immediate) {
        base = (u64 *)&in;
    } else {
        if (tag_of(in) == tag_string) {
            base = string_contents(in);
        }
    }

    if (keyv > length(in)) {
        panic("bad get");
    }
    return (base[keyv>>6] & (1ull<<(keyv&63ull)))?one:zero;
}
