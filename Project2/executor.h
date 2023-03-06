#ifndef EXECUTOR_H
#define EXECUTOR_H

#include "cmd.h"

void execute_command(tokenlist *tokens, bool should_fork);
void execute_io_redirect_command(tokenlist *tokens, bool should_fork);
void executePiping(tokenlist *tokens, int count);

#endif
