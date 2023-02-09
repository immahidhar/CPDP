#ifndef MY_TREE_H
#define MY_TREE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <dirent.h>

static int dirDepth = 0;

void exploreDirsRecursively(char* path);
void printDirName(struct dirent *dir);

#endif
