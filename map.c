#include <runtime.h>
#include <string.h>

static value mapvv_get(value m, value key)
{
    value *body = (value *)pointer_of(m);
    u64 count = to_number(*body);
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
    *p++=imm(count);
    valargs(r, i) *p++ = i;
    return new;
}

static boolean mapvv_equal(value a, value b)
{
    value *ap = pointer_of(a);
    u64 len = *(u64 *)ap;
    if (len == *(u64 *)pointer_of(b)) {
        for (int i; i < len; i++){
            value name = *++ap;
            value value = *++ap;
            if (!equal(get(b, name), value)) return false;
        }
    }
    
    return true;
}

// this leaks something like n^2 memory!
utf8 print_map(region r, value m) {
    bitstring result = 0;
    value *body = (value *)pointer_of(m);
    u64 count = to_number(*body);
    body++;
    value comma = utf8_immediate(",");
    value colon = utf8_immediate(":");    
    
    for (u64 i = 0; i < count; i++, body++) 
        result = concatenate(r, result, i>0?((i&1)?comma: colon):zero, print(r, *body));
    
    return result;
}

struct representation mapvv_representation = {
    "map", 
    print_map,
    mapvv_get,
    mapvv_equal,
    0,
    0,
};
