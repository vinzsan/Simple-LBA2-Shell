#ifndef INT_H 
#define INT_H

static inline void clear(){
    __asm__ __volatile__(
        "mov $0x0003,%%ax\n\tint $0x10\n\t"
        :::"ax"
    );
}

#endif