#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <time.h>
#include <sys/time.h>
#include <sys/stat.h>

int recursiveDir(char* path, long t, int count) 
{
    DIR *d;
    struct dirent *dir;
    d = opendir(path);

    if(d) 
    { 
        while((dir = readdir(d)) != NULL) 
        {
            if(dir == NULL)
            {
                continue;
            }
            if(strcmp(dir->d_name, ".") == 0)
            {
                continue;
            }
            if(strcmp(dir->d_name, "..") == 0)
            {
                continue;
            }
            if(dir->d_type != DT_DIR) 
            {
                struct stat stat_r;
                char* path_copy = NULL;
                char* file_name = NULL;
                path_copy = (char*) malloc(strlen(path)+strlen(dir->d_name)+2);
                memcpy(path_copy, path, strlen(path)+1);
                if(path_copy[strlen(path_copy)-1] == '/')
                {
                    file_name = strcat(path_copy, (char*)dir->d_name);
                } 
                else 
                {
                    file_name = strcat(path_copy, "/");
                    file_name = strcat(file_name, (char*)dir->d_name);
                }
                if(stat(file_name, &stat_r)==0) {
                    if(stat_r.st_mtime >= t && stat_r.st_mtime <= t + 3600)
                    {
                        count++;
                    }
                }
                free(path_copy);
            } 
            else 
            {
                char* path_copy = NULL;
                char* new_path = NULL;
                path_copy = (char*) malloc(strlen(path)+strlen(dir->d_name)+2);
                memcpy(path_copy, path, strlen(path)+1);
                if(path_copy[strlen(path_copy)-1] == '/')
                {
                    new_path = strcat(path_copy, (char*)dir->d_name);
                } 
                else 
                {
                    new_path = strcat(path_copy, "/");
                    new_path = strcat(new_path, (char*)dir->d_name);
                }
                count += recursiveDir(new_path, t, 0);
                free(path_copy);
            }
        }
        closedir(d);
    }
    return count;
}

int main(int argc, char **argv) 
{

    struct timespec curr_time;
    clock_gettime(CLOCK_REALTIME, &curr_time);

    char* path = NULL;
    if(argc == 1) 
    {
        path = "."; 
    } 
    else 
    {
        path = argv[1];
    }

    char* ts = (char*) malloc(200);
    long t = curr_time.tv_sec;
    t = t - (3600*24);
    for(int i = 0; i < 24; i++) 
    {
        time_t now_time;
        struct tm *now;
        now_time = t;
        now = localtime(&now_time);
        strftime(ts, 200, "%a %b %d %T %Y", now);
        printf("%s : %d\n", ts, recursiveDir(path, t, 0));
        t = t + 3600;
    }
    free(ts);

    return 0;
}