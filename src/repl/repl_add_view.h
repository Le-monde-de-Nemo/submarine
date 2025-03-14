#ifndef _REPL_ADD_VIEW_H_
#define _REPL_ADD_VIEW_H_

#include "repl.h"

extern struct repl_entry repl_entry_add_view;

int repl_add_view_match(int argc, char* argv[]);

enum _repl_cmd repl_add_view_exec(int argc, char* argv[]);
#endif
