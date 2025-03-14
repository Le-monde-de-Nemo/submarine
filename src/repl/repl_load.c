#include "repl_load.h"
#include "repl.h"
#include <stdio.h>
#include <string.h>

struct repl_entry repl_entry_load = {
    .match = repl_load_match,
    .exec = repl_load_exec,
};

int repl_load_match(int argc, char* argv[])
{
    return strcmp("load", argv[0]) == 0;
}

enum _repl_cmd repl_load_exec(int argc, char* argv[])
{
    printf("exec load\n");
    // Exec load or print error if wrong argv
    return ok;
}
