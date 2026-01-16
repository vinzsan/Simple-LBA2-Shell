i386-elf-gcc -m16 sector-lba2.c -o sector-lba2.o -c -static -fno-stack-protector -fno-pie -fno-pic -fomit-frame-pointer -Wall \
-ffreestanding -nostdlib -nostartfiles

i386-elf-ld -T link.ld sector-lba2.o -o sector-lba2.elf -static 
i386-elf-objcopy -O binary sector-lba2.elf sector-lba2.bin

nasm -f bin bootsector.s -o bootsector.bin

dd if=/dev/zero of=os.img bs=512 count=2880
dd if=bootsector.bin of=os.img bs=512 count=1 conv=notrunc
dd if=sector-lba2.bin of=os.img bs=512 seek=1 conv=notrunc