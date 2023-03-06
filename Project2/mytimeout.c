#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>

// child pid
int cid = 0;

/**
 * alarm signal handler which kills child process when invoked
*/
void sig_handler(int signum){
  // kill child process by sending terminate signal
  if(cid != 0) kill(cid, SIGTERM);
  exit(0);
}

/**
 * Executes command in a child process
*/
void executeCmd(char *argv[]) {
    cid = fork();
    if(cid == 0) {
        // child process
        execvp(argv[2], &argv[2]);
        fprintf(stderr, "Command not found - %s\n", argv[2]);
        exit(-1);
    } else {
        // parent process
        waitpid(cid, NULL, 0);
    }
}

/**
 * Executes a given command with timeout
*/
int main(int argc, char *argv[]) {
    if(argc <= 2) {
        fprintf(stderr, "mytimeout: correct usage - mytimeout secs cmd [cmdargs ...]\n");
        exit(-1);
    }
    int seconds = atoi(argv[1]);
    if(seconds<=0) {
        fprintf(stderr, "mytimeout: correct usage - timeout secs must be positive integer\n");
        exit(-1);
    }
    // Register signal handler
    signal(SIGALRM, sig_handler); 
    // set alarm
    alarm(seconds);
    // execute command
    executeCmd(argv);
    return 0;
}
