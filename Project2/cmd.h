#ifndef CMD_H
#define CMD_H

#include "util.h"

void execute_command(tokenlist *tokens);
void execute_cmd(tokenlist *tokens, bool should_fork);
void execute_exit();
void execute_cd(tokenlist *tokens);
void execute_pwd();

#endif
