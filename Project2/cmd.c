#include "cmd.h"

static const char *commands[3] = {"myexit", "mycd", "mypwd"};

/**
 * Execute built-in function
 * @param tokens
 */
void execute_command(tokenlist *tokens) {
    char *command = tokens->items[0];
    if (strcmp(command, commands[0]) == 0) {
        execute_exit();
    } else if (strcmp(command, commands[1]) == 0) {
        execute_cd(tokens);
    } else if (strcmp(command, commands[2]) == 0) {
        execute_pwd();
    } else execute_cmd(tokens, true);
}

/**
 * Assuming first token is the file that can be found in PATH to be executed, 
 * and all following tokens are args to the command.
 * We execute this with a fork
 * @param tokens
 * @param should_fork
*/
void execute_cmd(tokenlist *tokens, bool should_fork) {
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

/**
 * Exit implementation
 */
void execute_exit() {
    // wait for any child processes
    waitpid(-1, NULL, 0);
    // exiting
    exit(0);
}

/**
 * Change directory
 * @param tokens
 */
void execute_cd(tokenlist *tokens) {
    char *path = tokens->items[1];
    bool path_changed = false;
    if (tokens->size > 2) {
        fprintf(stderr, "mycd: too many arguments\n");
        return;
    } else if (tokens->size == 1) {
        path = get_env_val("HOME");
        path_changed = true;
    }
    // change dir
    if (chdir(path) == 0) {
        // set pwd env var
        char *cwd = getcwd(NULL, 0);
        setenv("PWD", cwd, 1);
        free(cwd);
        if (path_changed) free(path);
    } else {
        printf("mycd: %s: No such file or directory\n", path);
        return;
    }
}

/**
 * Show present working directory
*/
void execute_pwd() {
    printf("%s\n", get_env_val("PWD"));
}
