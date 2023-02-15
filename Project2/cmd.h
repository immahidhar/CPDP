#ifndef CMD_H
#define CMD_H

#define NUMOFBIFUNS 3

#include "util.h"

void execute_cmd(tokenlist *tokens);
bool check_if_bif(char *command);
void execute_exit();
void execute_cd(tokenlist *tokens);
void execute_pwd();

#endif
