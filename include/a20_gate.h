#ifndef A20_GATE_H
#define A20_GATE_H

#include "./ioport.h"

#define A20_PORT 0x92

struct Far_ptr{
    unsigned short offset;
    unsigned short segment;
};

// static inline unsigned char far_peek(){
    
// }

static inline unsigned short stack_pointer_offset(){
    register unsigned short sp_off = 0;
    __asm__ __volatile__(
        "movw %%sp,%0\n\t"
        :"=a"(sp_off)
        ::"memory"
    );
    return sp_off;
}

// static void enable_a20(){
//     unsigned char al = in_byte(A20_PORT);
//     out_byte(A20_PORT, (al | 0b00000010));
// }

#endif