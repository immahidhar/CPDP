#include "mytree.h"

/**
 * main
 * @return exit code int
 */
int main(int argc, char *argv[]) {
    char* path = NULL;
    if(argc == 1) path = ".";
    else path = argv[1];
    printf("%s\n", path);
    exploreDirsRecursively(path);
    return(0);
}


/**
 * explore the path and print directory contents recursively
*/
void exploreDirsRecursively(char* path) {
    DIR *d;
    struct dirent *dir;
    d = opendir(path);
    if(d) {
        while((dir = readdir(d)) != NULL) {
            //skip unncecessary dirs
            if(dir == NULL || strcmp(dir->d_name, ".") == 0 || strcmp(dir->d_name, "..") == 0 
            || dir->d_name[0] == '.')
                continue;
            if(dir->d_type != DT_DIR) {
                printDirName(dir);
            } else {
                printDirName(dir);
                bool memFlag = false;
                char* pathCopy = NULL;
                char* newPath = NULL;
                if(strcmp(path, ".") == 0) {
                    newPath = dir->d_name;
                } else {
                    memFlag = true;
                    pathCopy = (char*) malloc(strlen(path)+strlen(dir->d_name)+2);
                    memcpy(pathCopy, path, strlen(path)+1);
                    if(pathCopy[strlen(path)-1] == '/') {
                        newPath = strcat(pathCopy, (char*)dir->d_name);
                    } else {
                        newPath = strcat(pathCopy, "/");
                        newPath = strcat(newPath, (char*)dir->d_name);
                    }
                }
                dirDepth++;
                exploreDirsRecursively(newPath);
                dirDepth--;
                if(memFlag) free(pathCopy);
            }
        }
        closedir(d);
    } else {
        fprintf(stderr, "Couldn't find/access directory - %s\n", path);
    }
}


/**
 * prints directory/file name
*/
void printDirName(struct dirent *dir) {
    for(int i=0; i<dirDepth; i++) printf("|    ");
    printf("|--- ");
    if(dir->d_type == DT_DIR) printf("%s", strcat(dir->d_name, "/"));
    else printf("%s", dir->d_name);
    printf("\n");
}
