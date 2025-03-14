#include "repl_add_view.h"
#include <stdio.h>
#include <string.h>

struct repl_entry repl_entry_add_view = {
    .match = repl_add_view_match,
    .exec = repl_add_view_exec,
};

int repl_add_view_match(int argc, char* argv[])
{
    return argc > 1 && strcmp("add", argv[0]) == 0 && strcmp("view", argv[1]) == 0;
}

void repl_add_view_exec(int argc, char* argv[])
{
    printf("exec add_view\n");
    // Exec add_view or print error if wrong argv
}
