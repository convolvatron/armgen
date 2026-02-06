#include <b.h>
#include <stdio.h>
#include <unistd.h>



void output_ascii_line(string s) {
    write(1, (void *)string_contents(s), padlog(length(s), 3, 0));
    write (1, "\n", 1);
}

#define apply(_f)

#define instruction(_f)

int main(int argc, char **argv) {
    region r = mmap_region(malloc_region());
    value func = text_immediate("function");
    value add = text_immediate("add");
    value arg = text_immediate("add");        
    value reg0 = register_immediate(0);
    
    value p = map(func, add,
                  arg,  new_vector(r,
                                   apply(output_ascii_line),
                                   reg0, 
                                   new_vector(register_immediate(0))));
                  
    printf ("%p\n", execute(r, p, (void *)0xfull));
}

    
