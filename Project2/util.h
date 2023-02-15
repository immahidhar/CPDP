#ifndef UTIL_H
#define UTIL_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>

typedef struct {
    int size;
    char **items;
} tokenlist;

char *get_char_input(void);
tokenlist *get_tokens(char *input, char *delimiter);
tokenlist *new_tokenlist(void);
void add_token(tokenlist *tokens, char *item);
void remove_tokens(tokenlist *tokens, char **rm_tokens, int rm_tokens_size);
int token_present(tokenlist *tokens, char *token);
void free_tokens(tokenlist *tokens);
char *get_env_val(char *env_var);

#endif
