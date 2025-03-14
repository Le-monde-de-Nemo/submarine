#ifndef _REPL_SAVE_H_
#define _REPL_SAVE_H_

#include "repl.h"

extern struct repl_entry repl_entry_save;

int repl_save_match(int argc, char* argv[]);

enum _repl_cmd repl_save_exec(int argc, char* argv[]);
#endif
