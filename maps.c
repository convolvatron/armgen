#include <runtime.h>
#include <instructions.h>
// a map for me 
u64 entry_points

// ooh! bfi bit field insert
//      bfe bit field extract

string generate_get(x, y) {
    
}

string generate_tagof(context c, reg dest, reg source) {
    concatenate(c->r, c->i->shift_right());
}

// jumptable?
string generate_equal(context c, x, y) {
    reg reg0 = 7;
    concatenate(generate_tagof(c, x, reg0),
                branch(c, equal()
                       branch));
    
    
}

