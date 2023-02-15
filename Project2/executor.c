#include "executor.h"

/**
 * Assuming first token is the file that can be found in PATH to be executed, 
 * and all following tokens are args to the command.
 * We execute this with a fork
 * @param tokens
 * @param should_fork
*/
void execute_command(tokenlist *tokens, bool should_fork) {
    int pid = 0;
    if(should_fork) pid = fork();
    if (pid == 0) {
        // In child process
        execvp(tokens->items[0], tokens->items);
        fprintf(stderr, "Command not found - %s\n", tokens->items[0]);
        exit(-1);
    } else {
        // In parent process
        waitpid(pid, NULL, 0);
    }
}
