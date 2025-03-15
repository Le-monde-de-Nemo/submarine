#include "repl_show.h"
#include <stdio.h>
#include <string.h>

struct repl_entry repl_entry_show = {
    .match = repl_show_match,
    .exec = repl_show_exec,
};

int repl_show_match(int argc, char* argv[])
{
    return argc > 1 && strcmp("show", argv[0]) == 0 && strcmp("aquarium", argv[1]) == 0;
}

enum _repl_cmd repl_show_exec(int argc, char* argv[])
{
    printf("exec show aquarium\n");
    // Exec show or print error if wrong argv
    return ok;
}
