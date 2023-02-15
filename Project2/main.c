#include "main.h"

/**
 * Entry point
 * @return exit code
 */
int main() {
    // infinite loop
    while (1) {
        printf("$ ");
        char *input = get_char_input();
        //printf("%s\n", input);
        tokenlist *tokens = get_tokens(input, " ");
        for(int i = 0; i < tokens->size; i++){
            printf("%s\n", tokens->items[i]);
        }
        free(input);
    }
    return 0;
}
