#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/**
 * execute the command passed in arg
 * @return exit code int
*/
int executeCommand(char *argv[]) {
    int pid = 0;
    if ((pid = fork()) == -1) {
        printf("Error while forking!");
        return -1;
    }
    if (pid == 0) {
        // Child process
        // execute
        //printf("executing %s\n", argv[1]);
        execvp(argv[1], argv);
        printf("Unknown command - %s\n", argv[1]);
        exit(-1);
    } else {
        // parent process
        waitpid(pid, NULL, 0);
    }
    return 0;
}

/**
 * main
 * @return exit code int
 */
int main(int argc, char *argv[]) {
    char* cmd = NULL;
    if(argc == 1) {
        printf("No command provided to time");
        return(1);
    } else {
        cmd = argv[1];
        return executeCommand(argv);
    }
}
