CC=aarch64-none-elf-gcc.exe
AS=aarch64-none-elf-as.exe
AR=aarch64-none-elf-ar.exe
LD=aarch64-none-elf-ld.exe
OBJCOPY=aarch64-none-elf-objcopy.exe

CP=copy
RM=del /Q
RMDIR=rmdir /s /q
CC_OPTS=-nostdlib -nostartfiles -ffreestanding -march=armv8-a+fp+simd
AR_OPTS=rcs

all: test

libs:
	@$(CC) $(CC_OPTS) -c src/uart.c -o obj/uart.o
	@$(CC) $(CC_OPTS) -c src/led.c -o obj/led.o
	@$(CC) $(CC_OPTS) -c src/gfx.c -o obj/gfx.o
	@$(CC) $(CC_OPTS) -c src/maths.c -o obj/maths.o
	@$(AR) $(AR_OPTS) lib/libhw.a obj/maths.o obj/gfx.o obj/led.o obj/uart.o

test: libs
	@$(AS) -c startup/boot.S -o obj/boot.o
	@$(CC) $(CC_OPTS) -c test.c -o obj/test.o
	@$(LD) -T linker.ld -o test.elf obj/test.o obj/boot.o lib/libhw.a
	@$(OBJCOPY) test.elf -O binary test.img

flash:
	@$(CP) *.img boot\kernel8.img
	@$(CP) boot\*.* S:\

clean:
	@$(RM) obj\* lib\* *.img *.elf

.PHONY: clean
