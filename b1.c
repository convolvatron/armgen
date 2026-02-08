#include <runtime.h>
#include <stdio.h>
#include <unistd.h>



void output_ascii_line(string s) {
    write(1, (void *)string_contents(s), padlog(length(s), 3, 0));
    write (1, "\n", 1);
}


typedef string (*generator)(region, map);
    
string generate(region r, map instructions, map term) {
    value result = zero;
    value function = get(term, text_immediate("function"));
    if (!function) panic("node with no function");
    generator g = (generator)pointer_of(get(instructions, text_immediate("generator")));
    string out = g(r, get(term, text_immediate("arguments")));
    if (length(out) != 32) {
        panic("weird gen");
    }
    return out;
}


instruction_set arm_instruction_set(region r);

int count (...) {
    /*        int i = 0;
        va_list __a;         
        for (va_start(__a); va_arg(__a, value) != NOT_A_VALUE; i++); 
        return i;*/
    return argcount();
        
}
    
int main(int argc, char **argv) {
    printf(" count: %d\n", count(1, 2, 3, 4, 5, NOT_A_VALUE));
    
    region r = mmap_region(malloc_region());
    value reg0 = register_immediate(0);
    instruction_set s = arm_instruction_set(r);


    value p = concatenate(r,
                          generate(r, s,
                                   new_map_string(r, "function", op_add, "arguments",
                                                  new_vector(r, reg0, reg0))),
                          generate(r, s,
                                   new_map_string(r, "function", op_jump, "arguments",
                                                  new_vector(r, set_tag(output_ascii_line, tag_function)))));
                  
    printf ("%p\n", execute(r, p, (void *)1ull));
}

    
