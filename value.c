#include <b.h>

value one = ((void *)1);
value zero = ((void *)0);

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


value get(value in, value key) {
    u64 keyv = to_number(key);
    u64 *base;
    
    if (tag_of(in) == tag_immediate) {
        base = (u64 *)&in;
    } else {
        if (tag_of(in) == tag_string) {
            base = string_contents(in);
        }
    }

    if (keyv > length(in)) {
        return set_tag(0, tag_empty);
    }
    return (base[keyv>>6] & (1ull<<(keyv&63ull)))?one:zero;
}

value get_default(value in, value key, value otherwise) {
    value v = get(in, key);
    if (tag_of(v) == tag_empty) {
        return otherwise;
    }
    return v;
}
