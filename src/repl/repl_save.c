#include "repl_save.h"
#include <stdio.h>
#include <string.h>

struct repl_entry repl_entry_save = {
    .match = repl_save_match,
    .exec = repl_save_exec,
};

int repl_save_match(int argc, char* argv[])
{
    return strcmp("save", argv[0]) == 0;
}

enum _repl_cmd repl_save_exec(int argc, char* argv[])
{
    printf("exec save\n");
    // Exec save or print error if wrong argv
    return ok;
}
