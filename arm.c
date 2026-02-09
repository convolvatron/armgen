#include <runtime.h>

typedef value location;
typedef value expression;
typedef bitstring (*generator)(region r, ...);

static value regs[32];
static value link_register;

// we only need to encode these once
#define encode_reg(_m, _n) regs[to_number(get(_m, utf8_immediate(#_n)))]
#define encode_number(_r, _m, _n, _sz) coerce_number(_r, get(_m, utf8_immediate(#_n)), _sz)
#define encode_number_default(_r, _m, _n, _sz, _def) constant(_r, to_number(get_default(_m, utf8_immediate(#_n), _def)), _sz)
#define size(_m) one

#define is_memory(_m) (tag_of(_m) == tag_memory)
#define is_reg(_r) (tag_of(_r) == tag_register)
#define is_immediate(_r) (tag_of(_r) == tag_immediate)

// reg d, u16 value, int offset, boolean zero)
//movz https://developer.arm.com/documentation/ddi0602/2025-12/Base-Instructions/MOVZ--Move-wide-with-zero-
static bitstring move_immediate(region r, value a) {
    u16 offset = to_number(get_default(a, utf8_immediate("offset"), 0));    
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
                       encode_number(r, a, value, 16),
		       encode_reg( a, destination));
}

static bitstring move_u64(region r, map a) {
    // type checking - schematime!
    reg destination = get(a, utf8_immediate("destination"));
    u64 val = to_number(get(a, utf8_immediate("value")));
    bitstring output = 0;
    for (int i = 0 ; i < 64; i += 16) {
        u64 slice = (val >> i) & 65535;
	if (slice) 
	    output = concatenate(r, output,
                                 move_immediate(r, map(r,
                                                       "destination", destination,
                                                       "zero", output?true:false,
                                                       "value", slice)));
    }
    return output;
}

// add
// https://developer.arm.com/documentation/ddi0596/2020-12/Base-Instructions/ADD--immediate---Add--immediate--
bitstring add_immediate(region r, value a) {
    // hardcoded internal shift, op and S flags (??)
    return concatenate(r,
                       constant(r, 0b1001000100, 10),
                       encode_number(r, a, "source2", 12),
                       encode_reg( a, "source"),
                       encode_reg( a, "destination"));                       
}

// ldr
//https://developer.arm.com/documentation/ddi0596/2021-06/Base-Instructions/LDR--immediate---Load-Register--immediate--
static bitstring load(region r, map a) {
    return concatenate(r,
                       size(a), 
		       constant(r, 0b011100101, 9),
                       encode_number_default(r, a, offset, 12, 0),
                       encode_reg(a, source),
                       encode_reg(a, destination));                       
}

// there are pre-and post index variants, which steal some bits from the offset
static bitstring store(region r, map a) {
    return concatenate(r,
                       one,
                       size(a), 
		       constant(r, 0b11100100, 9),
                       encode_number_default(r, a, "offset", 12, 0),
                       encode_reg(a, source),
                       encode_reg(a, destination));                       
}


// this is a family
// 0xd65f03c0
// should be imm and register demux
// return
//https://developer.arm.com/documentation/ddi0602/2025-12/Base-Instructions/RET--Return-from-subroutine-
bitstring jump(region r, value parameters) {
    reg target = get_default(parameters, utf8_immediate("target"), link_register);
    return concatenate(r,
                       constant(r, 0b1101011001011111000000, 22),
                       constant(r, to_number(target), 5),
                       constant(r, 0, 5));
}


bitstring exception_generation(region r, u8 opc, u16 imm, u8 op2, u8 ll) {
    return concatenate(r, constant(r, 0b11010100, 8),
                       constant(r, opc, 3),
                       constant(r, imm, 16),
                       constant(r, op2, 3),
                       constant(r, ll, 2));
}


static bitstring svc (region r, u16 opcode) {
    return exception_generation(r, 0, opcode, 0, 1);
}

// ubfm
static bitstring extract(region r, map a) {
    // reg dest, reg source, u64 from, u64 to    
    return concatenate(r,
		       size(a),
                       constant(r, 0b10100110, 8),
		       constant(r, 0, 1), // in the 64 bit variant N is always zero
                       encode_number_default(r, a, from, 6, 0),
                       encode_number_default(r, a, to, 6, imm(63)),
                       encode_reg(a, dest),
                       encode_reg(a, source));                       
}

// page 555 - how am i supposed to even manage these refernces
static bitstring mov_reg(region r, map a) {
    return concatenate(r,
		       size(a),
                       constant(r, 0b0101010, 8),
                       encode_number_default(r, a, shift, 2, 0),
                       constant(r, 0, 1),
                       encode_reg(a, source),
                       constant(r, 0, 6),
                       constant(r, 0b111111, 6),
                       encode_reg(a, destination));
}

// in rule land we should try to fold this into instructions that support
// internal shifts. 
bitstring generate_shift(region r, map a) {
    value bits = get(a, utf8_immediate("bits"));
    if (is_negative(bits)) {
	s64 offset = from_signed(bits);
        value d = utf8_immediate("destination");
        value s= utf8_immediate("source");        
	return extract(r, map(r, d, get(a, d),
                              s, get(a, s),
                              utf8_immediate("from"), offset,
                              utf8_immediate("to"), 64-offset));
    }
    panic ("oddly, positive shifts aren't supported");
}

//extract(r, dest, source, 56, 63);
static bitstring arm_get_tag(region r, map a) {
    value d = utf8_immediate("destination");
    value s= utf8_immediate("source");            
    return generate_shift(r, map(r,
                                 d, get(a, d),
                                 s, get(a, s),
                                 utf8_immediate("bits"), imm(56)));
}


static bitstring move(region r, map parameters) {
    reg destination = get(parameters, utf8_immediate("destination"));
    reg source = get(parameters, utf8_immediate("destination"));
    if (is_reg(destination) && is_reg(source))  return mov_reg(r, parameters);
    if (is_reg(destination) && is_memory(source))  return load(r, parameters);
    if (is_memory(destination) && is_memory(source))  return store(r, parameters);
    if (is_memory(destination) && is_immediate(source))  return move_immediate(r, parameters);
    panic("unsuported move pair");
}

// there is a memory-memory add!
static bitstring add(region r, map parameters) {
    reg destination = get(parameters, utf8_immediate("destination"));
    reg source = get(parameters, utf8_immediate("destination"));
    if (is_reg(destination) && is_reg(source))  return mov_reg(r, parameters);
    if (is_reg(destination) && is_memory(source))  return load(r, parameters);
    if (is_memory(destination) && is_memory(source))  return store(r, parameters);
    if (is_memory(destination) && is_immediate(source))  move_immediate(r, parameters);
    panic("unsuported move pair");
}

instruction_set arm_instruction_set(region r) {
    instruction_set s = allocate(r, sizeof(struct instruction_set));
    s->arguments = new_vector(r, 0, 1, 2, 3, 4, 5);
    s->operators = map(r,
                       op_add, set_tag(add_immediate, tag_function),
                       op_jump, set_tag(jump, tag_function),
                       op_syscall, set_tag(svc, tag_function),
                       op_tagof, set_tag(arm_get_tag, tag_function));
    link_register = register_immediate(30);
    for (int i = 0; i < 32; i++) regs[i] = constant(r, i, 5);
    return s;
}

