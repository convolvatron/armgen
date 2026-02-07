
#define get_method 0
#define iterate_method 1

typedef value reg;
typedef value boolean;
typedef value vector;

value one = (value)1;
value zero = (value)0;

#define true ((void *)(1))
#define false ((void *)(0))

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

static inline string sallocate(region r, bits length) {
    u64 *x = set_tag(allocate(r, length+64), tag_string);
    *x = length;
    return x;
}

static inline u64 to_number(value x) {
    if ((tag_of(x) == tag_immediate) || (tag_of(x) == tag_register) )
	return (u64)x;
    panic("unhandled to_number case");
}

#define string_contents(_x) ((tag_of(_x) == tag_immediate)?(u64 *)&_x:((u64 *)pointer_of(_x))+1)


// we have this region version .. just in case we change the type layout?
static inline value immediate(region r, u64 x) {
    return (value)x;
}

extern value map_internal(region, ...);
extern value map_string_internal(region, ...);
extern value vector_internal(region, ...);
#define map(_r, ...) map_internal(_r, __VA_ARGS__, NOT_A_VALUE)
#define new_vector(_r, ...) vector_internal(_r)

#define text_immediate(_x) ({static u8 m[sizeof(_x) + 8];\
    static int init =0;\
    if (!init) {							\
	for(int i = 1 ;i < sizeof(_x); i++) m[i]=_x[i];			\
	*(u64 *)m = (sizeof(_x) - 1) * 8;				\
	init = 1;\
     }						\
    set_tag(((value)m), tag_string);\
})

static inline value new_text(region r, char *x) {
    int len = 0;
    for (char *i = x; *i; i++, len+=8);

    string new = sallocate(r, len);
    u8 *dst = (u8 *)string_contents(new);
    for (char *i = x; *i; i++, len+=8) *dst++ = *i;
    return new;
}

static inline value signed_immediate(region r, s64 x) {
    if (x < 0)
	return map(text_immediate("negative"), true, text_immediate("value"), immediate(r, -x));
    return immediate(r, (u64)x);
}

static inline boolean is_number(value x) {
    // or all keys are numbers or 'negative' and all values are 1 or 0
    return (tag_of(x) == tag_immediate)?true:false;
}

static inline boolean is_negative(value x) {
    return to_boolean(to_number(is_number(x)) & to_number(get(x, text_immediate("negative"))));
}


static inline boolean equal(value a, value b) {
    return true;
}

/*
#define foreach(__i, __v) for (value _count = 0, _i ;\\
			       _count< length(__v);\\
			       _i = ((u64 *)contents_of(__v))[_count], _count = (value)(((u64) i) + count))
*/

string coerce_number(region r, string in, bits target) ;

static inline s64 from_signed(value x) {
    return 0;
}

#define concatenate(_r, ...) concatenate_internal(_r, __VA_ARGS__, NOT_A_VALUE)
#define new_map(_r, ...) map_internal(_r, __VA_ARGS__, NOT_A_VALUE)
#define new_map_string(_r, ...) map_string_internal(_r, __VA_ARGS__, NOT_A_VALUE)
string concatenate_internal(region r, ...);

string print(region r, string s);
string constant(region r, u64 value, bits length);

typedef value map;
