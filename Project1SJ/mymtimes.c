#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <dirent.h>
#include <time.h>
#include <sys/time.h>
#include <sys/stat.h>

long c_t;
int counts[24];

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
                struct stat stat_r;
                char* path_copy = NULL;
                char* file_name = NULL;
                path_copy = (char*) malloc(strlen(path)+strlen(dir->d_name)+2);
                memcpy(path_copy, path, strlen(path)+1);
                if(path_copy[strlen(path_copy)-1] == '/'){
                    file_name = strcat(path_copy, (char*)dir->d_name);
                } else {
                    file_name = strcat(path_copy, "/");
                    file_name = strcat(file_name, (char*)dir->d_name);
                }
                if(stat(file_name, &stat_r)==0) {
                    long mT = stat_r.st_mtime;
                    long tD = c_t - mT;
                    int n = tD / 3600;
                    if(n < 24)
                        counts[n]++;
                }
                free(path_copy);
            } else {
                char* path_copy = NULL;
                char* new_path = NULL;
                path_copy = (char*) malloc(strlen(path)+strlen(dir->d_name)+2);
                memcpy(path_copy, path, strlen(path)+1);
                if(path_copy[strlen(path_copy)-1] == '/'){
                    new_path = strcat(path_copy, (char*)dir->d_name);
                } else {
                    new_path = strcat(path_copy, "/");
                    new_path = strcat(new_path, (char*)dir->d_name);
                }
                recursiveDir(new_path);
                free(path_copy);
            }
        }
        closedir(d);
    } else {
        fprintf(stderr, "Error accessing directory - %s\n", path);
    }
}

int main(int argc, char **argv) {

    struct timespec curr_time;
    clock_gettime(CLOCK_REALTIME, &curr_time);
    c_t = curr_time.tv_sec;
    for(int i = 0; i < 24; i++)
        counts[i] = 0;

    char* path = NULL;
    if(argc == 1) {
        path = "."; 
    } else {
        path = argv[1];
    }

    recursiveDir(path);

    char* ts = (char*) malloc(100);
    long t = c_t;
    for(int i = 24-1; i >= 0; i--) {
        time_t now_time;
        struct tm *now;
        now_time = t;
        now = localtime(&now_time);
        strftime(ts, 100, "%a %b %d %T %Y", now);
        printf("%s : %d\n", ts, counts[i]);
        t = t - 3600;
    }
    free(ts);

    return 0;
}