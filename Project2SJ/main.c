#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>

#define MAX_ARGS 100

int args_count = 0;
int pipe_count = 0;

// Parse the input string to extract the command and its arguments
void parse_input(char *input, char **args) {
    char *token = strtok(input, " \n");
    int i = 0;
    while (token != NULL) {
        args[i] = token;
        token = strtok(NULL, " \n");
        i++;
    }
    args[i] = NULL;
    args_count = i;
}

void myexit() {
    exit(0);
}

// Implementation of mycd command
void mycd(char **args) {
    if (args_count < 2) {
        // No directory specified, go to home directory
        chdir(getenv("HOME"));
    } else {
        if (chdir(args[1]) == -1) {
            printf("%s: No such file or directory\n", args[0]);
        }
    }
    char *cwd = getcwd(NULL, 0);
    setenv("PWD", cwd, 1);
}

// Implementation of mypwd command
void mypwd() {
    char cwd[1024];
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        printf("%s\n", cwd);
    } else {
        perror("getcwd() error");
    }
}

/*void addTimeoutToPath() {
    char *temp = getenv("PATH");
    if (temp == NULL) temp = "";
    char *path = (char *) calloc(strlen(temp) + 1, 1);
    strcpy(path, temp);
    temp = getenv("CWD");
    if (temp == NULL) temp = "";
    char *cwd = (char *) calloc(strlen(temp) + 1, 1);
    path = (char*) realloc(path, strlen(path)+strlen(cwd)+2);
    strcat(path, ":");
    strcat(path, cwd);
    setenv("PATH", path, 1);
}*/

int isIPresent(char **args) {
    for(int i = 0; i < args_count; i++) {
        if(strcmp(args[i], "<") == 0) {
            return i;
        }
    }
    return -1;
}

int isOPresent(char **args) {
    for(int i = 0; i < args_count; i++) {
        if(strcmp(args[i], ">") == 0) {
            return i;
        }
    }
    return -1;
}

int isIOpresent(char **args) {
    if(isIPresent(args) > 0 || isOPresent(args) > 0) return 0;
    else return -1;
}

void executeWithIO(char **args) {
    int iIndex = isIPresent(args);
    int oIndex = isOPresent(args);
    FILE *iF, *oF;
    if(iIndex != -1) {
        iIndex = isIPresent(args);
        char *filename = args[iIndex + 1];
        iF = fopen(filename, "r");
        if (!iF) {
            fprintf(stderr, "Error: Failed to open input file %s\n", filename);
            return;
        }
        for(int i = 0; i < args_count + 1; i++) {
            if(strcmp(args[i], "<") == 0) {
                for(int k = 0; k < 2; k++) {
                    for(int j = i; j < args_count; j++) {
                        args[j] = args[j+1];
                    }
                }
                args_count--;
                args_count--;
                break;
            }
        }
        /*for(int j = 0; j < args_count; j++) printf("%s\n", args[j]);
        printf("%d\n", args_count);*/
    }
    if(oIndex != -1) {
        oIndex = isOPresent(args);
        char *filename = args[oIndex + 1];
        oF = fopen(filename, "w");
        if (!oF) {
            fprintf(stderr, "Error: Failed to open output file %s\n", filename);
            return;
        }
        for(int i = 0; i < args_count + 1; i++) {
            if(strcmp(args[i], ">") == 0) {
                for(int k = 0; k < 2; k++) {
                    for(int j = i; j < args_count; j++) {
                        args[j] = args[j+1];
                    }
                }
                args_count--;
                args_count--;
            }
        }
        /*for(int j = 0; j < args_count; j++) printf("%s\n", args[j]);
        printf("%d\n", args_count);*/
    }
    /*for(int j = 0; j < args_count; j++) printf("%s\n", args[j]);
    printf("%d\n", args_count);*/
    int pid = fork();
    if(pid == 0) {
        if(iIndex != -1) {
            int ifno = fileno(iF);
            close(STDIN_FILENO);
            dup2(ifno, STDIN_FILENO);
            close(ifno);
        }
        if(oIndex != -1) {
            int ofno = fileno(oF);
            close(STDOUT_FILENO);
            dup2(ofno, STDOUT_FILENO);
            close(ofno);
        }
        execvp(args[0], args);
        printf("Failed to execute command: %s\n", args[0]);
        exit(-1);
    } else {
        if(iIndex != -1) {
            close(fileno(iF));
        }
        if(oIndex != -1) {
            close(fileno(oF));
        }
        waitpid(pid, NULL, 0);
    }

}

int isPipePresent(char **args) {
    pipe_count = 0;
    for(int i = 0; i< args_count; i++) {
        if(strcmp(args[i], "|") == 0) {
            pipe_count++;
        }
    }
    return pipe_count;
}

void executeWithPipes(char **args) {
    if(pipe_count > 2) {
        printf("Only 2 pipes are supported\n");
        return;
    }
    //printf("%d\n", pipe_count);
    char *filename1 = "tmp";
    char *filename2 = "tmp2";
    FILE *tmpFile;
    char *pipe_command[120];
    int argCounter = 0;
    int flag = 0;
    for(int i=0; i < args_count; i++) {
        if(strcmp(args[i], "|") != 0) {
            pipe_command[argCounter] = args[i];
            argCounter++;
        } else {
            pipe_command[argCounter] = NULL;
            /*for(int j = 0; j < argCounter; j++)
                printf("%s ", pipe_command[j]);
            printf("\n");*/
            // fork and exec
            int pid = fork();
            if(pid == 0) {
                //child
                if(flag == 0) {
                    tmpFile = fopen(filename1, "w");
                    int ofno = fileno(tmpFile);
                    close(STDOUT_FILENO);
                    dup2(ofno, STDOUT_FILENO);
                    close(ofno);
                } else {
                    tmpFile = fopen(filename2, "w");
                    int ofno = fileno(tmpFile);
                    close(STDOUT_FILENO);
                    dup2(ofno, STDOUT_FILENO);
                    close(ofno);
                }
                if(flag != 0) {
                    tmpFile = fopen(filename1, "r");
                    int ifno = fileno(tmpFile);
                    close(STDIN_FILENO);
                    dup2(ifno, STDIN_FILENO);
                    close(ifno);
                }
                execvp(pipe_command[0], pipe_command);
                printf("Failed to execute command: %s\n", pipe_command[0]);
                exit(-1);
            } else {
                //parent
                waitpid(pid, NULL, 0);
                flag = 1;
            }
            argCounter = 0;
        }
    }
    pipe_command[argCounter] = NULL;
    /*for(int j = 0; j < argCounter; j++)
        printf("%s ", pipe_command[j]);
    printf("\n");*/
    // fork and exec
    int pid = fork();
    if(pid == 0) {
        //child
        if(pipe_count == 1) {
            tmpFile = fopen(filename1, "r");
        } else {
            tmpFile = fopen(filename2, "r");
        }
        int ifno = fileno(tmpFile);
        close(STDIN_FILENO);
        dup2(ifno, STDIN_FILENO);
        close(ifno);
        execvp(pipe_command[0], pipe_command);
        printf("Failed to execute command: %s\n", pipe_command[0]);
        exit(-1);
    } else {
        //parent
        waitpid(pid, NULL, 0);
    }
}


int main() {

    //addTimeoutToPath();

    char input[120];
    char *args[120];

    while (1) {
        // Print prompt and read input
        printf("$ ");
        if(fgets(input, sizeof(input), stdin) == NULL) exit(0);
        // Parse input
        parse_input(input, args);

        /*for(int j = 0; j < args_count; j++) printf("%s\n", args[j]);
        printf("%d\n", args_count);*/

        if (strcmp(args[0], "myexit") == 0) {
            myexit();
        } else if (strcmp(args[0], "mycd") == 0) {
            mycd(args);
        } else if (strcmp(args[0], "mypwd") == 0) {
            mypwd();
        } else if(isIOpresent(args) == 0) {
            executeWithIO(args);
        } else if(isPipePresent(args) > 0) {
            executeWithPipes(args);
        } else {
            pid_t pid = fork();
            if (pid < 0) {
                printf("Failed to fork process\n");
                exit(-1);
            } else if (pid == 0) { 
                execvp(args[0], args);
                printf("Failed to execute command: %s\n", args[0]);
                exit(-1);
            } else {
                waitpid(pid, NULL, 0);
            }
        }

        
    }

    
}
