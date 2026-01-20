#ifndef PIT_CHANNEL_
#define PIT_CHANNEL_

#include "./ioport.h"

#define PIT_PORT 0x43
#define PIT_DATA 0x40
#define PIT_HZ 1193182

#define PIT_CH0      (0 << 6)
#define PIT_CH1      (1 << 6)
#define PIT_CH2      (2 << 6)

#define PIT_LATCH    (0 << 4)
#define PIT_LSB      (1 << 4)
#define PIT_MSB      (2 << 4)
#define PIT_LSB_MSB  (3 << 4)

#define PIT_MODE0    (0 << 1)
#define PIT_MODE2    (2 << 1)
#define PIT_MODE3    (3 << 1)

#define PIT_BINARY   (0 << 0)
#define PIT_BCD      (1 << 0)

static volatile unsigned int pit_ticks = 0;

static inline void pit_init(){
    register unsigned char cmd = PIT_CH0 | PIT_LSB_MSB | PIT_MODE3 | PIT_BINARY;
    out_byte(PIT_PORT,cmd);

    unsigned short divisor = 1193;
    out_byte(PIT_DATA,divisor & 0xFF);
    out_byte(PIT_DATA,divisor >> 8);
}

static inline unsigned short pit_counter(){
    out_byte(PIT_PORT, PIT_CH0 | PIT_LATCH);

    register unsigned char LSB_ = in_byte(0x40);
    register unsigned char MSB_ = in_byte(0x40);

    return (LSB_ << 8) | MSB_;
}

static void pit_sleep(unsigned int milis){
    register unsigned int ticks = milis * PIT_HZ;
    register unsigned short start = 0,current = 0;

    start = pit_counter();

    while(ticks){
        current = pit_counter();
        if(current <= start)
            ticks -= (start - current);
        else ticks -= (start + (0xFFFF - current));
        start = current;
    }
}

static inline void pit_init_irq(){
    out_byte(PIT_PORT, PIT_CH0 | PIT_LSB_MSB | PIT_MODE2 | PIT_BINARY);
    
    unsigned short divisor = 1193;
    out_byte(PIT_DATA, divisor & 0xFF);
    out_byte(PIT_DATA, divisor >> 8);
}

static inline void pit_irq_handler(){
    pit_ticks++;
}

static void pit_sleep_irq(unsigned int milis){
    unsigned long target = pit_ticks + milis;
    while(pit_ticks < target){
        __asm__ volatile("hlt");
    }
}

#endif