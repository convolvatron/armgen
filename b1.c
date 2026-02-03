#include <b.h>
#include <stdio.h>
#include <unistd.h>



void output_ascii_line(string s) {
    write(1, (void *)string_contents(s), padlog(length(s), 3, 0));
    write (1, "\n", 1);
}

#define c_function(_x) imm(_x)

int main(int argc, char **argv) {
    region r = mmap_region(malloc_region());

    value p = map(text_immediate("function"), text_immediate("add"),
		  text_immediate("arguments"), new_vector(c_function(output_ascii_line),
							  register_immediate(0),
							  new_vector(register_immediate(0))));
    
    printf ("%p\n", execute(r, p, (void *)0xfull));
}

    
