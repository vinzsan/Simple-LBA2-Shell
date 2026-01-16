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

static inline void pit_init(){
    unsigned char cmd = PIT_CH0 | PIT_LSB_MSB | PIT_MODE3 | PIT_BINARY;
    out_byte(PIT_PORT,cmd);

    out_byte(PIT_DATA,0xFFFF & 0xFF);
    out_byte(PIT_DATA,0xFFFF >> 8);
}

static inline unsigned short pit_counter(){
    out_byte(PIT_PORT, PIT_CH0 | PIT_LATCH);

    unsigned char MSB_ = in_byte(0x40);
    unsigned char LSB_ = in_byte(0x40);

    return (LSB_ << 8) | MSB_;
}

static void pit_sleep(unsigned int milis){
    unsigned int ticks = milis * PIT_HZ;
    unsigned short start,current;

    start = pit_counter();

    while(ticks){
        current = pit_counter();
        if(current <= start)
            ticks -= (start - current);
        else ticks -= (start + (0xFFFF - current));
        start = current;
    }
}

#endif