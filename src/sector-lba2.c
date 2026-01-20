//#include "../include/pit_channel.h"
//#include "../include/ioport.h"
#include "../include/vga_dma.h"
#include "../include/a20_gate.h"
#include "../include/string.h"
#include "../include/interrupt.h"

//#include "../include/igdt.h"

__asm__(".code16gcc");

#define ROW_MAX 25
#define COL_MAX 80
#define CHAR_BUFFER_SIZE 1024

static char BOOT_DRIVE = 0;
char char_buffer[CHAR_BUFFER_SIZE] = {0};

/*

    END BIOS INT caller
    INFO : Karena switch ke VGA buffer untuk proses visual
    jadi tidak boleh mencampur aduk menggunakan Interrupt BIOS,
    untuk ekosistem jangka panjang misal setelah switch ke 32 bit mode
    tidak ada layanan BIOS,cuman ini masih stay di 16 bit mode.

    Layanan PIT tidak berfungsi dan masih crash,terkadang IRQ terganggu,dan tidak
    Jadi beberapa Header yang di comment memang masih belum layak di gunakan,dan
    kode semakin berantakan.
    
    Shutdown tidak bisa di gunakan untuk board asli,karena hanya reserve dari
    IN OUT milik QEMU di port 0x604/0x2000

    Do what you want.
    
*/

__attribute__((section(".boot_entry")))
void _start(){
    __asm__ __volatile__(
        "cli\n\t"
        "movb %%dl,(%0)\n\t"
        "xor %%ax,%%ax\n\t"
        "mov %%ax,%%es\n\t"
        "mov %%ax,%%ds\n\t"
        "mov %%ax,%%ss\n\t"
        "mov $0x9000,%%sp\n\t"
        "sti\n\t"
        :"=memory" (BOOT_DRIVE)
        :
        : "ax"
    );
    clear();
    
    //pit_init_irq();
    print_vga("Welcome to x86_16 shell\n", 0x1A);
    unsigned short line = 0;
    unsigned char color_change = 0x0F;
    while(1){
        print_vga("Shell ", color_change);
        putchar_vga('[',0x0F);
        print_vga(short2char_arr(line),0x0A);
        print_vga("] >> ", 0x0F);
        unsigned int cursor_len = 0;
        while(1){
            unsigned char sc = keyboard_read_scancode();
            if(sc & 0x80) continue;
            
            char c = scancode_to_ascii[sc];
            
            if(c == '\n'){
                POSITION &= 0xFFFF0000;
                POSITION = (POSITION & 0x0000FFFF)
                         | (((POSITION >> 16) + 1) << 16);

                if((POSITION >> 16) >= ROW_MAX){
                    scroll_row(0x0F);
                }
                set_cursor_vga(POSITION >> 16, 0);
                break;
            }
        
            if(c == '\b' && cursor_len != 0){
                if(cursor_len == 0){
                    continue;
                }

                cursor_len--;

                if((POSITION & 0xFFFF) > 0){
                    POSITION = (POSITION & 0xFFFF0000)
                             | ((POSITION & 0xFFFF) - 1);
                
                    vga_buffer[(POSITION >> 16) * COL_MAX
                             + (POSITION & 0xFFFF)] = putchars(' ', 0x0F);
                    
                    //set_cursor(POSITION >> 16, POSITION & 0xFFFF);
                    set_cursor_vga(POSITION >> 16,POSITION & 0xFFFF);
                }
            }
        
            if(c && c != '\b' && cursor_len < sizeof(char_buffer)){
                char_buffer[cursor_len++] = c;
                putchar_vga(c, 0x0F);
            }
        }
        char_buffer[cursor_len] = '\0';
        if(!charcmp(char_buffer, "print")){
            print_vga("Hello world\n", 0x2F);
        }
        else if(!charcmp(char_buffer,"clear")){
            clear_screen_vga(0x00);
            line = 0;
        }
        else if(!charcmp(char_buffer,"shutdown")){
            shutdown();
        }
        else if(!charcmp(char_buffer,"help")){
            print_vga("x86_16 Simple Shell\nversion 0.1 (debug mode)\n\n \
            -print      for show hello world text\n \
            -shutdown   for shutdown virtual (not APM)\n \
            -clear      for clearthe terminal\n \
            \n",0x0F);
        }
        else if(!charcmp(char_buffer, "change-color")){
            if(color_change > 255){
                print_vga("Color reset to 0\n", 0x0F);
                color_change = 0x0F;
                break;
            } else {
                print_vga("color changed : current [ ", 0x0F);
                print_vga(short2char_arr(color_change), 0x0F);
                print_vga(" ]\n", 0x0F);
                color_change += 0x0F;
            }
        }
        else if(!charcmp(char_buffer,"reset")){
            line = 0;
            color_change = 0x0F;
            clear_screen_vga(0x0F);
            print_vga("reset terminal\n", 0x0F);
            //pit_sleep(1);
        }
        else if(!charcmp(char_buffer,"show-stack-pointer")){
            print_vga("[INFO] : stack pointer current : ", 0x0F);
            print_vga(short2char_arr(stack_pointer_offset()), 0x0F);
            putchar_vga('\n', 0x0F);
            print_vga(short2char_arr(BOOT_DRIVE), 0x0F);
            putchar_vga('\n', 0x0F);
        }
        else if(!charcmp(char_buffer, "convert")){
            unsigned short pos = 0;
            char buffer_clone[32];
            for(short i = 0;char_buffer[i] != ';';i++){
                pos++;
            }
        }
        else {
            print_vga(char_buffer, 0x0F);
            putchar_vga('\n',0x0F);
        }
        for(short i = 0;i < sizeof(char_buffer);i++){
            char_buffer[i] = 0;
        }
        line++;
    }

    for(;;) __asm__ __volatile__("hlt");
}