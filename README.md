# BMRPI

This is intended to be a collection of baremetal applications in Raspberry Pi 4. Makefile is written for aarch64 compiler tools for Windows (because I am lazy). May add support for linux as well later. Endgoal is to have some graphics based games/apps which can run baremetal on the RPI4.

## Apps Done Yet

- Basic boot => `make test` will compile the test.c file with the required libs to create a simple app which will print a hello on AUX MINI UART and print hello messages to the screen.
- CMD-GFX =>
  - What started as experimenting with preliminary graphics drawing (line and circle using bresenham algorithm) has developed into a fun app!
  - It is basically a shell which supports some basic commands (read memory, write memory) and some graphics ones! (draw circle, square etc)
  - Graphics is hardcoded to be 1920x1080, 32 bit color
  - Shell supports history, tab completion, line editing with left and right arrows
  - Display is done by first writing to a 1920x1080x32 byte buffer in RAM and then DMA ing to Framebuffer. 2D DMA MEM COPY is enabled for this.
  
## Drivers and Libs added

As I am lazy, the codebase is not very well structured. Peripheral drivers and software libraries are a bit mixed up. Might clean up a while later! Anyway we have:

 - io driver => sort of a gpio + uart driver
 - led driver => extension of io, for blinking that LED on RPi4
 - gfx driver / library => a lazy amalgam of gfx driver which handles simple raster graphics (with bresenham algorithms) and contains the framebuffer init functions.
 - dma driver => a very simplified DMA driver. Only supports memcopy (1d and 2d) (huge thanks to circle library and rockytriton for their work on this. There are many nittygritties to find out to enable and use DMA correctly)
 - shell library => simple shell lib which uses the io driver for uart_putc, uart_getc and gives you shell features like readline with echo, history, tab completion etc
 - font lookup => to draw text on the screen. Included a 32 bit glyph (oh boy, the size!)

## How to make 

You need the aarch64-none-elf toolchain for Windows. Currently there are only 2 buildable target apps - test, and cmd-gfx. There is DEBUG=1 which can be added for symbols (helps in debug with QEMU + gdb). Lazy me has added make targets to launch qemu as well. Also the makefile is quite plain, without any fancy makefile logic to make it elegant. Might change it sometime :)

To make cmd-gfx, simply run `make cmd-gfx`. It should create the kernel8.img file you need to copy to the SD card (and the kernel.elf you need for QEMU and gdb)

## Immediate TODO

  - Add printf
  - Add more string operations

## Aspirations

 - A uart bootloader (like raspbootin). Removing and inserting SD card is a pain.
 - Storage driver (SD card, or be some SPI flash)
 - File system for storage (fatfs, littlefs)
 - Implement malloc
 - MultiCore Applications
 - Compile an existing RTOS (Zephyr, FreeRTOS) and run some test apps.

More apps. Something like desmos? Manim? Maybe a game like Flappy Bird? Initially input via UART, and then HID USB device?

## Big Aspirations

GPU driver and GPU rendering (haha, I can dream)

### Very Big Aspirations

Full OS (with graphics and all :O)
