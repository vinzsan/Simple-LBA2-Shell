#ifndef IOPORT_H
#define IOPORT_H

static inline unsigned char in_byte(unsigned short port){
    register unsigned char _return_value;
    __asm__ __volatile__(
        "inb %1,%0"
        :"=a"(_return_value)
        :"Nd"(port)
    );
    return _return_value;
}

static inline void out_byte(unsigned short port,unsigned char com){
    __asm__ __volatile__(
        "outb %0,%1\n\t"
        ::"a"(com),"Nd"(port)
        :
    );
}

/*
    NOTE : Shutdown qemu,tidak kompatible untuk board langsung.
*/

static inline void shutdown(){
    __asm__ __volatile__(
        "movw $0x2000,%%ax\n\t"
        "movw $0x604,%%dx\n\t"
        "outw %%ax,%%dx\n\t"
        :::"ax","dx"
    );
}

#endif