#ifndef _REPL_QUIT_H_
#define _REPL_QUIT_H_

#include "repl.h"

extern struct repl_entry repl_entry_quit;

int repl_quit_match(int argc, char* argv[]);

enum _repl_cmd repl_quit_exec(int argc, char* argv[]);
#endif
