typedef u8 reg;

typedef struct context {
    region r;
    region temp; //inter-intstruction
    struct instruction_set *i;
    u64 tag_table;
} *context;
    

// add a declarative target  - at the minimum we'd like to capture register and immediate right now, but
// memories would be nice too
typedef struct instruction_set{
    reg arguments[6];
    reg link_register;
    reg equal_flag;
    value (*jump)(context, reg);
    value (*allocate)(context, reg);
    value (*add)(context, reg d, reg a, reg b); // (a, b)->value
    value (*equal)(context, reg a, reg b); // (a, b)->value
    value (*load)(context, reg dest, reg source);
    value (*load_tag)(context, reg dest, reg source);
    string (*branch)(context, reg cond, string truth, string falsity);    // cant use this to implement a loop - probably for the best..this is really a ternary?
} *instruction_set;
