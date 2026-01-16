# x86_16 simple OS

Proyek pengembangan sistem operasi minimal 16-bit x86 yang ditulis dalam C dan Assembly.

## Gambaran Umum

Ini adalah sistem operasi bootable yang masih dalam tahap pengembangan, berjalan di real mode (16-bit) dengan rencana transisi ke protected mode (32-bit). Saat ini fokus pada implementasi fungsi inti sistem dan interfacing hardware.

![brief]("./preview/image.png)

## Status Saat Ini

### Fitur yang Sudah Diimplementasikan
- [x] Bootloader (boot sector 512-byte)
- [x] Multi-sector loading (pengalamatan LBA)
- [x] Driver VGA text mode (akses langsung ke buffer 0xB8000)
- [x] Keyboard input handling (translasi scancode)
- [x] Driver PIT (Programmable Interval Timer)
- [x] Operasi I/O port (instruksi in/out)
- [x] Interface shell dasar dengan command parsing
- [x] Manajemen cursor (VGA hardware cursor)
- [x] Screen scrolling
- [x] Rendering teks berwarna

### Sedang Dikerjakan
- [ ] Inisialisasi GDT (Global Descriptor Table)
- [ ] Aktivasi A20 line (untuk akses memori >1MB)
- [ ] Switching ke protected mode (transisi 16-bit ke 32-bit)
- [ ] Setup IDT (Interrupt Descriptor Table)
- [ ] IRQ handling di protected mode

### Fitur yang Direncanakan
- [ ] Memory management (paging/segmentation)
- [ ] Dukungan filesystem (FAT12/FAT16)
- [ ] Driver disk I/O (ATA/IDE)
- [ ] Hapus dependensi BIOS INT
- [ ] Dukungan user mode
- [ ] System calls dasar
- [ ] Process/task switching
- [ ] Higher half kernel

## Kebutuhan Sistem

### Build Dependencies
- `nasm` - Netwide Assembler untuk bootloader
- `i386-elf-gcc` - Cross-compiler untuk target x86
- `i386-elf-ld` - Linker untuk objek ELF
- `i386-elf-objcopy` - Konverter object file
- `dd` - Data duplicator (standar di sistem Unix)
- `make` - Build automation

### Runtime Requirements
- CPU kompatibel x86 (hardware asli atau emulator)
- Floppy disk drive 1.44MB (atau emulasi)
- Display kompatibel VGA
- Keyboard PS/2

### Environment Testing yang Disarankan
- QEMU (`qemu-system-i386`)
- Bochs (untuk debugging)
- VirtualBox/VMware (untuk testing di hardware)

## Compile dan Build

```bash
make build
```

Proses ini akan:
1. Kompilasi file C source ke object code 16-bit
2. Assembly bootloader dan file assembly tambahan
3. Link semua object menjadi satu ELF executable
4. Konversi ELF ke raw binary
5. Buat floppy disk image 1.44MB
6. Tulis bootloader ke sektor pertama
7. Tulis kernel ke sektor berikutnya

## Menjalankan OS

### QEMU
```bash
make run
```

### Opsi QEMU custom
```bash
qemu-system-i386 -fda os.img -boot a
```

### Bochs
```bash
bochs -f bochsrc -q
```

### Hardware Asli
Tulis image ke USB/floppy:
```bash
sudo dd if=os.img of=/dev/sdX bs=512
```

## Project Structure

```
.
├── src/
│   ├── bootsector.s        # Boot sector (stage 1)
│   ├── sector-lba2.c       # Main kernel entry
│   ├── igdt_loader.c       # GDT loader (WIP)
│   └── gdt_loader.asm      # Assembly GDT routines (WIP)
├── include/
│   ├── pit_channel.h       # PIT timer definitions
│   ├── ioport.h            # Port I/O functions
│   ├── vga_dma.h           # VGA buffer operations
│   └── igdt.h              # GDT structures (WIP)
├── link.ld                 # Linker script
├── Makefile                # Build configuration
└── README.md               # This file
```

## Command Shell

Perintah yang sudah tersedia:

- `print` - Tampilkan pesan "Hello world"
- `clear` - Bersihkan layar dan reset line counter
- `shutdown` - Matikan VM (khusus QEMU/Bochs)
- `debug-mode` - Tampilkan status debug
- `help` - Tampilkan daftar perintah
- `change-color` - Ganti skema warna
- `reset` - Reset state terminal
- `load-kernel` - Inisialisasi protected mode (WIP)
- Dokumentasi masih terbatas karena hanya sebatas uji coba

## Detail Teknis

### Memory Layout
```
0x0000 - 0x03FF   : Interrupt Vector Table (IVT)
0x0400 - 0x04FF   : BIOS Data Area (BDA)
0x0500 - 0x7BFF   : Free conventional memory
0x7C00 - 0x7DFF   : Boot sector (stage 1)
0x7E00 - 0x????   : Kode kernel (stage 2)
0x9000 - 0x9000   : Stack (tumbuh ke bawah)
0xA0000 - 0xBFFFF : Video memory
0xB8000 - 0xBFFFF : VGA text mode buffer
```

### Struktur GDT (Direncanakan)
```
Entry 0: Null descriptor
Entry 1: Code segment (base=0, limit=4GB, ring 0)
Entry 2: Data segment (base=0, limit=4GB, ring 0)
```

### Limitasi Saat Ini
- Masih beroperasi di 16-bit real mode
- Terbatas pada pengalamatan memori 1MB (A20 belum diaktifkan)
- Masih menggunakan BIOS interrupts (akan dihapus di protected mode)
- Belum ada interrupt handling selain BIOS
- Single-tasking saja
- Belum ada dukungan filesystem
- Hanya akses hardware langsung

## Known Issues

- Transisi protected mode belum stabil
- Aktivasi A20 line perlu testing di hardware asli
- Beberapa kode shutdown khusus QEMU (port 0x604)
- Tidak ada error handling untuk disk reads
- Inisialisasi keyboard controller mungkin gagal di beberapa hardware

## Roadmap Pengembangan

### Fase 1: Fondasi Real Mode (SAAT INI)
- [x] Boot dari disk
- [x] Load sektor tambahan
- [x] Driver I/O dasar
- [x] Shell sederhana

### Fase 2: Transisi Protected Mode
- [ ] Setup dan loading GDT
- [ ] Aktivasi A20 gate
- [ ] Implementasi mode switch
- [ ] Entry point kernel 32-bit

### Fase 3: Manajemen Interrupt
- [ ] Inisialisasi IDT
- [ ] Konfigurasi PIC (8259)
- [ ] Framework IRQ handler
- [ ] Timer interrupts
- [ ] Keyboard interrupts

### Fase 4: Manajemen Memori
- [ ] Deteksi physical memory
- [ ] Page frame allocator
- [ ] Setup paging
- [ ] Virtual memory manager

### Fase 5: Fitur Lanjutan
- [ ] Driver disk (ATA)
- [ ] Filesystem (FAT12)
- [ ] User mode
- [ ] System calls
- [ ] Multitasking

## Kontribusi

Ini adalah proyek pembelajaran pribadi, namun saran dan improvement tetap diterima.

## License

Proyek ini disediakan apa adanya untuk tujuan edukasi.

## Referensi

- Intel 80386 Programmer's Reference Manual
- OSDev Wiki: https://wiki.osdev.org
- Ralf Brown's Interrupt List
- Writing a Simple Operating System from Scratch (Nick Blundell)

## Debugging

### Aktifkan QEMU monitor
```bash
qemu-system-i386 -fda os.img -monitor stdio
```

### Bochs debugger
```bash
bochs -f bochsrc -q
```

Perintah debugger umum:
- `c` - continue
- `s` - step
- `b 0x7c00` - set breakpoint
- `x /10wx 0x7c00` - examine memory
- `info gdt` - tampilkan GDT
- `info idt` - tampilkan IDT

## Penjelasan Build Flags

### GCC Flags
- `-m16` - Generate kode 16-bit
- `-ffreestanding` - Tanpa standard library
- `-nostdlib` - Jangan link standard library
- `-fno-pie` - Tidak ada position independent code
- `-fno-stack-protector` - Disable stack protection
- `-static` - Static linking saja

### NASM Flags
- `-f bin` - Output flat binary
- `-f elf32` - Output objek ELF 32-bit

## Acknowledgments

Terinspirasi dari berbagai tutorial OS development dan komunitas OSDev.

---

Terakhir diupdate: Januari 2026
Status: Pengembangan Aktif (Pre-Alpha)