#include "processor.h"

/**
 * Process the given input before command execution and then execute it.
 * @param tokens
 */
void process_command(tokenlist *tokens) {

    // TODO: make bif external progs
    if (check_if_bif(tokens->items[0])) {
        execute_cmd(tokens);
    } else if (checkIfIORedirect(tokens)) {
        execute_io_redirect_command(tokens, true);
    } else if (checkIfPipe(tokens) > 0) {
        executePiping(tokens, checkIfPipe(tokens));
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
 * Check if we need to do piping
 * @param tokens
 * @return
 */
int checkIfPipe(tokenlist *tokens) {
    int count = 0;
    for (int i = 0; i < tokens->size; i++)
        if (strcmp(tokens->items[i], "|") == 0) count++;
    return count;
}
