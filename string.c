#include <b.h>


static void cat_one(string source, u64 *offset, u64 *working, u64 **base) {
    u64 *sbase = string_contents(source);
    u64 total = length(source);
    
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

string substring(region r, string source, u64 start_inc, u64 end_exc) {
    string new = sallocate(r, end_exc - start_inc);
    u64 dst = 0;

    // not..really...use cat_one
    for (u64 i = start_inc; i< end_exc; i++, dst++) {
	string_contents(new)[dst>>6] |= to_number(get(source, immediate(r, i)))<< (dst&63);
    }
    return new;
}

string concatenate_internal(region r, ...) {
    u64 total = 0;
    u64 count = 0;    
    valargs(r, i) {
        count ++; 
        total += length(i);
    }
    // so i can run them backwards
    string *temp = alloca(count*sizeof(string));
    int i = 0;
    valargs(r, source) {
        temp[i++] = source;
    }
    
    string new = sallocate(r, total);
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

string constant(region r, u64 value, bits length) {
    string new = sallocate(r, length);
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

string print(region r, string s) {
    u64 len = length(s);
    string new = sallocate(r, len*8);
    u8 *target = (u8 *)string_contents(new);
    for (s64 i=len-1; i>=0; i--) {
        *target++ = get(s, immediate(r, i))?'1':'0';
    }
    return new;
}

static char *hex_digits= "0123456789abcdef";

string print_hex(region r, string s) {
    u64 len = length(s);
    string new = sallocate(r, len*2+(len/32)*8);
    u8 *target = (u8 *)string_contents(new);
    for (s64 i=len-4; i >=0 ; i-=4) {
	string n = substring(r, s, i, i+4);
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

string coerce(region r, string in, bits target) {
    if (length(in) == target) {
        return in;
    }
    
    if (length(in) < target) {
        return concatenate(r, in, constant(r, 0, target-length(in)));
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
