CC=aarch64-none-elf-gcc.exe
AS=aarch64-none-elf-as.exe
AR=aarch64-none-elf-ar.exe
LD=aarch64-none-elf-ld.exe
OBJCOPY=aarch64-none-elf-objcopy.exe
DEBUG=0

CP=copy
RM=del /Q
RMDIR=rmdir /s /q
CC_OPTS=-nostdlib -nostartfiles -ffreestanding -march=armv8-a+fp+simd
LD_OPTS=
AR_OPTS=rcs

DRIVE=S

ifeq ($(DEBUG), 1)
	CC_OPTS+= -g -O0
	LD_OPTS+= -g -O0
endif

all: test

libs:
	@$(CC) $(CC_OPTS) -c src/dma.c -o obj/dma.o
	@$(CC) $(CC_OPTS) -c src/io.c -o obj/io.o
	@$(CC) $(CC_OPTS) -c src/led.c -o obj/led.o
	@$(CC) $(CC_OPTS) -c src/gfx.c -o obj/gfx.o
	@$(CC) $(CC_OPTS) -c src/term.c -o obj/term.o
	@$(CC) $(CC_OPTS) -c src/maths.c -o obj/maths.o
	@$(CC) $(CC_OPTS) -c src/strops.c -o obj/strops.o
	@$(CC) $(CC_OPTS) -c src/rand.c -o obj/rand.o
	@$(CC) $(CC_OPTS) -c src/shell.c -o obj/shell.o
	@$(AR) $(AR_OPTS) lib/libhw.a obj/dma.o obj/shell.o obj/strops.o obj/maths.o obj/gfx.o obj/led.o obj/io.o obj/term.o obj/rand.o

test: libs
	@$(AS) -c startup/boot.S -o obj/boot.o
	@$(CC) $(CC_OPTS) -c test.c -o obj/test.o
	@$(LD) $(LD_OPTS) -T linker.ld -o test.elf obj/test.o obj/boot.o lib/libhw.a -Map test.map
	@$(OBJCOPY) test.elf -O binary test.img
	$(CP) test.img kernel8.img

cmd-gfx: libs
	@$(AS) -c games/cmd-gfx/boot.S -o obj/boot.o
	@$(CC) $(CC_OPTS) -c games/cmd-gfx/kernel.c -o obj/kernel.o
	@$(LD) $(LD_OPTS) -T games/cmd-gfx/linker.ld -o kernel.elf obj/kernel.o obj/boot.o lib/libhw.a -Map test.map
	@$(OBJCOPY) kernel.elf -O binary kernel8.img

run:
	qemu-system-aarch64 -M raspi4b -kernel kernel8.img -serial null -serial stdio -S -gdb tcp::1234

clean:
	@$(RM) obj\* lib\* *.img *.elf

.PHONY: clean
