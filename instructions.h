
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
    map operators;
} *instruction_set;


enum operators {
    op_add = 1,
    op_jump = 2,
    op_condition = 3,
    op_print = 4,
    op_tagof = 5,
    op_length = 6,
    op_syscall = 7,
    op_move = 8,
    op_get = 9,
    op_allocate = 10,
};
    
