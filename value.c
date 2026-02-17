#include <runtime.h>

value one = ((void *)1);
value zero = ((void *)0);

extern struct representation immediate_representation,
    bitstring_representation,
    mapvv_representation,
    utf8_representation,
    oid_representation,
    vector_representation;

static representation representations[TAG_MAX] = {
    &immediate_representation,
    &bitstring_representation,
    0, // region
    &mapvv_representation,
    0, // register
    0, // status
    0, // memory
    &utf8_representation,
    &oid_representation,
    0, // function
    &vector_representation,
};
    

value get_default(value in, value key, value otherwise) {
    value v = get(in, key);
    if (tag_of(v) == tag_empty) {
        return otherwise;
    }
    return v;
}

value get(value v, value key) {
    if (tag_of(v) > TAG_MAX) panic("invalid tag");
    representation r = representations[tag_of(v)];
    if (r == 0) panic("null representation");
    if (r->get == 0) panic("no get handler");
    return r->get(v, key);
}

value print(region m, value v) {
    if (tag_of(v) > TAG_MAX) panic("invalid tag");
    representation r = representations[tag_of(v)];
    if (r == 0) panic("null representation");
    if (r->print == 0) panic("no print handler");
    return r->print(m, v);
}

#include <stdio.h>

boolean equal(value a, value b) {
    if (tag_of(a)  == tag_of(b)){
        if (tag_of(a) > TAG_MAX) panic("invalid tag");
        representation r = representations[tag_of(a)];
        if (r == 0) panic("null representation");
        if (r->equal == 0) panic("no equal handler");
        return r->equal(a, b);
    }
    printf("missing equals case tags %d %d\n", tag_of(a), tag_of(b));
    return false;
    //    panic("missing generalized equality implementation");
}

