#include "mymtimes.h"

/**
 * main
 * @return exit code int
 */
int main(int argc, char *argv[]) {

    initVariables();

    char* path = NULL;
    if(argc == 1) path = "."; 
    else path = argv[1];
    
    exploreDirsRecursively(path);

    printModifiedCounts();

    return 0;
}

/**
 * set current time and initiate variables
*/
void initVariables(void) { 
    struct timeval curr_time;
    gettimeofday(&curr_time, NULL);
    current_time = curr_time.tv_sec;
    long time = current_time;
    for(int i = 0; i < NUM_HOURS; i++) {
        past_hours[i] = time - HR_IN_SECS;
        time = time - HR_IN_SECS;
    }
    for(int i = 0; i < NUM_HOURS; i++) mod_count[i] = 0;
}

/**
 * explore dirs recursively and calculate number of files modified
*/
void exploreDirsRecursively(char* path) {
    DIR *d;
    struct dirent *dir;
    d = opendir(path);

    if(d) {   
        while((dir = readdir(d)) != NULL) {

            if(dir == NULL || strcmp(dir->d_name, ".") == 0 || strcmp(dir->d_name, "..") == 0)
            //|| dir->d_name[0] == '.')
                continue;

            if(dir->d_type == DT_DIR) {
                char* newPath = NULL;
                newPath = (char*) malloc(strlen(path)+strlen(dir->d_name)+2);
                memcpy(newPath, path, strlen(path)+1);
                if(newPath[strlen(newPath)-1] != '/'){
                    newPath = strcat(newPath, "/");
                }
                newPath = strcat(newPath, (char*)dir->d_name);
                exploreDirsRecursively(newPath);
                free(newPath);
            } else {
                struct stat result;
                char* filename = NULL;
                filename = (char*) malloc(strlen(path)+strlen(dir->d_name)+2);
                memcpy(filename, path, strlen(path)+1);
                if(filename[strlen(filename)-1] != '/'){
                    filename = strcat(filename, "/");
                }
                filename = strcat(filename, (char*)dir->d_name);
                if(stat(filename, &result)==0) {
                    long mod_time = result.st_mtime;
                    long timeDiff = current_time - mod_time;
                    int n = timeDiff / HR_IN_SECS;
                    if(n >= 0 && n < 24) mod_count[n]++;
                }
                free(filename);
            }
            
        }
        closedir(d);
    } else {
        fprintf(stderr, "Couldn't find/access directory - %s\n", path);
    }

}

/**
 * print number of files modified
*/
void printModifiedCounts(void) {
    char* timeString = (char*) malloc(100);
    for(int i = NUM_HOURS-1; i >= 0; i--)
        printf("%s : %d\n", getTimeString(past_hours[i], timeString), mod_count[i]);
    free(timeString);
}

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
