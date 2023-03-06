#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>

pid_t pid = 0;

void sig_handler(int signum){
  if(pid != 0) kill(pid, SIGTERM);
  exit(0);
}

int main(int argc, char *argv[]) {
    int timeout = 0;

    if (argc < 3) {
        printf("Usage: mytimeout.x <timeout> <command> [args]\n");
        return 1;
    }

    timeout = atoi(argv[1]);
    if (timeout <= 0) {
        printf("Timeout value must be a positive integer.\n");
        return 1;
    }

    signal(SIGALRM, sig_handler);  /* Disable alarm signal while waiting for child process */
    alarm(timeout);  /* Set timeout */

    pid = fork();
    if (pid == 0) {
        /* Child process */
        execvp(argv[2], &argv[2]);
        printf("Command not found - %s\n", argv[2]);
        exit(-1);
    } else if (pid > 0) {
        /* Parent process */
        int status;
        pid_t result = wait(&status);  /* Wait for child process to finish */
        if (result == -1) {
            perror("wait");
            return 1;
        } else {
            alarm(0);  /* Disable alarm signal after child process finishes */
            return 0;
        }
    } else {
        /* Fork error */
        perror("fork");
        return 1;
    }
}
