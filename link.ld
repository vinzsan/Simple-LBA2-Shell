ENTRY(_start)

SECTIONS {
    . = 0x9000;
    
    .text : {
        *(.boot_entry)
        *(.text)
    }
    .rodata : {*(.rodata)}
    .data : {*(.data)}
    .bss : {*(.bss)}
}