#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <dirent.h>

static int subDirCount = 0;

void recursiveDir(char* path) {
    DIR *d;
    struct dirent *dir;
    d = opendir(path);
    if(d) {
        while((dir = readdir(d)) != NULL) {
            if(dir == NULL)
                continue;
            if(strcmp(dir->d_name, ".") == 0)
                continue;
            if(strcmp(dir->d_name, "..") == 0)
                continue;
            if(dir->d_name[0] == '.')
                continue;
            if(dir->d_type != DT_DIR) {
                int i = 0;
                while(i<subDirCount){
                    printf("|    ");
                    i++;
                }
                printf("|--- ");
                if(dir->d_type == DT_DIR){
                    printf("%s", strcat(dir->d_name, "/"));
                } else {
                    printf("%s", dir->d_name);
                }
                printf("\n");
            } else {
                int i = 0;
                while(i<subDirCount){
                    printf("|    ");
                    i++;
                }
                printf("|--- ");
                if(dir->d_type == DT_DIR){
                    printf("%s", strcat(dir->d_name, "/"));
                } else {
                    printf("%s", dir->d_name);
                }
                printf("\n");
                bool mf = false;
                char* new_path = NULL;
                char* path_copy = NULL;
                if(strcmp(path, ".") != 0) {
                    path_copy = (char*) malloc(strlen(path)+strlen(dir->d_name)+2);
                    memcpy(path_copy, path, strlen(path)+1);
                    if(path_copy[strlen(path)-1] == '/'){
                        new_path = strcat(path_copy, (char*)dir->d_name);
                    } else {
                        new_path = strcat(path_copy, "/");
                        new_path = strcat(new_path, (char*)dir->d_name);
                    }
                    mf = true;
                } else {
                    new_path = dir->d_name;
                }
                subDirCount++;
                recursiveDir(new_path);
                subDirCount--;
                if(mf) {
                    free(path_copy);
                }
            }
        }
        closedir(d);
    } else {
        fprintf(stderr, "Error accessing directory - %s\n", path);
    }
}

int main(int argc, char **argv) {

    if(argc == 1) {
        printf("%s\n", ".");
        recursiveDir(".");
    } else {
        printf("%s\n", argv[1]);
        recursiveDir(argv[1]);
    }

    return 0;
}