#include "repl_add_view.h"
#include "aqua.h"
#include "store.h"
#include "vec2.h"
#include "vue.h"
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

enum _repl_cmd repl_add_view_exec(int argc, char* argv[])
{
    int id, x, y, width, height;

    sscanf(argv[2], "N%d", &id);
    sscanf(argv[3], "%dx%d+%d+%d", &x, &y, &width, &height);

    struct vec2 pos = vec2__create(x, y);
    struct vec2 size = vec2__create(width, height);

    struct vue_t new_vue = vue__init_vue(id, pos, size);
    store.global_aqua = aqua__add_vue(new_vue, store.global_aqua);

    printf("\t-> view added\n");
    return ok;
}
