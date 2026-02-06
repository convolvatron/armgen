typedef u8 reg;

typedef struct context {
    region r;
    region temp; //inter-intstruction
    struct instruction_set *i;
    u64 tag_table;
} *context;
    

// add a declarative target  - at the minimum we'd like to capture register and immediate right now, but
// memories would be nice too
// this goes away for the most part? at least the instructions, they are indirected through oids
typedef struct instruction_set{
    reg arguments[6];
    reg link_register;
    reg equal_flag;
    instruction_definition *defs;

    string (*branch)(context, reg cond, string truth, string falsity);    // cant use this to implement a loop - probably for the best..this is really a ternary?
} *instruction_set;

struct instruction_definition {
    char *name;
    Oid oid;
    void (*f)()
} *instruction_definition;

instruction_definition instructions [] ={
    {"add", 1},
    {"jump", 2},
    {"condition", 3},
    {"print", 4},
    {"get_tag", 5},
    {"length", 6},
    {"syscall", 7},
    {"move", 8},
    {"get", 9},
    {"allocate", 10},
    {"equal", 11},            
}
