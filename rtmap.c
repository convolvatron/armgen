#include <b.h>

// scan with a filter?
value mapvv_get(value m, value key)
{
    value *body = (value *)pointer_of(m);
    u64 count = cast_to_number(*body);
    body++;
    
    for (u64 i = 0; i < count; i++, body+=2) {
	if (equal(key, *body)) {
	    return (value)(*(body +1));
	}
    }
    return NOT_A_VALUE;
}

// look ma, no hash
value map_internal(region r, ...)
{
    u64 count = 0;
    valargs(r, i) count++;
	
    value new = set_tag(allocate(r, (count + 1) *64), tag_mapvv);
    value *p = pointer_of(new);
    valargs(r, i) *p++ = i;
    return new;
}
