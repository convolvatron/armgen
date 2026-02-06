#include <b.h>

typedef value location;
typedef value expression;
typedef string (*generator)(region r, ...);

// we think this defines whether the operands are 32 or 64 bits
#define SIZE coerce_number(r, true, 1)

// reg d, u16 value, int offset, boolean zero)
//movz https://developer.arm.com/documentation/ddi0602/2025-12/Base-Instructions/MOVZ--Move-wide-with-zero-
static string move_immediate(region r, value parameters) {
    reg destination = get(parameters, text_immediate("destination"));
    u16 val = to_number(get(parameters, text_immediate("value")));
    u16 offset = to_number(get_default(parameters, text_immediate("offset"), 0));    
    // "hw" encodes the amount by which to shift the immediate left, either 0 (the default), 16(1), 32(2) or 48(3),
    u64 hw = offset/16;
    if (hw*16 != offset) {
	panic("move immediate with unsupported offset");
    }
    
    return concatenate(r,
		       SIZE,
		       imm(1),
		       constant(r, 0, 1), // why is this on a separate line?
		       constant(r, 0b100101, 6),
		       coerce_number(r, imm(hw), 2),
                       constant(r, val, 16),	
		       constant(r, to_number(destination), 5));
}

static string move_u64(region r, value parameters) {
    // type checking - schematime!
    reg destination = get(parameters, text_immediate("destination"));
    u64 val = to_number(get(parameters, text_immediate("value")));
    string output = 0;
    for (int i = 0 ; i < 64; i += 16) {
	u64 slice = (val >> i) & 65535;
	if (slice) {
	    string m = move_immediate(r, map(r,
                                             "destination", destination,
                                             "zero", output?true:false,
                                             "value", slice));
	    output = concatenate(r, output, m);
	}
    }
    return output;
}

// add
// https://developer.arm.com/documentation/ddi0596/2020-12/Base-Instructions/ADD--immediate---Add--immediate--
string add_immediate(region r, value parameters) {
    reg destination = get(parameters, text_immediate("destination"));
    reg srouce = get(parameters, text_immediate("source"));    
    u64 val = to_number(get(parameters, text_immediate("value")));    
    // hardcoded internal shift, op and S flags (??)
    return concatenate(r, constant(r, 0b1001000100, 10),
                       coerce_number(r, val, 12),
                       // why constant v coerce?
                       constant(r, to_number(source), 5),
                       constant(r, to_number(destination), 5));		       
}

// ldr
//https://developer.arm.com/documentation/ddi0596/2021-06/Base-Instructions/LDR--immediate---Load-Register--immediate--
string load(region r, value d, value s, value offset, value shift) {
    reg destination = get(parameters, text_immediate("destination"));
    u64 val = get(parameters, text_immediate("value"));
    reg offset = get_default(parameters, text_immediate("destination"), 0);
    u64 val = get_default(parameters, text_immediate("value"), 0);    
    return concatenate(r,
		       constant(r, 0b10111000011, 12),
		       constant(r, to_number(offset), 5),
		       constant(r, to_number(shift), 1));
}


// this is a family
// 0xd65f03c0
#define link_register 30
// should be imm and register demux
string jump(region r, value parameters) {
    reg target = get_default(parameters, text_immediate("target"), link_register);
    return concatenate(r,
                       constant(r, 0b1101011001011111000000, 22),
                       constant(r, 30, 5),
                       constant(r, 0, 5));
}


string exception_generation(region r, u8 opc, u16 imm, u8 op2, u8 ll) {
    return concatenate(r, constant(r, 0b11010100, 8),
                       constant(r, opc, 3),
                       constant(r, imm, 16),
                       constant(r, op2, 3),
                       constant(r, ll, 2));
}


string svc (region r, u16 opcode) {
    return exception_generation(r, 0, opcode, 0, 1);
}

string gen_string_length(region r, value d, value s) {
    // seems like there is a further indirection here
    // and also a specialization in the immediate case
    if (tag_of(s) == tag_text) {
	return move_u64(r, d, length(s));
    }
    // use load
    panic("non-immediate string length!");
}

// ubfm
static string extract(region r, reg dest, reg source, u64 from, u64 to) {
    return concatenate(r,
		       SIZE,
                       constant(r, 0b10100110, 8),
		       constant(r, 0, 1), // in the 64 bit variant N is always zero
                       constant(r, from, 6),
                       constant(r, to, 6),
                       constant(r, to_number(dest), 5),
                       constant(r, to_number(source), 5));
}

// in rule land we should try to fold this into instructions that support
// internal shifts. 
string generate_shift(region r, reg d, reg s, value amount) {

    if (is_negative(amount)) {
	s64 offset = from_signed(amount);
	return (extract(r, d, s, offset, 64-offset));
    }
    panic ("oddly, positive shifts aren't supported");
}

#define syscall_write 64
// linux!
#define syscall_number_register 8
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
		       move_u64(r, register_immediate(syscall_number_register), syscall_write)
		       );
}


string arm_get_tag(region r, reg dest, reg source) {
    return extract(r, dest, source, 56, 63);
}

typedef struct encoder  {
    char *name;
    generator g;
    tag arguments[10];
} *encoder;

// instructions are objects not names
struct encoder arm64_encoders[] = {
    {"add", (void *)add_immediate, {tag_register, tag_register, tag_immediate, tag_empty}},
    {"print", (void *)generate_print, {tag_register, tag_empty}},    
};


boolean compare_signature(vector args, tag accepts[]) {
    return true;
}


program generate_arm(region r, expression e) {
    value result = zero;
    value function = get(e, text_immediate("function"));
    value args = get(e, text_immediate("arguments"));
    if (!function) panic("node with no function");
    for (int i = 0 ; i < sizeof(arm64_encoders)/sizeof(struct encoder); i++) {
	encoder g = arm64_encoders + i;
	if (vstrcmp(g->name, function))
	    if (compare_signature(args, g->arguments)) {
		// n^2 :(
		result = concatenate(r, apply((void *)g->g, args));
	    }
    }
    return result;
}

instruction_set bind_generators(map root) {
    bind("add", add_immediate);
}

