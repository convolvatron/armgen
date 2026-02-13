#include <runtime.h>


/*
static boolean vstrcmp(char *x, utf8 a) {
    int i = 0;
    u8 *ab = (u8*)string_contents(a);
    for (;x[i];i++);
    if ((i * 8) != length(a)) return false;
    for (int j = 0;j < i; j++) if (ab[j] != x[j]) return false;
    return true;
}
*/

static value utf8_get(value v, value k) {
    return 0;
}

#include <string.h>
static boolean utf8_equal(value a, value b){
    u64 *ap = (u64*) a;
    u64 *bp = (u64*) b;
    u64 len = *ap++;
    
    if (len != *bp++) return false;
    return (memcmp(ap, bp, len) == 0)?true:false;
}

static utf8 utf8_print(region r, value a) {
    
    return a;
}

struct representation utf8_representation = {
    "utf8",
    utf8_print,    
    utf8_equal,
    utf8_get,
    0,
    0
};
