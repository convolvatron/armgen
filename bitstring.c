#include <runtime.h>

u64 bitstring_length(value s) {
    if (tag_of(s) == tag_bitstring) {
        u64 *x = pointer_of(s);
        return *x;
    }
    panic("not a bitstring");
}

static void cat_one(bitstring source, u64 *offset, u64 *working, u64 **base) {
    u64 *sbase = string_contents(source);
    u64 total = bitstring_length(source);
    
    while (total) {
        u64 xfer = min(total, 64-*offset);
        u64 sword = *sbase++;
        *working |= sword << (*offset);
        if ((*offset += xfer) == 64) {
            **base = *working;
	    *base = *base+1;
            *working = sword >> (64 - *offset);
            *offset = 0;
        }
        total -= xfer;
    }
}

bitstring substring(region r, bitstring source, u64 start_inc, u64 end_exc) {
    bitstring new = sallocate(r, end_exc - start_inc);
    u64 dst = 0;

    // not..really...use cat_one
    for (u64 i = start_inc; i< end_exc; i++, dst++) {
	string_contents(new)[dst>>6] |= to_number(get(source, immediate(r, i)))<< (dst&63);
    }
    return new;
}

bitstring concatenate_internal(region r, ...) {
    u64 total = 0;
    u64 count = 0;    
    valargs(r, i) {
        count ++; 
        total += bitstring_length(i);
    }
    // so i can run them backwards
    bitstring *temp = alloca(count*sizeof(bitstring));
    int i = 0;
    valargs(r, source) {
        temp[i++] = source;
    }
    
    bitstring new = sallocate(r, total);
    u64 *base = string_contents(new);
    u64 offset = 0, working = 0;

    // vector foreach
    for (int i = count-1; i >=0; i--){
	cat_one(temp[i], &offset, &working, &base);
    }

    if (offset >0) {
        *base++ = working;
    }
    return new;
}

bitstring constant(region r, u64 value, bits length) {
    bitstring new = sallocate(r, length);
    *string_contents(new) = value;
    return new;
}


/*bit string_get(string s, u64 index) {
    if ((s == one) || (s == zero)) {
      return ((s == one)  && (index == 0))?1:0;
    }
    return !!((string_contents(s))[index>>6]&(1ull<<(index&63)));
}
*/

utf8 print(region r, bitstring s) {
    u64 len = bitstring_length(s);
    utf8 new = sallocate(r, len*8);
    u8 *target = (u8 *)string_contents(new);
    for (s64 i=len-1; i>=0; i--) {
        *target++ = get(s, immediate(r, i))?'1':'0';
    }
    return new;
}

static char *hex_digits= "0123456789abcdef";

utf8 print_hex(region r, bitstring s) {
    u64 len = bitstring_length(s);
    bitstring new = sallocate(r, len*2+(len/32)*8);
    u8 *target = (u8 *)string_contents(new);
    for (s64 i=len-4; i >=0 ; i-=4) {
	bitstring n = substring(r, s, i, i+4);
	// macro
	int nib = *string_contents(n);
        *target++ = hex_digits[nib];
	if (!(i % 128)) {
	    *target++ = '\n';
	} else {
	    if (!(i % 32)) *target++ = ' ';
	}
    }
    return new;
}

// why isn't this just constant? we should be checking for overflow regardless
bitstring coerce_number(region r, bitstring in, bits target) {
    int len = bitstring_length(in);
    if (len == target) {
        return in;
    }
    
    if (len < target) {
        return concatenate(r, in, constant(r, 0, target-len));
    }

    u64 result;
            
    for (u64 i=0; i < target; i++) {
        value some;
        if (((some = get(in, (value)i)) != imm(0)) && (i >= target)){
            // fix and use another instruction
            panic("discarding 1 bits in instruction selection truncatation");
        } else {
            result |= 1<<i;
        }
    }
    return constant(r, result, target);

}

/*
static boolean vstrcmp(char *x, utf8 a) {
    int i = 0;
    u8 *ab = (u8*)string_contents(a);
    for (;x[i];i++);
    if ((i * 8) != length(a)) return false;
    for (int j = 0;j < i; j++) if (ab[j] != x[j]) return false;
    return true;
}
*/
