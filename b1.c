#include <runtime.h>
#include <stdio.h>
#include <unistd.h>

void output_utf8_line(utf8 s) {
    write(1, (void *)string_contents(s), padlog(utf8_length(s), 3, 0));
    write (1, "\n", 1);
}

typedef bitstring (*generator)(region, map);
    
bitstring generate(region r, map instructions, map term) {
    value result = zero;
    value function = get(term, utf8_immediate("function"));
    if (!function) panic("node with no function");
    generator g = (generator)pointer_of(get(instructions, function));
    bitstring out = g(r, get(term, utf8_immediate("arguments")));
    if (bitstring_length(out) != 32) {
        panic("weird gen");
    }
    return out;
}


instruction_set arm_instruction_set(region r);

    
int main(int argc, char **argv) {
    region r = mmap_region(malloc_region());
    value reg0 = register_immediate(0);
    instruction_set s = arm_instruction_set(r);

    value add = new_map(r,
                        utf8_immediate("function"), op_add,
                        utf8_immediate("arguments"), new_vector(r, reg0, reg0));

    value call= new_map(r, utf8_immediate("function"), op_jump,
                         utf8_immediate("arguments"), new_vector(r, set_tag(output_ascii_line, tag_function)));
    
    output_utf8_line(print(r, add));

    
    value p = concatenate(r,
                          generate(r, s->operators,add),
                          generate(r, s->operators, call));
                  
    printf ("%p\n", execute(r, p, (void *)1ull));
}

    
