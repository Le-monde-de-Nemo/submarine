#ifndef _REPL_HELP_H_
#define _REPL_HELP_H_

#include "repl.h"

extern struct repl_entry repl_entry_help;

int repl_help_match(int argc, char* argv[]);

enum _repl_cmd repl_help_exec(int argc, char* argv[]);
#endif
