#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <sys/resource.h>

#define BILLION  1000000000.0

int main(int argc, char **argv) 
{
    if(argc == 1) 
    {
        fprintf(stderr, "No command provided to time");
        return -1;
    } 
    else 
    {
        int pid = 0, stat = 0;
        struct rusage rusg;
        struct timespec start, end;
        struct timeval stime, utime;
        clock_gettime(CLOCK_REALTIME, &start);
        if ((pid = fork()) == 0) 
        {
            execvp(argv[1], &argv[1]);
            fprintf(stderr, "Error executing command - %s\n", argv[1]);
            exit(-1);
        } 
        else 
        {
            wait(&stat);
            clock_gettime(CLOCK_REALTIME, &end);
            double elapsed = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / BILLION;
            getrusage(RUSAGE_CHILDREN, &rusg);
            utime = rusg.ru_utime;
            stime = rusg.ru_stime;
            printf("%s - ", argv[1]);
            printf("user_time: %ld.%.6ds ", utime.tv_sec, utime.tv_usec);
            printf("sys_time: %ld.%.6ds  ", stime.tv_sec, stime.tv_usec);
            printf("elapsed_time: %lfs\n", elapsed);
        }
        return 0;
    }
}