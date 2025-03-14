#include "repl_del_view.h"
#include <stdio.h>
#include <string.h>

struct repl_entry repl_entry_del_view = {
    .match = repl_del_view_match,
    .exec = repl_del_view_exec,
};

int repl_del_view_match(int argc, char* argv[])
{
    return argc > 1 && strcmp("del", argv[0]) == 0 && strcmp("view", argv[1]) == 0;
}

void repl_del_view_exec(int argc, char* argv[])
{
    printf("exec del_view\n");
    // Exec del_view or print error if wrong argv
}
