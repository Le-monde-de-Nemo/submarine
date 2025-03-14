#ifndef _REPL_LOAD_H_
#define _REPL_LOAD_H_

#include "repl.h"

extern struct repl_entry repl_entry_load;

int repl_load_match(int argc, char* argv[]);

void repl_load_exec(int argc, char* argv[]);
#endif
