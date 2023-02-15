#include "cmd.h"

static const char *commands[3] = {"myexit", "mycd", "mypwd"};

/**
 * Exit implementation
 */
void execute_exit() {
    // wait for child processes
    waitpid(-1, NULL, 0);
    // exiting
    exit(0);
}

/**
 * Execute built-in function
 * @param tokens
 */
void execute_command(tokenlist *tokens) {
    char *command = tokens->items[0];
    if (strcmp(command, commands[0]) == 0) {
        execute_exit();
    }
}
