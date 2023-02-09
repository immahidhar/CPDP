#ifndef MY_M_TIMES_H
#define MY_M_TIMES_H

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

void initVariables(void);
void exploreDirsRecursively(char* path);
void printModifiedCounts(void);
char* getTimeString(long ts, char* timeString);

#endif
