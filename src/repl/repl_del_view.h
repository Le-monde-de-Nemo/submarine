#ifndef _REPL_DEL_VIEW_H_
#define _REPL_DEL_VIEW_H_

#include "repl.h"

extern struct repl_entry repl_entry_del_view;

int repl_del_view_match(int argc, char* argv[]);

void repl_del_view_exec(int argc, char* argv[]);
#endif
