#include "../../src/hw.h"
#include "../../src/io.h"
#include "../../src/gfx.h"
#include "../../src/colors.h"
#include "../../src/strops.h"
#include "../../src/rand.h"
#include "../../src/shell.h"
#include "../../src/dma.h"
#include "../../src/printf.h"
#include "../../src/text.h"

#define MAX_LINE_LENGTH (256)
#define MAX_ARGS (10)

char commandline[MAX_LINE_LENGTH] = { 0 };

int hist_index = 0;

// handles
void h_help(char **args, int argc);
void h_put_circle(char **args, int argc);
void h_colorlist(char **args, int argc);
void h_fontlist(char **args, int argc);
void h_clear(char **args, int argc);
void h_put_rect(char **args, int argc);
void h_clear_rect(char **args, int argc);
void h_show(char **args, int argc);
void h_put_square(char **args, int argc);
void h_circles(char** args, int argc);
void h_dmacopy_test(char **args, int argc);
void h_r32(char **args, int argc);
void h_w32(char **args, int argc);
void h_idmacopy(char **args, int argc);
void h_drawtext(char **args, int argc);

cmd_t commands[] = {
    { "help", "Prints this help message", h_help },
    { "colorlist", "Print the list of colors supported", h_colorlist },
    { "fontlist", "Print the list of fonts supported", h_fontlist },
    { "dmatest", "Does a RAM to RAM DMA Copy test", h_dmacopy_test },
    { "r32", "Read n words starting from addr. Args: addr, count", h_r32 },
    { "w32", "Write word to addr. Args: addr, word", h_w32 },
    { "idma", "Interactive DMA copy. Give args as physical addr: dst, src, byte_count", h_idmacopy },
    { "clear", "Clears the screen.", h_clear },
    { "rclear", "Clears part of screen specified by rect(x0, y0, x1, y1).", h_clear_rect },
    { "show", "Push draw buffer to display", h_show },
    { "circle", "Draw a circle. Args: x, y, r, color", h_put_circle },
    { "circles", "Draw n random circles. Args: n, seed[optional]", h_circles },
    { "rect", "Draw a rectangle. Args: x1, y1, x2, y2, color", h_put_rect },
    { "square", "Draw a square. Args: x, y, a, color", h_put_square },
    { "text", "Draw text. Args: x, y, text, color, font (index of font list, default is 0)", h_drawtext },
    { "null", "null", NULL}, // sentry for while looping
};

void start_cmd_processor(void);

void kernel_main() {
    uart_init();
    printf("Hello! Welcome to RPi4B MASH (Mad Again SHell).\n\n");
    gfx_init();
    start_cmd_processor();

    // spin
    while(1);
}

int process_command(char *cmdline) {
    char *args[MAX_ARGS];
    int argc = 0;
    char *ptr = cmdline;
    int in_token = 0;

    /* Tokenize */
    while(*ptr != '\0' && argc < MAX_ARGS) {
        /* Skip leading whitespace */
        while((*ptr == ' ') || (*ptr == '\t') || (*ptr == '\n') || (*ptr == '\r')) {
            ptr++;
        }

        /* Check if we are at a token */
        if(*ptr != '\0') {
            char c = *ptr;
            int q_arg = 0;
            if(c == '"' || c == '`' || c == '\'') {
                q_arg = 1;
                ptr++;
            }
            args[argc++] = ptr;
            in_token = 1;
            /* Find the end of the token */
            if(q_arg) {
                while(*ptr != c) ptr++;
            } else {
                while((*ptr != '\0') && (*ptr != ' ') && (*ptr != '\t') && (*ptr != '\n') && (*ptr != '\r')) ptr++;
            }
            /* null-terminate the token */
            if(*ptr != '\0') {
                *ptr = '\0';
                ptr++;
            }
        }
    }

    if(argc == 0) return 0;

    // Find the matching command_handler and invoke
    int found = 0;
    for(int i = 0; commands[i].handler != NULL; ++i) {
        if(strops_cmp(args[0], commands[i].name) == 0) {
            found = 1;
            commands[i].handler(args, argc);
            return (strops_cmp(commands[i].name, "quit") == 0) ? 2 : 0;
        }
    }
    if(!found) return 1;
}

void start_cmd_processor(void) {
    int status = 0;

    while(status == 0) {
        printf(PROMPT);

        // Read line
        int rl_return = shell_readline_with_echo(commandline, MAX_LINE_LENGTH, commands);
        if(rl_return < 0) {
            printf("Readine error.");
            break;
        }

        // Process command
        status = process_command(commandline);

        if(status == 1) {
            printf("Unknown command `%s`. Check help.", commandline);
            // continue loop
            status = 0;
        }

        if(status == 2) {
            printf("Exiting cmd processor....");
            return;
        }

        // print two new lines after execution
        printf("\n\n");
    }
}

void print_no_args() {
    printf("Not enough arguments. Check help.");
}

// handles' definitions

void h_help(char **args, int argc) {
    uart_print("Available commands:\n");
    for(int i = 0; commands[i].handler != NULL; ++i) {
        printf(" - %s => %s\n", commands[i].name, commands[i].help);
    }
}

void h_clear_rect(char **args, int argc) {
    if(argc < 5) {
        print_no_args();
        return;
    }

    int x0 = strops_atoi(args[1]);
    int y0 = strops_atoi(args[2]);
    int x1 = strops_atoi(args[3]);
    int y1 = strops_atoi(args[4]);

    gfx_clear_rect(x0, y0, x1, y1);
}

void h_show(char **args, int argc) {
    gfx_push_to_screen();
}

void h_put_circle(char **args, int argc) {
    if(argc < 5) {
        print_no_args();
        return;
    }

    int x = strops_atoi(args[1]);
    int y = strops_atoi(args[2]);
    int r = strops_atoi(args[3]);
    uint32_t color;
    char *ptr = args[4];
    if(ptr[0] == '0' && ptr[1] == 'x') {
        // color as hex
        color = strops_htoi(args[4]);
    } else {
        // color as string
        color = gfx_get_color_from_str(args[4]);
    }
    gfx_draw_circle_imm(x, y, r, color, 1);
}

void h_put_rect(char **args, int argc) {
    if(argc < 6) {
        print_no_args();
        return;
    }

    int x0 = strops_atoi(args[1]);
    int y0 = strops_atoi(args[2]);
    int x1 = strops_atoi(args[3]);
    int y1 = strops_atoi(args[4]);
    uint32_t color;
    char *ptr = args[5];
    if(ptr[0] == '0' && ptr[1] == 'x') {
        // color as hex
        color = strops_htoi(args[5]);
    } else {
        // color as string
        color = gfx_get_color_from_str(args[5]);
    }
    
    gfx_draw_rect_imm(x0, y0, x1, y1, color, 1);
}

void h_put_square(char **args, int argc) {
    if(argc < 5) {
        print_no_args();
        return;
    }

    int x = strops_atoi(args[1]);
    int y = strops_atoi(args[2]);
    int a = strops_atoi(args[3]);
    uint32_t color;
    char *ptr = args[4];
    if(ptr[0] == '0' && ptr[1] == 'x') {
        // color as hex
        color = strops_htoi(args[4]);
    } else {
        // color as string
        color = gfx_get_color_from_str(args[4]);
    }
    
    gfx_draw_square_imm(x, y, a, color, 1);
}

void h_circles(char** args, int argc) {
    uint32_t seed = 2;
    if(argc < 2) {
        print_no_args();
        return;
    }
    int n = strops_atoi(args[1]);
    if(argc == 3) {
        seed = strops_atoi(args[2]);
    }
    srand(seed);
    if(n) {
        while(n--) {
            int x = rand() % PD_WIDTH;
            int y = rand() % PD_HEIGHT;
            int x_min = (x < (PD_WIDTH - x)) ? x : (PD_WIDTH - x);
            int y_min = (y < (PD_HEIGHT - y)) ? y : (PD_HEIGHT - y);
            int r_max = (x_min < y_min) ? x_min : y_min;
            int r = rand() % r_max;
            // Make sure they're always less than 192
            r %= 192;
            int color = (1 + rand()) % 32; // skip black,  1 <= color <= 31
            gfx_draw_circle(x, y, r, gfx_get_color_by_idx(color), 1);
        }
    }
    gfx_push_to_screen();
}

void h_clear(char **args, int argc) {
    gfx_clearscreen();
}

void h_colorlist(char **args, int argc) {
    gfx_print_color_list();
}

void h_r32(char **args, int argc) {
    if (argc < 3) {
        print_no_args();
        return;
    }

    uint32_t addr = strops_htoi(args[1]);
    uint32_t count = strops_atoi(args[2]);

    for(int i = 0; i < count; i++) {
        uint32_t p = addr + i*4;
        uint32_t val = *(uint32_t *)(uintptr_t)(p);
        printf("0x%x : 0x%x\n", p, val);
    }
}

void h_w32(char **args, int argc) {
    if (argc < 3) {
        print_no_args();
        return;
    }

    uint32_t addr = strops_htoi(args[1]);
    uint32_t val = strops_htoi(args[2]);

    *(uint32_t *)(uintptr_t)(addr) = val;
}

void h_idmacopy(char **args, int argc) {
    if (argc < 4) {
        print_no_args();
        return;
    }

    uint32_t dst = strops_htoi(args[1]);
    uint32_t src = strops_htoi(args[2]);
    uint32_t count = strops_htoi(args[3]);

    dma_channel *ch = dma_open_channel(CT_NORMAL);
    dma_setup_mem_copy(ch, (void *)(uintptr_t)dst, (void *)(uintptr_t)src, count, 2);
    dma_start(ch);
    dma_wait(ch);
    dma_close_channel(ch);
}

uint32_t dma_srcbuf[128] __attribute((aligned(32))) = { 0 };
uint32_t dma_dstbuf[128] __attribute((aligned(32))) = { 0 };
void h_dmacopy_test(char **args, int argc) {
    for(int i = 0; i < 128; i++) {
        dma_srcbuf[i] = i;
    }
    int fail = 0;
    
    dma_channel *ch = dma_open_channel(CT_NORMAL);
    dma_setup_mem_copy(ch, dma_dstbuf, dma_srcbuf, 128*4, 2);
    dma_start(ch);
    dma_wait(ch);
    for(int i = 0; i < 128; i++) {
        if(dma_dstbuf[i] != dma_srcbuf[i]) {
            fail = 1;
            break;
        }
    }
    if(fail) {
        char temp[10];
        printf("DMA Memcpy Test - FAILED :(\n");
        printf("SRC         DST       \n");
        for(int i = 0; i < 128; i++) {
            printf("0x%x 0x%x\n", dma_srcbuf[i], dma_dstbuf[i]);
        }
    } else {
        printf("DMA Memcpy Test - PASSED !!\n");
    }
    dma_close_channel(ch);
}

void h_fontlist(char **args, int argc) {
    text_print_fontlist();
}

void h_drawtext(char **args, int argc) {
    if (argc < 5) {
        print_no_args();
        return;
    }

    int x = strops_atoi(args[1]);
    int y = strops_atoi(args[2]);
    char *s = args[3];
    uint32_t color;
    char *ptr = args[4];
    if(ptr[0] == '0' && ptr[1] == 'x') {
        // color as hex
        color = strops_htoi(args[4]);
    } else {
        // color as string
        color = gfx_get_color_from_str(args[4]);
    }
    int font = ROBOTO_MONO;
    if(argc >= 6)font = strops_atoi(args[5]);
    if(font >= FONT_COUNT)font = ROBOTO_MONO;

    int dir = 0;
    if(argc >= 7)dir = strops_atoi(args[6]);
    if(dir > 1)dir = 0;

    text_draw_str_imm(x, y, s, font, dir, color);
}
