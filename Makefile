CC=aarch64-none-elf-gcc.exe
AS=aarch64-none-elf-as.exe
AR=aarch64-none-elf-ar.exe
LD=aarch64-none-elf-ld.exe
OBJCOPY=aarch64-none-elf-objcopy.exe
DEBUG=0

CP=copy /y
RM=del /Q
RMDIR=rmdir /s /q
CC_OPTS=-nostdlib -nostartfiles -ffreestanding -march=armv8-a+fp+simd
LD_OPTS=
AR_OPTS=rcs

RES=480p

ifeq ($(RES), 480p)
	CC_OPTS+= -DRES_480P
endif
ifeq ($(RES), 720p)
	CC_OPTS+= -DRES_720P
endif
ifeq ($(RES), 1080p)
	CC_OPTS+= -DRES_1080P
endif

ifeq ($(DEBUG), 1)
	CC_OPTS+= -g -O0 -DDEBUG
	LD_OPTS+= -g -O0
endif

# Get all C files in the src/ directory
SRC_FILES := $(wildcard src/*.c)
# Convert C file paths to object file paths
OBJ_FILES := $(patsubst src/%.c, obj/%.o, $(SRC_FILES))

libs: $(OBJ_FILES)
	@$(AR) $(AR_OPTS) lib/libhw.a $(OBJ_FILES)

# A pattern rule to compile any C file from src/ to obj/
obj/%.o: src/%.c
	@$(CC) $(CC_OPTS) -c $< -o $@

cmd-gfx: libs
	@$(AS) -c games/cmd-gfx/boot.S -o obj/boot.o
	@$(CC) $(CC_OPTS) -c games/cmd-gfx/kernel.c -o obj/kernel.o
	@$(LD) $(LD_OPTS) -T games/cmd-gfx/linker.ld -o kernel.elf obj/kernel.o obj/boot.o lib/libhw.a -Map kernel.map
	@$(OBJCOPY) kernel.elf -O binary kernel8.img

move-sprite: libs
	@$(AS) -c games/move-sprite/boot.S -o obj/boot.o
	@$(CC) $(CC_OPTS) -c games/move-sprite/kernel.c -o obj/kernel.o
	@$(CC) $(CC_OPTS) -c games/move-sprite/sprite/sprite_data.c -o obj/sprite_data.o
	@$(LD) $(LD_OPTS) -T games/move-sprite/linker.ld -o kernel.elf obj/sprite_data.o obj/kernel.o obj/boot.o lib/libhw.a -Map kernel.map
	@$(OBJCOPY) kernel.elf -O binary kernel8.img

bounce: libs
	@$(AS) -c games/bounce/boot.S -o obj/boot.o
	@$(CC) $(CC_OPTS) -c games/bounce/kernel.c -o obj/kernel.o
	@$(CC) $(CC_OPTS) -c games/bounce/sprite/sprite_data.c -o obj/sprite_data.o
	@$(LD) $(LD_OPTS) -T games/bounce/linker.ld -o kernel.elf obj/sprite_data.o obj/kernel.o obj/boot.o lib/libhw.a -Map kernel.map
	@$(OBJCOPY) kernel.elf -O binary kernel8.img

matrix-mazha: libs
	@$(AS) -c games/matrix-mazha/boot.S -o obj/boot.o
	@$(CC) $(CC_OPTS) -c games/matrix-mazha/kernel.c -o obj/kernel.o
	@$(LD) $(LD_OPTS) -T games/matrix-mazha/linker.ld -o kernel.elf obj/kernel.o obj/boot.o lib/libhw.a -Map kernel.map
	@$(OBJCOPY) kernel.elf -O binary kernel8.img

space-invaders: libs
	@$(AS) -c games/space-invaders/boot.S -o obj/boot.o
	@$(CC) $(CC_OPTS) -c games/space-invaders/kernel.c -o obj/kernel.o
	@$(LD) $(LD_OPTS) -T games/space-invaders/linker.ld -o kernel.elf obj/kernel.o obj/boot.o lib/libhw.a -Map kernel.map
	@$(OBJCOPY) kernel.elf -O binary kernel8.img

flash:
	@$(CP) kernel8.img G:\embedded-dev\rpishare\kernel8.img

run:
	qemu-system-aarch64 -M raspi4b -kernel kernel8.img -serial null -serial stdio

debug:
	qemu-system-aarch64 -M raspi4b -kernel kernel8.img -serial null -serial stdio -S -gdb tcp::1234

clean:
	@$(RM) obj\*.o lib\*.a *.img *.elf

.PHONY: clean