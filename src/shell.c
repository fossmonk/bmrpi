#include <stdint.h>
#include "io.h"
#include "shell.h"
#include "strops.h"

#define MAX_LINE_LENGTH (256)
#define HISTORY_SIZE (10)

static char history[HISTORY_SIZE][MAX_LINE_LENGTH] = { 0 };

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

int shell_readline_with_echo(char *buf, int max_len) {
    int i = 0; // Current buffer length
    int cursor_pos = 0; // Current cursor position
    int c;
    int history_idx = -1;

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
            }

            // Normal character input handling
            else if (c == '\n' || c == '\r') {
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
    add_to_history(buf);

    return i;
}