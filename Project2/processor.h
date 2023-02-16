#ifndef PROCESSOR_H
#define PROCESSOR_H

#include "executor.h"

void process_command(tokenlist *tokens);
bool checkIfIORedirect(tokenlist *tokens);
int checkIfPipe(tokenlist *tokens);

#endif
