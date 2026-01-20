ASSEMBLER = nasm
COLLECTION_i386 = i386-elf
OUT-CC = sector-lba2

IMAGE = os.img

all: os.img

build:

	$(COLLECTION_i386)-gcc -m16 src/sector-lba2.c -o sector-lba2.o -c -static -fno-stack-protector -fno-pie -fno-pic -fomit-frame-pointer -Wall \
	-ffreestanding -nostdlib -nostartfiles -O3 -g

	$(COLLECTION_i386)-ld -T link.ld sector-lba2.o -o sector-lba2.elf -static 
	$(COLLECTION_i386)-objcopy -O binary sector-lba2.elf sector-lba2.bin

	$(ASSEMBLER) -f bin src/bootsector.s -o bootsector.bin

	dd if=/dev/zero of=$(IMAGE) bs=512 count=2880
	dd if=bootsector.bin of=os.img bs=512 count=1 conv=notrunc
	dd if=$(OUT-CC).bin of=os.img bs=512 seek=1 conv=notrunc

run	:

	qemu-system-i386 -fda $(IMAGE)

clean : 
	rm -rf os.img bootsector.bin $(OUT-CC).bin $(OUT-CC).o $(OUT-CC).elf \
	