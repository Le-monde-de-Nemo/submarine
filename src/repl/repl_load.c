#include "repl_load.h"
#include "aqua.h"
#include "repl.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>

static struct aqua_t global_aqua;

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
    if (argc <= 1) {
        fprintf(stderr, "Usage: load <aquarium pathname>\n");
        return ok;
    }

    if (access(argv[1], F_OK) != 0) {
        fprintf(stderr, "Err: file %s does not exist\n", argv[1]);
        return ok;
    }

    global_aqua = aqua__from_file(argv[1]);
    return ok;
}
