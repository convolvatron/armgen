#include <b.h>
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
    return g(r, get(term, text_immediate("arguments")));
}


map arm_generators(region r);

int main(int argc, char **argv) {
    region r = mmap_region(malloc_region());
    value reg0 = register_immediate(0);
    map generators = arm_generators(r);
    
    value p = concatenate(r,
                          generate(r, generators,
                                   new_map_string(r, "function", op_add, "arguments",  new_vector(r, reg0, reg0))),
                          generate(r, generators,
                                   new_map_string(r, "function", op_jump, "arguments", new_vector(r, set_tag(output_ascii_line, tag_function)))));
                  
    printf ("%p\n", execute(r, p, (void *)1ull));
}

    
