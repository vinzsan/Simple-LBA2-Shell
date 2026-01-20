[BITS 16]
[ORG 0x7C00]

BOOT_DRIVE_BUF: db 0        ; BOOT DRIVE static block buat simpen state si DL register
SECTOR2_OFFSET equ 0x9000   ; Dari awal boot atau masuk ke offset mem 0x7C00 DL udah simpen state sector sectornya
SECTOR2_SEGMENT equ 0x0000  ; Trus 0x9000 ini buat offset jmp ke sector2/stage2,segment bisa di null in

_start:

    cli
    
    mov byte [BOOT_DRIVE_BUF],dl    ; Disini statenya di simpan dari DL
    
    xor ax,ax   ; Bersihin AX cuman buat place holder
    mov es,ax   ; BERSIHIN SEMUA REGISTER SEGMENT,(WAJIB)
    mov ds,ax   ; ES,DS,SS,FS,GS dll
    mov ss,ax

    mov sp,0x7C00   ; set stack dari 0x7C00
    sti             ; Enable INT
    
    mov ax,0x0003   ; Clear layar
    int 0x10
    
    mov ah,0x02
    mov al,SECTOR_COUNT ; Jumlah Sector yang mau di muat
    xor ch,ch           ; INI critical section sih,coba aku demons langsung disini aja
    mov cl,2            ; Jadi nanti ada beberapa kode yang nggak di load sama memory,namun kodenya ada
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
SECTOR_COUNT equ 30

times 510 - ($ - $$) db 0
dw 0xAA55