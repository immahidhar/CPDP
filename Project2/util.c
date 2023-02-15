#include "util.h"

/**
 * Read command line from shell as a string
 * @return
 */
char *get_char_input(void) {
    char *buffer = NULL;
    int bufsize = 0;
    char line[5];
    while (fgets(line, 5, stdin) != NULL) {
        int addby = 0;
        char *newln = strchr(line, '\n');
        if (newln != NULL)
            addby = newln - line;
        else
            addby = 5 - 1;
        buffer = (char *) realloc(buffer, bufsize + addby);
        memcpy(&buffer[bufsize], line, addby);
        bufsize += addby;
        if (newln != NULL) break;
    }
    buffer = (char *) realloc(buffer, bufsize + 1);
    buffer[bufsize] = 0;
    return buffer;
}

/**
 * Tokenize given input using the delimeter passed
 * @param input
 * @param delimiter
 * @return
 */
tokenlist *get_tokens(char *input, char *delimiter) {
    char *buf = (char *) malloc(strlen(input) + 1);
    strcpy(buf, input);
    tokenlist *tokens = new_tokenlist();
    char *tok = strtok(buf, delimiter);
    while (tok != NULL) {
        add_token(tokens, tok);
        tok = strtok(NULL, delimiter);
    }
    free(buf);
    return tokens;
}

/**
 * Create a new tokenlist and ready it
 * @return
 */
tokenlist *new_tokenlist(void) {
    tokenlist *tokens = (tokenlist *) malloc(sizeof(tokenlist));
    tokens->size = 0;
    tokens->items = (char **) malloc(sizeof(char *));
    tokens->items[0] = NULL;
    return tokens;
}

/**
 * Adds a given token to the tokenlist passed
 * @param tokens - token list
 * @param item - token to be added
 */
void add_token(tokenlist *tokens, char *item) {
    int i = tokens->size;
    tokens->items = (char **) realloc(tokens->items, (i + 2) * sizeof(char *));
    tokens->items[i] = (char *) malloc(strlen(item) + 1);
    tokens->items[i + 1] = NULL;
    strcpy(tokens->items[i], item);
    tokens->size += 1;
}

/**
 * Gets environment variable value
 * NOTE: the returned value's memory should be freed up by the caller function
 * @param env_var
 * @return env_var value
 */
char *get_env_val(char *env_var) {
    char *temp;
    temp = getenv(env_var);
    if (temp == NULL) {
        temp = "";
    }
    char *env_val = (char *) calloc(strlen(temp) + 1, 1);
    strcpy(env_val, temp);
    return env_val;
}
