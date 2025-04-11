#include "repl_del_view.h"
#include "aqua.h"
#include <stdio.h>
#include <string.h>

extern struct aqua_t global_aqua;

struct repl_entry repl_entry_del_view = {
    .match = repl_del_view_match,
    .exec = repl_del_view_exec,
};

int repl_del_view_match(int argc, char* argv[])
{
    return argc > 1 && strcmp("del", argv[0]) == 0 && strcmp("view", argv[1]) == 0;
}

enum _repl_cmd repl_del_view_exec(int argc, char* argv[])
{
    int id;
    sscanf(argv[2], "N%d", &id);

    global_aqua = aqua__del_vue(id, global_aqua);

    printf("\t-> view N%d deleted\n", id);
    return ok;
}
