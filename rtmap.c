#include <b.h>

// scan with a filter?
value mapvv_get(value m, value key)
{
    u64 *body = (u64 *)pointer_of(m);
    u64 count = *body;
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
	
    value new = allocate(r, (count + 1) *64, tag_mapvv);
    value *p = pointer_of(new);
    valargs(r, i) *p++ = i;
    return new;
}
