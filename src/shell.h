#ifndef _SHELL_H_
#define _SHELL_H_

#define PROMPT "rpi4b » "

typedef struct {
    char* name;
    char* help;
    void (*handler)(char**, int);
} cmd_t;

int shell_readline_with_echo(char *buf, int max_len, cmd_t *cmd_list);

#endif /* _SHELL_H_ */