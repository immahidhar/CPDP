#include "main.h"

void eof_sig_handler(int signum) {
  // kill process
  exit(0);
}

/**
 * Entry point
 * @return exit code
 */
int main() {
    signal( SIGINT, eof_sig_handler);
    addPWDToPATH();
    setpgid(0, 0);
    // infinite loop
    while (1) {
        printf("$ ");
        char *input = get_char_input();
        //printf("%s\n", input);
        tokenlist *tokens = get_tokens(input, " ");
        /*for(int i = 0; i < tokens->size; i++)
            printf("%s\n", tokens->items[i]);*/
        if (tokens->size != 0) process_command(tokens);
        free(input);
    }
    return 0;
}
