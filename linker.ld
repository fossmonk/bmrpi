OUTPUT_FORMAT("elf64-littleaarch64")
OUTPUT_ARCH(aarch64)

ENTRY(_start)

SECTIONS
{
    /* The kernel must be loaded at physical address 0x80000 */
    . = 0x80000;

    /* The .text section contains all executable code */
    .text :
    {
        /* Ensure the boot-up code is at the start of the section */
        KEEP(*(.text.boot))
        *(.text*)
    }

    /* The .rodata section contains read-only data */
    .rodata :
    {
        *(.rodata*)
    }

    /* The .data section contains initialized data */
    .data :
    {
        *(.data*)
    }

    /* The vector table must be aligned to a 4KB boundary */
    . = ALIGN(0x1000);
    .vector_table :
    {
        *(.vector_table)
    }

    /* The .bss section contains uninitialized data and must be cleared at boot */
    __bss_start = .;
    .bss (NOLOAD) :
    {
        *(.bss*)
        *(COMMON)
    }
    __bss_end = .;

    /* The stack is placed at the end of the memory space */
    . = ALIGN(0x1000);
    _stack_start = .;
    .stack (NOLOAD) :
    {
        . = . + 0x4000; /* 16KB stack size */
    }
    _stack_end = .;
}