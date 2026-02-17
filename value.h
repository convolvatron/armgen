
#define get_method 0
#define iterate_method 1

typedef value reg;
typedef value boolean;
typedef value vector;
typedef value utf8;

#define true ((void *)(1))
#define false ((void *)(0))

extern value one;
extern value zero;

extern value get(value, value);
extern u64 length (value s);

#define imm(__x) ((void *)__x)

static value register_immediate(u64 i) {
    return set_tag(imm(i), tag_register);
}

    
static inline boolean to_boolean(u64 x)
{
    return x?true:false;
}

static inline bitstring sallocate(region r, bits length) {
    u64 *x = set_tag(allocate(r, length+64), tag_bitstring);
    *x = length;
    return x;
}

//nope!
#define string_contents(_x) ((tag_of(_x) == tag_immediate)?(u64 *)&_x:((u64 *)pointer_of(_x))+1)

u64 number_length (value s);
static inline u64 to_number(value x) {
    if (tag_of(x) == tag_immediate) {
        return (u64)x;
    }
    if (tag_of(x) == tag_bitstring) {
        u64 *p = pointer_of(x);
        if (number_length(x)> 63)  {
            panic("string integer coersion too big");
        }
        return (*string_contents(p));
    }
    panic ("unhandled tag in cast to number");
}

// we have this region version .. just in case we change the type layout?
static inline value immediate(region r, u64 x) {
    return (value)x;
}

extern value map_internal(region, ...);
extern value vector_internal(region, ...);
#define map(_r, ...) map_internal(_r, __VA_ARGS__, NOT_A_VALUE)
#define new_vector(_r, ...) vector_internal(_r)

// in bytes
static inline u64 utf8_length(value v) {return *(u64 *)v;}

#include <stdio.h>
#define utf8_immediate(_x) ({\
    static u8 m[sizeof(_x) + 8];    \
    static int init =0;\
    if (!init) {							\
	for(int i = 0 ;i < sizeof(_x); i++) m[i+8]=_x[i];		\
	*(u64 *)m = (sizeof(_x) - 1) * 8;				\
	init = 1;\
     }						\
    set_tag(((value)m), tag_utf8);\
})

static inline value new_text(region r, char *x) {
    int len = 0;
    for (char *i = x; *i; i++, len+=8);

    bitstring new = sallocate(r, len);
    u8 *dst = (u8 *)string_contents(new);
    for (char *i = x; *i; i++, len+=8) *dst++ = *i;
    return new;
}

static inline value signed_immediate(region r, s64 x) {
    if (x < 0)
	return map(utf8_immediate("negative"), true, utf8_immediate("value"), immediate(r, -x));
    return immediate(r, (u64)x);
}

static inline boolean is_number(value x) {
    // or all keys are numbers or 'negative' and all values are 1 or 0
    return (tag_of(x) == tag_immediate)?true:false;
}

static inline boolean is_negative(value x) {
    return to_boolean(to_number(is_number(x)) & to_number(get(x, utf8_immediate("negative"))));
}

boolean equal(value a, value b);
/*
#define foreach(__i, __v) for (value _count = 0, _i ;\\
			       _count< length(__v);\\
			       _i = ((u64 *)contents_of(__v))[_count], _count = (value)(((u64) i) + count))
*/

bitstring coerce_number(region r, bitstring in, bits target) ;

static inline s64 from_signed(value x) {
    return 0;
}

#define concatenate(_r, ...) concatenate_internal(_r, __VA_ARGS__, NOT_A_VALUE)
#define new_map(_r, ...) map_internal(_r, __VA_ARGS__, NOT_A_VALUE)
bitstring concatenate_internal(region r, ...);

utf8 print(region r, value v);
bitstring constant(region r, u64 value, bits length);
u64 bitstring_length(value v);

typedef value map;
#define is_negative(_x) false

typedef struct representation {
    char *name;
    utf8 (*print)(region r, value v);
    boolean (*equal)(value a, value b);
    value (*get)(value v, value k);
    bitstring (*serialize)(value v);
    value (*deserialize)(region r, bitstring b);
} *representation;
    

    
