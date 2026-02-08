#include <runtime.h>

value partial_apply_internal(region r, instruction_set s, value in, ...) {
    let bindings = argcount() - 3;
    return in;
    
}

string generate_print(region r, reg source) {
    //syscall in x8
    // x0-x5 are arguments
    // x0 is return
    reg length = register_immediate(1);
    reg base = register_immediate(0);

    return concatenate(r,
		       gen_string_length(r, length, source),
		       // bits to bytes
		       generate_shift(r, length, length, signed_immediate(r, -3)),
		       // 
		       add_immediate(r, base, source, imm(8)),
		       move_u64(r, syscall_number_register, syscall_write)
		       );
}


string gen_string_length(region r, instruction_set s, value d, value s) {
    // seems like there is a further indirection here
    // and also a specialization in the immediate case
    if (tag_of(s) == tag_text) {
	return move_u64(r, d, length(s));
    }
    // use load
    panic("non-immediate string length!");
}

