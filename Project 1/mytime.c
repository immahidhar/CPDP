#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/resource.h>

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
    struct rusage childUsage;
    struct timeval childStime, childUtime;
    if (pid == 0) {
        // Child process
        // execute
        execvp(argv[1], &argv[1]);
        printf("Unknown command - %s\n", argv[1]);
        exit(-1);
    } else {
        // parent process
        waitpid(pid, NULL, 0);
        getrusage(RUSAGE_CHILDREN, &childUsage);
        childUtime = childUsage.ru_utime;
        childStime = childUsage.ru_stime;
        printf("%s  %ld.%.6ds user  %ld.%.6ds system", argv[1], childUtime.tv_sec, (childUtime.tv_usec), 
            childStime.tv_sec, (childStime.tv_usec));
    }
    return 0;
}

/**
 * main
 * @return exit code int
 */
int main(int argc, char *argv[]) {
    if(argc == 1) {
        printf("No command provided to time");
        return -1;
    } else {
        return executeCommand(argv);
    }
}
