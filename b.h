
#define PAGESIZE (1024*1024*8)

#define padlog(__x, __pad1, __pad2) ((((__x-1) >> __pad1) + 1) << __pad2)
#define bitsizeof(__x) (sizeof(__x)<<3)
typedef unsigned long u64;
typedef signed long s64;
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned u32;
typedef u64 bits;
typedef u64 *string;
typedef unsigned _BitInt(128) u128;
typedef u8 bit;


_Noreturn void panic(char *);

// struct -> deallocate
typedef void *(**region)(void *state, bits length);

#include <tag.h>

static inline void *allocate(region r, u64 length) {
    return (**r)((void *)r, length);
}

//static inline void deallocate(region r, u64 base, u64 length) {
//    (**r)((void *)r, length);
//}

#define NOT_A_VALUE ((void *)-1ull)

#include <value.h>
#include <apply.h>
#include <stdarg.h>

#define valargs(__start, __each) \
    for (value __each = zero; __each != NOT_A_VALUE;)  for (va_list __a; va_start(__a), __each != NOT_A_VALUE; ) for (;__each = va_arg(__a, value), __each != NOT_A_VALUE;)

#define argcount(__start) \
    ({int i = 0; for (value __each = zero; __each != NOT_A_VALUE;) i++; (i)})

#define min(_x, _y) (((_x) < (_y))?(_x):(_y))

#define concatenate(_r, ...) concatenate_internal(_r, __VA_ARGS__, NOT_A_VALUE)
#define map(_r, ...) map_internal(_r, __VA_ARGS__, NOT_A_VALUE)

string concatenate_internal(region r, ...);

string print(region r, string s);

string constant(region r, u64 value, bits length);

// a program is the address of a block entry point
typedef value program;

// unix
region mmap_region(region r);
region malloc_region();
string execute(region r, program instructions, value input);
#include <alloca.h>
u64 to_number(value x);
value get_default(value in, value key, value otherwise);

