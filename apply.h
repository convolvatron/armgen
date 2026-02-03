

static inline value apply(value (*f)(), vector arguments) {
    switch(length(arguments)/bitsizeof(value)) {
    case 0: return f();
    case 1: return f(get(arguments, 0));    
    case 2: return f(get(arguments, 0), get(arguments, imm(1)));
    case 3: return f(get(arguments, 0), get(arguments, imm(1)), get(arguments, imm(2)));
    case 4: return f(get(arguments, 0), get(arguments, imm(1)), get(arguments, imm(2)), get(arguments, imm(3)));
    case 5: return f(get(arguments, 0), get(arguments, imm(1)), get(arguments, imm(2)), get(arguments, imm(3)), get(arguments, imm(4)));	
    default:
	panic("insufficienctly expanded apply");
    }
}

