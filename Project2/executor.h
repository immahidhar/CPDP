#ifndef EXECUTOR_H
#define EXECUTOR_H

#include "cmd.h"

void execute_command(tokenlist *tokens, bool should_fork);

#endif
