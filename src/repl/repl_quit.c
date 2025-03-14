#include "repl_quit.h"
#include "repl.h"
#include <string.h>

struct repl_entry repl_entry_quit = {
    .match = repl_quit_match,
    .exec = repl_quit_exec,
};

int repl_quit_match(int argc, char* argv[])
{
    return strcmp("quit", argv[0]) == 0;
}

enum _repl_cmd repl_quit_exec(int argc, char* argv[])
{
    return quit;
}
