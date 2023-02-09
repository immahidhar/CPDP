#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <dirent.h>
#include <time.h>
#include <sys/time.h>
#include <sys/stat.h>

#define NUM_HOURS 24
#define HR_IN_SECS 3600

static long current_time;
static long past_hours[NUM_HOURS];
static int mod_count[NUM_HOURS];

/**
 * return time and date
*/
char* getTimeString(long ts, char* timeString) {
    time_t nowtime;
    struct tm *nowtm;
    nowtime = ts;
    nowtm = localtime(&nowtime);
    strftime(timeString, 100, "%a %b %d %T %Y", nowtm);
    return timeString;
}

/**
 * print number of files modified
*/
void printModCounts(void) {
    char* timeString = (char*) malloc(100);
    for(int i = NUM_HOURS-1; i >= 0; i--)
        printf("%s : %d\n", getTimeString(past_hours[i], timeString), mod_count[i]);
    free(timeString);
}

/**
 * explore dirs recursively and calculate number of files modified
*/
void exploreDirs(char* path) {
    DIR *d;
    struct dirent *dir;
    d = opendir(path);

    if(d) {   
        while((dir = readdir(d)) != NULL) {

            if(dir == NULL || strcmp(dir->d_name, ".") == 0 || strcmp(dir->d_name, "..") == 0 
            || dir->d_name[0] == '.')
                continue;

            if(dir->d_type == DT_DIR) {
                char* pathCopy = NULL;
                char* newPath = NULL;
                pathCopy = (char*) malloc(strlen(path)+1000);
                memcpy(pathCopy, path, strlen(path)+1);
                if(pathCopy[strlen(pathCopy)-1] == '/'){
                    newPath = strcat(pathCopy, (char*)dir->d_name);
                } else {
                    newPath = strcat(pathCopy, "/");
                    newPath = strcat(newPath, (char*)dir->d_name);
                }
                exploreDirs(newPath);
                free(pathCopy);
            } else {
                struct stat result;
                char* pathCopy = NULL;
                char* filename = NULL;
                pathCopy = (char*) malloc(strlen(path)+1000);
                memcpy(pathCopy, path, strlen(path)+1);
                if(pathCopy[strlen(pathCopy)-1] == '/'){
                    filename = strcat(pathCopy, (char*)dir->d_name);
                } else {
                    filename = strcat(pathCopy, "/");
                    filename = strcat(filename, (char*)dir->d_name);
                }
                if(stat(filename, &result)==0) {
                    long mod_time = result.st_mtime;
                    long timeDiff = current_time - mod_time;
                    int n = timeDiff / HR_IN_SECS;
                    if(n < 24)
                        mod_count[n]++;
                }
                free(pathCopy);
            }
            
        }
        closedir(d);
    } else {
        fprintf(stderr, "Couldn't find/access directory - %s\n", path);
    }

}

/**
 * set current time and initiate variables
*/
void setCurrentTime(void) { 
    struct timeval curr_time;
    gettimeofday(&curr_time, NULL);
    current_time = curr_time.tv_sec;
    long time = current_time;
    for(int i = 0; i < NUM_HOURS; i++) {
        past_hours[i] = time - HR_IN_SECS;
        time = time - HR_IN_SECS;
    }
    for(int i = 0; i < NUM_HOURS; i++)
        mod_count[i] = 0;
}

/**
 * main
 * @return exit code int
 */
int main(int argc, char *argv[]) {

    setCurrentTime();

    char* path = NULL;

    if(argc == 1) path = "."; 
    else path = argv[1];
    
    exploreDirs(path);

    printModCounts();

    return(0);
}
