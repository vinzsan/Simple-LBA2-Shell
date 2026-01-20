#ifndef VGA_DMA_H
#define VGA_DMA_H

#include "./ioport.h"

#define ROW_MAX 25
#define COL_MAX 80

static unsigned int POSITION = 0;
static unsigned short *vga_buffer = (unsigned short *)0xB8000;

static char scancode_to_ascii[128] = {
    0, 27, '1','2','3','4','5','6','7','8','9','0','-','=', '\b',
    '\t','q','w','e','r','t','y','u','i','o','p','[',']','\n',
    0, 'a','s','d','f','g','h','j','k','l',';','\'','`',
    0, '\\','z','x','c','v','b','n','m',',','.','/', 0,
    '*', 0,' '
};

static unsigned short putchars(char c,unsigned char color){
    return (((unsigned char)color << 8) | c);
}

static inline void set_cursor(unsigned char row,unsigned char col){
    //register unsigned short _pos asm("dx") = ((pos & 0x0000FFFF) | (pos >> 16) << 16) | ((pos & 0xFFFF0000) | (pos &0xFFFF));
    __asm__ __volatile__(
        "mov $0x02,%%ah\n\t"
        "xor %%bh,%%bh\n\t"
        "movb %0,%%dh\n\t"
        "movb %1,%%dl\n\t"
        "int $0x10\n\t"
        :
        :"r"(row),"r"(col)
        :"ax","bx"
    );
}

static inline void set_cursor_vga(short row,short col){
    unsigned int pos = row * COL_MAX + col;
    
    out_byte(0x3D4,0x0F);
    out_byte(0x3D5,(unsigned char)pos & 0xFF);
    
    out_byte(0x3D4, 0x0E);
    out_byte(0x3D5, (unsigned char)(pos >> 8) & 0xFF);
}

static inline void scroll_row(unsigned char color){
    for(unsigned short i = 0;i < COL_MAX * (ROW_MAX - 1);i++){
        vga_buffer[i] = vga_buffer[i + COL_MAX];
    }
    
    unsigned short last_row = COL_MAX * (ROW_MAX - 1);
    unsigned short blank = ((unsigned short)color << 8) | ' ';

    for(unsigned short i = 0;i < COL_MAX;i++){
        vga_buffer[last_row +i] = blank;
    }

    POSITION = ((ROW_MAX - 1) << 16) | 0;
}

static void print_vga(const char *str,unsigned char color){
    for(unsigned short i = 0;str[i] != '\0';i++){
        if(str[i] == '\n'){
            POSITION = ((POSITION & 0xFFFF0000));
            POSITION = ((POSITION & 0x0000FFFF) | ((POSITION >> 16) + 1) << 16);
            if((POSITION >> 16) >= ROW_MAX){
                scroll_row(color);
            }
            continue;
        }
        
        unsigned short idx = (POSITION >> 16) * COL_MAX + (POSITION & 0xFFFF);
        vga_buffer[idx] = putchars(str[i], color);
        
        POSITION = ((POSITION & 0xFFFF0000)) | (((POSITION & 0xFFFF) + 1));
        if((POSITION & 0xFFFF) >= COL_MAX){
            POSITION = ((POSITION & 0xFFFF0000));
            POSITION = ((POSITION & 0x0000FFFF) | ((POSITION >> 16) + 1) << 16);
            if((POSITION >> 16) >= ROW_MAX) {
                scroll_row(color);
            }
        }
        set_cursor_vga((unsigned char)(POSITION >> 16),(unsigned char)(POSITION & 0xFFFF));
    }
}

static unsigned char keyboard_read_scancode(){
    while(!(in_byte(0x64) & 1));
    return in_byte(0x60);
}

static inline unsigned char putchar_vga(char c,unsigned char color){
    if(c == '\0') return 0;
    if(c == '\n'){
        POSITION &= 0xFFFF0000;
        POSITION = (POSITION & 0x0000FFFF) | (((POSITION >> 16) + 1) << 16);
        if((POSITION >> 16) >= ROW_MAX){
            scroll_row(color);
        }
        set_cursor_vga((POSITION >> 16) + 1, POSITION & 0xFFFF);
        return 0;
    }

    vga_buffer[(POSITION >> 16) * COL_MAX + (POSITION & 0xFFFF)] = putchars(c, color);
    POSITION = ((POSITION & 0xFFFF0000)) | (((POSITION & 0xFFFF) + 1));
    if((POSITION & 0xFFFF) >= COL_MAX){
        POSITION &= 0xFFFF0000;
        POSITION = (POSITION & 0x0000FFFF) | (((POSITION >> 16) + 1) << 16);
        if((POSITION >> 16) >= ROW_MAX){
            scroll_row(color);
        }
    }
    set_cursor_vga(POSITION >> 16,POSITION & 0xFFFF);
    return 0;
}

static void clear_screen_vga(unsigned char color){
    for(short i = 0;i < ROW_MAX * COL_MAX;i++){
        vga_buffer[i] = ((unsigned char)color << 8) | ' ';
    }
    POSITION = 0;
    set_cursor_vga(0,0);
}

#endif