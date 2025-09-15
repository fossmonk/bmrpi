#include <stdint.h>
#include "io.h"
#include "shell.h"
#include "strops.h"

#define MAX_LINE_LENGTH (256)
#define HISTORY_SIZE (10)
#define MAX_CMDS (256)
#define MATCH_MAP_SIZE (MAX_CMDS / 8)

static char history[HISTORY_SIZE][MAX_LINE_LENGTH] = { 0 };

/* Assume 256 cmds */
static uint8_t match_map[MATCH_MAP_SIZE] = { 0 };

static int size = 0;
static int top = -1;

void add_to_history(char *s) {
    if(s != NULL && s[0] != '\0') {
        top = (top + 1) % HISTORY_SIZE;
        int count = 0;
        while(*s && count < MAX_LINE_LENGTH - 1) {
            history[top][count] = *s++;
            count++;
        }
        history[top][count] = '\0';
        if(size < HISTORY_SIZE) size++;
    }
}

char* retrieve_from_history(int retrieve_idx) {
    if (retrieve_idx >= size || retrieve_idx < 0) {
        return NULL;
    }
    int i = top - retrieve_idx;
    if (i < 0) {
        i += HISTORY_SIZE;
    }
    return history[i];
}

void print_history() {
    for(int i = 0; i < size; i++) {
        uart_print(retrieve_from_history(i));
        uart_putc('\n');
    }
}

int get_matches(char *buf, cmd_t *cmds, int* match_pos, int* match_idx) {
    int matches = 0;
    // zero init match map
    for(int i = 0; i < MATCH_MAP_SIZE; i++) {
        match_map[i] = 0;
    }

    for(int i = 0; cmds[i].handler != NULL; i++) {
        char *p = buf;
        char *cmd = cmds[i].name;
        int match_count = 0;
        int j = 0;
        int matched = 1;
        while(*p) {
            if((*p++) != *cmd++) {
                matched = 0;
            } else {
                match_count++;
            }
        }
        if(matched) {
            *match_pos = match_count;
            *match_idx = i;
            match_map[(i/8)] |= (1 << (i%8));
            matches++;
        }
    }

    return matches;
}

int shell_readline_with_echo(char *buf, int max_len, cmd_t *cmd_list) {
    int i = 0; // Current buffer length
    int cursor_pos = 0; // Current cursor position
    int c;
    int history_idx = -1;
    int b_add_to_hist = 1;

    // clear buffer
    for(int i = 0; i < max_len; i++) buf[i] = '\0';

    // A loop to get characters and echo them
    while (1) {
        c = uart_getc();

        if (c != -1) {
            // Handle escape sequences
            if (c == 0x1B) {
                char next_char = uart_getc();
                if (next_char == 0x5B) {
                    char arrow_char = uart_getc();

                    // UP ARROW
                    if (arrow_char == 'A') {
                        if (history_idx < size - 1) {
                            history_idx++;
                            char *hist_entry = retrieve_from_history(history_idx);
                            if (hist_entry) {
                                // Clear current line
                                for (int k = 0; k < i; k++) uart_print("\b \b");
                                // Copy and display history entry
                                i = 0;
                                while(hist_entry[i] != '\0' && i < max_len - 1) {
                                    buf[i] = hist_entry[i];
                                    uart_putc(buf[i]);
                                    i++;
                                }
                                buf[i] = '\0';
                                cursor_pos = i;
                            }
                        }
                        continue;
                    }
                    // DOWN ARROW
                    else if (arrow_char == 'B') {
                        if (history_idx > 0) {
                            history_idx--;
                            char *hist_entry = retrieve_from_history(history_idx);
                            // Clear and redraw line
                            for (int k = 0; k < i; k++) uart_print("\b \b");
                            i = 0;
                            if (hist_entry) {
                                while(hist_entry[i] != '\0' && i < max_len - 1) {
                                    buf[i] = hist_entry[i];
                                    uart_putc(buf[i]);
                                    i++;
                                }
                            }
                            buf[i] = '\0';
                            cursor_pos = i;
                        } else if (history_idx == 0) { // Return to empty line
                            for (int k = 0; k < i; k++) uart_print("\b \b");
                            i = 0;
                            buf[i] = '\0';
                            cursor_pos = 0;
                            history_idx = -1;
                        }
                        continue;
                    }
                    // RIGHT ARROW
                    else if (arrow_char == 'C') {
                        if (cursor_pos < i) {
                            uart_putc(0x1B); uart_putc(0x5B); uart_putc('C');
                            cursor_pos++;
                        }
                        continue;
                    }
                    // LEFT ARROW
                    else if (arrow_char == 'D') {
                        if (cursor_pos > 0) {
                            uart_putc(0x1B); uart_putc(0x5B); uart_putc('D');
                            cursor_pos--;
                        }
                        continue;
                    }
                }
            } else if (c == '\n' || c == '\r') { // Normal character input handling
                uart_putc('\n');
                break;
            } else if (c == '\b' || c == 0x7F) { // Backspace or Delete
                if (cursor_pos > 0) {
                    for(int k = cursor_pos; k < i; k++) {
                        buf[k-1] = buf[k];
                    }
                    i--;
                    cursor_pos--;
                    uart_print("\b \b"); // Destructive backspace
                    // Redraw the rest of the line
                    for (int k = cursor_pos; k < i; k++) uart_putc(buf[k]);
                    uart_putc(' ');
                    // Move cursor back
                    for (int k = cursor_pos; k <= i; k++) uart_print("\b");
                }
            } else if (c == '\t') { // Handle TAB
                // Check buffer and see if any commands match
                if(cmd_list != NULL) {
                    int match_pos = 0, match_idx = 0;
                    int n = get_matches(buf, cmd_list, &match_pos, &match_idx);
                    if(n == 1) {
                        /* Only one match, get that command and complete the buf */
                        char *cmd = cmd_list[match_idx].name;
                        int z = 0;
                        for(z = match_pos; cmd[z] != '\0'; z++) {
                            buf[z] = cmd[z];
                            uart_putc(buf[z]);
                        }
                        i = z + 1;
                        cursor_pos = i;
                        buf[i] = ' ';
                        uart_putc(' ');
                    } else if (n > 1) {
                        /* Many matches, print them to console and exit. Leave buf as is. Don't add to history */
                        b_add_to_hist = 0;
                        uart_print("\n");
                        for(int cmd_idx = 0; cmd_list[cmd_idx].handler != NULL; cmd_idx++) {
                            int match_bit = (match_map[cmd_idx / 8] >> (cmd_idx % 8)) & 0x01;
                            if(match_bit == 1) {
                                uart_print(cmd_list[cmd_idx].name);
                                uart_putc('\n');
                            }
                        }
                        uart_putc('\n');
                        // print the prompt again
                        uart_print(PROMPT);
                        // print the buf again to screen
                        uart_print(buf);
                    } else {
                        /* No match, leave as is */
                    }
                }
            } else {
                if (i < max_len - 1) {
                    // Shift characters to the right for insertion
                    for(int k = i; k > cursor_pos; k--) {
                        buf[k] = buf[k-1];
                    }
                    buf[cursor_pos] = c;
                    i++;
                    cursor_pos++;
                    // Redraw the rest of the line
                    for (int k = cursor_pos - 1; k < i; k++) uart_putc(buf[k]);
                    // Move cursor back
                    for (int k = cursor_pos; k < i; k++) uart_print("\b");
                }
            }
        }
    }

    if (i < max_len) buf[i] = '\0';
    if (b_add_to_hist) add_to_history(buf);

    return i;
}