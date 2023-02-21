#include "cmd.h"

static const char *commands[NUMOFBIFUNS] = {"myexit", "mycd", "mypwd"};

/**
 * Execute built-in function
 * @param tokens
 */
void execute_cmd(tokenlist *tokens) {
    char *command = tokens->items[0];
    if (strcmp(command, commands[0]) == 0) {
        execute_exit();
    } else if (strcmp(command, commands[1]) == 0) {
        execute_cd(tokens);
    } else if (strcmp(command, commands[2]) == 0) {
        //execute_pwd();
        execute_command(tokens, true);
    }
}

/**
 * Check if command is built-in function i.e. one of the above 3
 * @param command
 * @return
 */
bool check_if_bif(char *command) {
    for (int i = 0; i < NUMOFBIFUNS; i++) 
        if (strcmp(command, commands[i]) == 0) 
            return true;
    return false;
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
