#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <time.h>
#include <stdlib.h>

void bring_me_modification_times(char *str);
static int counts[24];
int main(int argc, char **argv)
{
    if (argc > 1)
    {
        bring_me_modification_times(argv[1]);
    }
    else
    {
        bring_me_modification_times(".");
    }
    // printf("check point 4 \n");
    time_t curtime_val;
    struct tm *localTime;

    char asciiTime[26];
    for (int i = 0; i < 24; i++)
    {
        time(&curtime_val); // Get the current time
        localTime = localtime(&curtime_val);
        localTime->tm_hour -= i + 1;
        mktime(localTime);
        strcpy(asciiTime, asctime(localTime));
        int len = strlen(asciiTime);
        asciiTime[len - 1] = '\0';
        printf("%s : %d \n ", asciiTime, counts[i]);
    }
    return 0;
}

void bring_me_modification_times(char *str)
{
    // printf("%s \n", str);
    char *path = (char *)malloc(strlen(str) + 1000);
    struct dirent *cur_entry;
    time_t time_val;
    DIR *dir = opendir(str);
    struct stat file_info;
    if (dir == NULL)
    {
        perror("error");
    }
    // printf("check point 1 \n");
    while ((cur_entry = readdir(dir)) != NULL)
    {
        if (strcmp(".", cur_entry->d_name) != 0 && strcmp("..", cur_entry->d_name) != 0)
        {
            // sprintf(path, "%s/%s", str, cur_entry->d_name);
            strcpy(path, str);
            strcat(path, "/");
            strcat(path, cur_entry->d_name);
            // printf("%s\n", path);
            stat(path, &file_info);
            if (S_ISDIR(file_info.st_mode))
            {
                bring_me_modification_times(path);
            }
            else
            {
                // printf("check point 2 \n");
                // printf("%s\n", cur_entry->d_name);
                //  printf("check point 3 \n");
                time(&time_val);
                // printf(" %ld \n", file_info.st_mtime);
                int hour = (time_val - file_info.st_mtime) / 3600;
                if (hour >= 0 && hour < 24)
                {
                    counts[hour]++;
                }
            }
        }
    }

    closedir(dir);
}