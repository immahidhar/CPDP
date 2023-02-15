#ifndef PROCESSOR_H
#define PROCESSOR_H

#include "executor.h"

void process_command(tokenlist *tokens);
bool checkIfIORedirect(tokenlist *tokens);
void execute_io_redirect_command(tokenlist *tokens, bool should_fork);

#endif
