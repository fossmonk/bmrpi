# BMRPI

This is intended to be a collection of baremetal applications in Raspberry Pi 4. Makefile is written for aarch64 compiler tools. Endgoal is to have some graphics based games/apps which can run baremetal on the RPI4.

## Apps Done Yet

- CMD-GFX =>
  - What started as experimenting with preliminary graphics drawing (line and circle using bresenham algorithm) has developed into a fun app!
  - It is basically a shell which supports some basic commands (read memory, write memory) and some graphics ones! (draw circle, square etc)
  - ~Graphics is hardcoded to be 1920x1080, 32 bit color~ Resolution can be selected, but color depth is 32b.
  - Shell supports history, tab completion, line editing with left and right arrows

- BOUNCE =>
  - A physics experiment, modelling freefalling bodies which has an imperfect elasticity
  - Screen will show 1 Rubiks cube falling from top, bouncing off ground and finishing bounces. Once done, it is dropped again
  - If you keep UART console open and press a, a new cube will be added in to the mix :)

- MOVE-SPRITE =>
  - Rudimentary efforts at a platformer style game, WIP.
  
## Drivers and Libs added

As I am lazy, the codebase is not very well structured. Peripheral drivers and software libraries are a bit mixed up. Might clean up a while later! Anyway we have:

 - io driver => sort of a gpio + uart driver
 - led driver => extension of io, for blinking that LED on RPi4
 - gfx driver / library => a lazy amalgam of gfx driver which handles simple raster graphics (with bresenham algorithms) and contains the framebuffer init functions.
 - dma driver => a very simplified DMA driver. Only supports memcopy (1d and 2d) (huge thanks to circle library and rockytriton for their work on this. There are many nittygritties to find out to enable and use DMA correctly)
 - shell library => simple shell lib which uses the io driver for uart_putc, uart_getc and gives you shell features like readline with echo, history, tab completion etc
 - font lookup => to draw text on the screen. Included a 32 bit glyph (oh boy, the size!)

## How to make 

You need the aarch64-none-elf toolchain.

Currently there are 3 buildable target apps - bounce, move-sprite, cmd-gfx. There is DEBUG=1 which can be added for symbols (helps in debug with QEMU + gdb). Lazy me has added make targets to launch qemu as well. Also the makefile is quite plain, without any fancy makefile logic to make it elegant. Might change it sometime :)

To make any app, simply run `make <appname>`. It should create the kernel8.img file you need to copy to the SD card (and the kernel.elf you need for QEMU and gdb). I use TFTP boot, so `make flash` copies it to my TFTP share location. Might make this generic.

## How to add new app/game

 - Create a folder with the appname under `games/`
 - Copy the `boot.S` and `linker.ld` files from one of the app folders. 
 - Add a file called kernel.c (or anyname, just update the Makefile accordingly)
 - Add a new make target with the appname and update the makerules for the target

## TODO (short features)

  [x] Add printf
  [x] Add more string operations
  [ ] Cleanup
  [ ] Software Serial Port

## Aspirations
 - More GAMES
 - Storage driver (SD card, or be some SPI flash)
 - File system for storage (fatfs, littlefs)
 - Implement malloc
 - MultiCore Applications
 - Compile an existing RTOS (Zephyr, FreeRTOS) and run some test apps.

More apps. Something like desmos? Manim? Maybe a game like Flappy Bird?

## Big Aspirations

 - USB Host Driver
 - Get all PL011 UARTs working

### Very Big Aspirations

Full OS (with graphics and all :O)
