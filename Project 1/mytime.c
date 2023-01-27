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
    struct rusage childUsage;
    struct timeval childStime, childUtime, childBegin, childEnd;
    gettimeofday(&childBegin, 0);
    if ((pid = fork()) == -1) {
        fprintf(stderr, "Error while forking!");
        return -1;
    }
    if (pid == 0) {
        // Child process
        // execute
        execvp(argv[1], &argv[1]);
        fprintf(stderr, "Command not found - %s\n", argv[1]);
        exit(-1);
    } else {
        // parent process
        waitpid(pid, NULL, 0);
        gettimeofday(&childEnd, 0);
        getrusage(RUSAGE_CHILDREN, &childUsage);
        childUtime = childUsage.ru_utime;
        childStime = childUsage.ru_stime;
        long elapsedSeconds = childEnd.tv_sec - childBegin.tv_sec;
        long elapsedMicroseconds = childEnd.tv_usec - childBegin.tv_usec;
        printf("%s  %ld.%.6ds user  %ld.%.6ds system  %ld.%.6lds elapsed\n", argv[1], childUtime.tv_sec, 
        childUtime.tv_usec, childStime.tv_sec, childStime.tv_usec, elapsedSeconds, elapsedMicroseconds);
    }
    return 0;
}

/**
 * main
 * @return exit code int
 */
int main(int argc, char *argv[]) {
    if(argc == 1) {
        fprintf(stderr, "No command provided to time");
        return -1;
    } else {
        return executeCommand(argv);
    }
}
