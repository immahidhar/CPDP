#include "processor.h"

/**
 * Process the given input before command execution and then execute it.
 * @param tokens
 */
void process_command(tokenlist *tokens) {

    if (check_if_bif(tokens->items[0])) {
        execute_cmd(tokens);
    } else if (checkIfIORedirect(tokens)) {
        execute_io_redirect_command(tokens, true);
    } else execute_command(tokens, true);

}

/**
 * Check if we need to do IO redirection
 * @param tokens
 * @return
 */
bool checkIfIORedirect(tokenlist *tokens) {
    if (token_present(tokens, ">") != -1 || token_present(tokens, "<") != -1) return true;
    else return false;
}

/**
 * Executes command with io redirection implemented.
 * This should work for both input and output redirection at a time.
 * @param tokens
 * @param should_fork
 */
void execute_io_redirect_command(tokenlist *tokens, bool should_fork) {
    int fdo = -1;
    int fdi = -1;
    int outIndex = token_present(tokens, ">");
    if (outIndex != -1) {
        char *filename = tokens->items[outIndex + 1];
        if (filename == NULL) {
            fprintf(stderr, "No filename given for IO redirect \n");
        }
        // create file with all permissions if not present
        fdo = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0777);
        if (fdo == -1) {
            fprintf(stderr, "Error opening file - %s\n", filename);
            return;
        }
        char *rm_tokens[2] = {tokens->items[outIndex], tokens->items[outIndex + 1]};
        remove_tokens(tokens, rm_tokens, 2);
    }
    int inIndex = token_present(tokens, "<");
    if (inIndex != -1) {
        char *filename = tokens->items[inIndex + 1];
        if (filename == NULL) {
            fprintf(stderr, "No filename given for IO redirect \n");
        }
        // open file with all permissions
        fdi = open(filename, O_RDONLY, 0777);
        if (fdi == -1) {
            fprintf(stderr, "Error opening file - %s\n", filename);
            return;
        }
        char *rm_tokens[2] = {tokens->items[inIndex], tokens->items[inIndex + 1]};
        remove_tokens(tokens, rm_tokens, 2);
    }
    int pid = 0;
    if(should_fork) pid = fork();
    if (pid == 0) {
        // Child process
        if (outIndex != -1) {
            close(STDOUT_FILENO);
            dup2(fdo, STDOUT_FILENO);
            close(fdo);
        }
        if (inIndex != -1) {
            close(STDIN_FILENO);
            dup2(fdi, STDIN_FILENO);
            close(fdi);
        }
        // execute
        execute_command(tokens, false);
        return;
    } else {
        // parent process
        if (fdo != -1) close(fdo);
        if (fdi != -1) close(fdi);
        waitpid(pid, NULL, 0);
    }
}
