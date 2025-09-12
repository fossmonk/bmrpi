#include "../../src/hw.h"
#include "../../src/io.h"
#include "../../src/gfx.h"
#include "../../src/colors.h"
#include "../../src/strops.h"
#include "../../src/rand.h"
#include "../../src/shell.h"

#define MAX_LINE_LENGTH (256)
#define MAX_ARGS (10)
#define NUM_CMDS (7)
#define PROMPT "rpi4b » "

char commandline[MAX_LINE_LENGTH] = { 0 };

int hist_index = 0;

typedef struct {
    char* name;
    char* help;
    void (*handler)(char**, int);
} cmd_t;

// handles
void h_help(char **args, int argc);
void h_put_circle(char **args, int argc);
void h_colorlist(char **args, int argc);
void h_clear(char **args, int argc);
void h_put_rect(char **args, int argc);
void h_put_square(char **args, int argc);
void h_circles(char** args, int argc);

cmd_t commands[NUM_CMDS] = {
    { "help", "Prints this help message", h_help },
    { "colorlist", "Print the list of colors supported", h_colorlist },
    { "clear", "Clears the screen.", h_clear },
    { "circle", "Draw a circle. Args: x, y, r, color", h_put_circle },
    { "circles", "Draw n random circles. Args: n, seed[optional]", h_circles },
    { "rect", "Draw a rectangle. Args: x1, y1, x2, y2, color", h_put_rect },
    { "square", "Draw a square. Args: x, y, a, color", h_put_square },
};

void start_cmd_processor(void);

void kernel_main() {
    uart_init();
    uart_print("HELLO FROM RASPI 4B\r\n\r\n");
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
            args[argc++] = ptr;
            in_token = 1;
            /* Find the end of the token */
            while((*ptr != '\0') && (*ptr != ' ') && (*ptr != '\t') && (*ptr != '\n') && (*ptr != '\r')) {
                ptr++;
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
    for(int i = 0; i < NUM_CMDS; ++i) {
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
        uart_print(PROMPT);

        // Read line
        int rl_return = shell_readline_with_echo(commandline, MAX_LINE_LENGTH);
        if(rl_return < 0) {
            uart_print("Readine error.");
            break;
        }

        // Process command
        status = process_command(commandline);

        if(status == 1) {
            uart_print("Unknown command `");
            uart_print(commandline);
            uart_print("`. Check help.");
            // continue loop
            status = 0;
        }

        if(status == 2) {
            uart_print("Exiting cmd processor....");
            return;
        }

        // print two new lines after execution
        uart_print("\n\n");
    }
}

// handles' definitions

void h_help(char **args, int argc) {
    uart_print("Available commands:\n");
    for(int i = 0; i < NUM_CMDS; ++i) {
        uart_print("  - ");
        uart_print(commands[i].name);
        uart_print(":");
        uart_print(commands[i].help);
        uart_putc('\n');
    }
}

void h_put_circle(char **args, int argc) {
    if(argc < 5) {
        uart_print("Not enough arguments. Check help.");
        return;
    }

    int x = strops_atoi(args[1]);
    int y = strops_atoi(args[2]);
    int r = strops_atoi(args[3]);
    uint32_t color = gfx_get_color_from_str(args[4]);
    
    gfx_draw_circle(x, y, r, color, 1);
}

void h_put_rect(char **args, int argc) {
    if(argc < 6) {
        uart_print("Not enough arguments. Check help.");
        return;
    }

    int x0 = strops_atoi(args[1]);
    int y0 = strops_atoi(args[2]);
    int x1 = strops_atoi(args[3]);
    int y1 = strops_atoi(args[4]);
    uint32_t color = gfx_get_color_from_str(args[5]);
    
    gfx_draw_rect(x0, y0, x1, y1, color, 1);
}

void h_put_square(char **args, int argc) {
    if(argc < 5) {
        uart_print("Not enough arguments. Check help.");
        return;
    }

    int x = strops_atoi(args[1]);
    int y = strops_atoi(args[2]);
    int a = strops_atoi(args[3]);
    uint32_t color = gfx_get_color_from_str(args[4]);
    
    gfx_draw_square(x, y, a, color, 1);
}

void h_circles(char** args, int argc) {
    uint32_t seed = 2;
    if(argc < 2) {
        uart_print("Not enough arguments. Check help.");
        return;
    }
    int n = strops_atoi(args[1]);
    if(argc == 3) {
        seed = strops_atoi(args[2]);
    }
    srand(seed);
    if(n) {
        while(n--) {
            int x = rand() % DISPLAY_WIDTH;
            int y = rand() % DISPLAY_HEIGHT;
            int x_min = (x < (DISPLAY_WIDTH - x)) ? x : (DISPLAY_WIDTH - x);
            int y_min = (x < (DISPLAY_HEIGHT - y)) ? y : (DISPLAY_HEIGHT - y);
            int r_min = (x_min < y_min) ? x_min : y_min;
            int r = 10 + rand() % r_min;
            // 10 < r < 100
            r %= 100;
            int color = rand() % 32;
            gfx_draw_circle(x, y, r, gfx_get_color_by_idx(color), 1);
        }
    }
}

void h_clear(char **args, int argc) {
    gfx_clearscreen();
}

void h_colorlist(char **args, int argc) {
    gfx_print_color_list();
}
