#include <b.h>

typedef value location;
typedef value expression;
typedef string (*generator)(region r, ...);

static value regs[32];

// we only need to encode these once
#define encode_reg(_m, _n) regs[to_number(get(_m, text_immediate(##_n)))]
#define encode_number(_r, _m, _n, _sz) coerce_number(r, to_number(get(_m, text_immediate(##_n)), _sz))
#define encode_number_default(_r, _m, _n, _sz, _def) constant(r, to_number(get_default(_m, text_immediate(##_n), _def)), _sz)
#define size(_m) one

// reg d, u16 value, int offset, boolean zero)
//movz https://developer.arm.com/documentation/ddi0602/2025-12/Base-Instructions/MOVZ--Move-wide-with-zero-
static string move_immediate(region r, value parameters) {
    u16 offset = to_number(get_default(parameters, text_immediate("offset"), 0));    
    // "hw" encodes the amount by which to shift the immediate left, either 0 (the default), 16(1), 32(2) or 48(3),
    u64 hw = offset/16;
    if (hw*16 != offset) {
	panic("move immediate with unsupported offset");
    }
    
    return concatenate(r,
		       size(a),
		       imm(1),
		       constant(r, 0, 1), // why is this on a separate line?
		       constant(r, 0b100101, 6),
		       coerce_number(r, imm(hw), 2),
                       encode_number(r, a, "value", 16),
		       encode_reg( a, destination));
}

static string move_u64() {
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
string add_immediate(region r, value a) {
    // hardcoded internal shift, op and S flags (??)
    return concatenate(r,
                       constant(r, 0b1001000100, 10),
                       encode_number(r, a, "source2", 12),
                       encode_reg( a, "source"),
                       encode_reg( a, "destination"));                       
}

// ldr
//https://developer.arm.com/documentation/ddi0596/2021-06/Base-Instructions/LDR--immediate---Load-Register--immediate--
static string load(region r, map a) {
    reg offset = get_default(parameters, text_immediate("destination"), 0);
    return concatenate(r,
                       size(a), 
		       constant(r, 0b011100101, 9),
                       encode_number_default(r, parameters, "offset", 12, 0),
                       encode_reg(a, source),
                       encode_reg(a, destination));                       
}


// this is a family
// 0xd65f03c0
#define link_register 30
// should be imm and register demux
// return
https://developer.arm.com/documentation/ddi0602/2025-12/Base-Instructions/RET--Return-from-subroutine-
string jump(region r, value parameters) {
    reg target = get_default(parameters, text_immediate("target"), link_register);
    return concatenate(r,
                       constant(r, 0b1101011001011111000000, 22),
                       constant(r, target, 5),
                       constant(r, 0, 5));
}


string exception_generation(region r, u8 opc, u16 imm, u8 op2, u8 ll) {
    return concatenate(r, constant(r, 0b11010100, 8),
                       constant(r, opc, 3),
                       constant(r, imm, 16),
                       constant(r, op2, 3),
                       constant(r, ll, 2));
}


static string svc (region r, u16 opcode) {
    return exception_generation(r, 0, opcode, 0, 1);
}

// ubfm
static string extract(region r, reg dest, reg source, u64 from, u64 to) {
    return concatenate(r,
		       size(a),
                       constant(r, 0b10100110, 8),
		       constant(r, 0, 1), // in the 64 bit variant N is always zero
                       constant(r, from, 6),
                       constant(r, to, 6),
                       constant(r, to_number(dest), 5),
                       constant(r, to_number(source), 5));
}

// page 555 - how am i supposed to even manage these refernces
static string mov_reg(region r, map a) {
    return concatenate(r,
		       size(a),
                       constant(r, 0b0101010, 8),
		       constant(r, get_default(parameters, text_immediate("shift"), 0), 2),
                       constant(r, 0b0, 1),
                       encode_reg(a, source),
                       constant(r, 0, 6),
                       constant(r, 0b111111, 6),
                       encode_reg(a, destination));
}

// in rule land we should try to fold this into instructions that support
// internal shifts. 
string generate_shift(region r, map a) {
    if (is_negative(amount)) {
	s64 offset = from_signed(amount);
	return (extract(r, d, s, offset, 64-offset));
    }
    panic ("oddly, positive shifts aren't supported");
}

static string arm_get_tag(region r, reg dest, reg source) {
    return extract(r, dest, source, 56, 63);
}


static string move(region r, map parameters) {
    reg destination = get(parameters, text_immediate("destination"));
    reg source = get(parameters, text_immediate("destination"));
    if (is_reg(destination) && is_reg(source))  return mov_reg(r, parameters);
    if (is_reg(destination) && is_memory(source))  return load(r, parameters);
    if (is_memory(destination) && is_memory(source))  return store(r, parameters);
    if (is_memory(destination) && is_immediate(source))  move_immediate store(r, parameters);
    panic("unsuported move pair");
}

intruction_set arm_instruction_set(region r) {
    instruction_set s = allocate(r, sizeof(struct instruction_set));
    r->arguments = new_vector(0, 1, 2, 3, 4, 5);
    r->operators = map(r, op_add, set_tag(add_immediate, tag_function),
                       op_jump, set_tag(jump, tag_function),
                       op_syscall, set_tag(svc, tag_function),
                       op_tag, set_tag(arm_get_tag, tag_function));
    for (int i = 0; i < 32; i++) regs[i] = encode_constant(r, i, 5); 
}

