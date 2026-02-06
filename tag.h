
typedef void *value;
#define tag_immediate 0
#define tag_string 1
#define tag_region 2
#define tag_mapvv 3
#define tag_register 4
#define tag_status 5 // maybe?
#define tag_memory 6
#define tag_text 7
#define tag_oid 8 // we're using a 64 bit subset today
#define tag_any (-2) //??
#define tag_empty (-1)

typedef u8 tag;
static inline value set_tag(value x, u64 tag) {
    return (void *)(((u64)x)|(tag<<56));
}

static inline tag tag_of(value x) {
    return (((u64)x) >> 56) & 127; 
}

static inline void *pointer_of(value x){
    return (void *)((u64)x & ((1ull<<56)-1));
}

// generated versions
