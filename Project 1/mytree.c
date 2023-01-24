#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <dirent.h>

static int dirDepth = 0;
static bool debug = false;

/**
 * prints directory name
*/
void printDirName(struct dirent *dir) {
    for(int i=0; i<dirDepth; i++)
        printf("|    ");
    printf("|--- ");
    if(dir->d_type == DT_DIR){
        printf("%s", strcat(dir->d_name, "/"));
    } else {
        printf("%s", dir->d_name);
    }
    printf("\n");
}

/**
 * explore the path and print directory contents recursively
*/
void printDirs(char* path) {
    DIR *d;
    struct dirent *dir;
    d = opendir(path);

    if(debug)
        printf("\nHERE\t%s\n", path);

    if(d) {
        while((dir = readdir(d)) != NULL) {
            if(dir == NULL || strcmp(dir->d_name, ".") == 0 || strcmp(dir->d_name, "..") == 0)
                continue;
            if(dir->d_type != DT_DIR) {
                printDirName(dir);
            } else {
                if(debug)
                    printf("\nHERE2\t%s\n", dir->d_name);
                printDirName(dir);
                //char* newPath = strcat(path, "/");
                //newPath = strcat(newPath, (char*)dir->d_name);
                char* newPath = NULL;
                if(strcmp(path, ".") == 0) {
                    newPath = dir->d_name;
                } else {
                    char* pathCopy = (char*) malloc(strlen(path)+1);
                    memcpy(pathCopy, path, strlen(path)+1);
                    newPath = strcat(pathCopy, (char*)dir->d_name);
                }
                dirDepth++;
                printDirs(newPath);
                dirDepth--;
            }
        }
        closedir(d);
    }
}

/**
 * main
 * @return exit code int
 */
int main(int argc, char *argv[]) {

    char* path = NULL;

    if(argc == 1) {
        path = ".";
    } else {
        path = argv[1];
    }

    printf("%s\n", path);
    printDirs(path);

    return(0);
}
