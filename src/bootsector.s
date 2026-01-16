[BITS 16]
[ORG 0x7C00]

BOOT_DRIVE_BUF: db 0
SECTOR2_OFFSET equ 0x9000
SECTOR2_SEGMENT equ 0x0000

_start:

    cli
    
    mov byte [BOOT_DRIVE_BUF],dl
    
    xor ax,ax
    mov es,ax
    mov ds,ax
    mov ss,ax

    mov sp,0x7C00
    sti
    
    mov ax,0x0003
    int 0x10
    
    mov ah,0x02
    mov al,SECTOR_COUNT
    xor ch,ch
    mov cl,2
    xor dh,dh
    
    mov dl,byte [BOOT_DRIVE_BUF]
    mov bx,SECTOR2_SEGMENT
    mov es,bx
    mov bx,SECTOR2_OFFSET
    int 0x13
    
    jc .L02

.L01:

    lea si,[DS : L1]
    call print
    
    jmp SECTOR2_SEGMENT : SECTOR2_OFFSET

    jmp .L03

.L02:

    lea si,[DS : L2]
    call print

    jmp .L03

.L03:

    hlt 

    jmp .L03

print:

    pusha
    mov ah,0x0E

.putchar:

    lodsb
    or al,al
    jz .done
    int 0x10
    jmp .putchar
    
.done:

    popa
    ret

L1: db "[INFO] : Load Sector 2",0x00
L2: db "[ERROR] : Failed to load sector 2",0x00
SECTOR_COUNT equ 20

times 510 - ($ - $$) db 0
dw 0xAA55