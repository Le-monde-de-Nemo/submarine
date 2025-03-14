#ifndef _REPL_SHOW_H_
#define _REPL_SHOW_H_

#include "repl.h"

extern struct repl_entry repl_entry_show;

int repl_show_match(int argc, char* argv[]);

void repl_show_exec(int argc, char* argv[]);
#endif
