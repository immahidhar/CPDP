#ifndef CMD_H
#define CMD_H

#include <sys/wait.h>

#include "util.h"

void execute_exit();
void execute_command(tokenlist *tokens);

#endif
