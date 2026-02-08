#include <b.h>
#include <stdarg.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <string.h>

void panic(char *x) {
    write(1, x, strlen(x));
    write(1, "\n", 1);
    exit(-1);
}


void *mallocate(region state, bits length) {
    return malloc(padlog(length, 3, 0));
}

region malloc_region() {
    region r = malloc(sizeof(region));
    *r = (void *)&mallocate;
    return r;
}

typedef struct mmap_region_state {
    void *(*r)(void *, bits);
    void *base;
    bits length, fill;
} *mmap_region_state;

void *mmap_allocate(void *r, bits length) {
    mmap_region_state s = r;
    if ((s->fill + length) > s->length) {
        s->base = mmap(0, PAGESIZE, PROT_READ| PROT_WRITE,
                       MAP_PRIVATE| MAP_ANONYMOUS, -1, 0);
        s->fill = 0;
    }
    void *new = s->base + (s->fill>>3);
    s->fill += length;
    return new;
}

region mmap_region(region r) {
    mmap_region_state new = set_tag(allocate(r, bitsizeof(struct mmap_region_state)), tag_region);
    new->r = mmap_allocate;
    new->base= 0;
    new->length = new->fill = 0;
    return (region)new;
}


string execute(region r, string instructions, value input)
{
    string code = concatenate(r, instructions);
    u64 clen = padlog(length(code), 3, 0);
    
    u32 *cs = (u32 *)(void *)(string_contents(code));
    //    for (u64 i = 0; i < (clen>>2); i++) 
    //        cs[i] = __builtin_bswap32(cs[i]);

    if (mprotect(code, clen, PROT_EXEC|PROT_READ)) {
        panic("mprotect set execution");
    }
    
    string (*n)(string) = (void *)cs;
    return n(input);
}

