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

/**
 * Piping implementation
 * @param tokens
 * @param should_fork
 */
void executePiping(tokenlist *tokens, int count,  bool shouled_fork) {
    //TODO: implement piping
    printf("piping isn't implemented yet! %d\n", count);
    int cmdCount = count + 1;
    tokenlist *commands[cmdCount];
    int cmdIndex = 0;
    tokenlist *command = new_tokenlist();
    printf("token size - %d\n", tokens->size);
    for(int i = 0; i < tokens->size; i++) {
        printf("checking token %s\n", tokens->items[i]);
        if(strcmp(tokens->items[i], "|") == 0) {
            commands[cmdIndex] = command;
            command = new_tokenlist();
            cmdIndex++;
        } else {
            printf("adding token %s\n", tokens->items[i]);
            add_token(command, tokens->items[i]);
        }
    }
    commands[cmdIndex] = command;
    /*for(int j = 0; j < cmdCount; j++) {
        printf("Command1:\n");
        for(int i = 0; i < commands[j]->size; i++)
            printf("%s\t", commands[j]->items[i]);
        printf("\n");
    }*/
    
    
}
