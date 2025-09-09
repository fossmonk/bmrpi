/* boot.S - Bare-metal Raspberry Pi 4 startup (aarch64) */

.section .text.boot
.global _start

.set STACK_SIZE, 0x10000 // 64 KB stack
.set PAGE_SIZE,  0x1000  // 4 KB page

_start:
    // Set up the stack pointer to a safe, high memory address.
    // In a bare-metal environment, the stack is the first thing you must configure.
    ldr x0, =_stack_start
    mov sp, x0

    // Enable the MMU and caching.
    // Set SCTLR_EL1.
    ldr x0, =(1 << 2) | (1 << 12) | (1 << 19) // Example: D-cache, I-cache, and MMU
    msr SCTLR_EL1, x0

    // Initialize the vector base address register (VBAR_EL1).
    // This is crucial for handling exceptions and interrupts.
    ldr x0, =vector_table
    msr VBAR_EL1, x0

    // Clear the .bss section.
    // The .bss section contains uninitialized global variables that must be zeroed out
    // before the C code can use them.
    ldr x1, =__bss_start
    ldr x2, =__bss_end

bss_clear_loop:
    cmp x1, x2
    b.ge bss_clear_done
    str xzr, [x1], #8 // Store a zero value (xzr) and increment the pointer by 8 bytes
    b bss_clear_loop

bss_clear_done:
    // Call the C entry point.
    bl kernel_main

halt:
    // A simple endless loop to halt execution.
    // `wfe` (Wait For Event) is a low-power instruction that stops the core until an event occurs.
    wfe
    b halt

.align 12 // Align to 4 KB boundary
.global vector_table
vector_table:
    // Exception vectors for EL1
    // These vectors handle different types of exceptions, such as system calls,
    // data/instruction aborts, and interrupts. Each vector should point to a handler.
    .rept 4 // Synchronous exceptions
        b exception_handler
    .endr
    .rept 4 // IRQs
        b irq_handler
    .endr
    .rept 4 // FIQs
        b fiq_handler
    .endr
    .rept 4 // SError
        b serror_handler
    .endr

// Placeholder handlers
exception_handler:
    // Example: Print an error message and halt.
    b halt

irq_handler:
    // Example: Acknowledge interrupt and return.
    b irq_handler

fiq_handler:
    b fiq_handler

serror_handler:
    b serror_handler
