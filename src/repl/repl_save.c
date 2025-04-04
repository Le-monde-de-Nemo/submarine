#include "repl_save.h"
#include "aqua.h"
#include <stdio.h>
#include <string.h>

static struct aqua_t global_aqua;

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
    if (argc <= 1) {
        fprintf(stderr, "Usage: load <aquarium pathname>\n");
        return ok;
    }

    aqua__save_file(argv[1], global_aqua);
    return ok;
}
