#include "repl_show.h"
#include "aqua.h"
#include <stdio.h>
#include <string.h>

#define REPL_SHOW_BUFLEN 2048

extern struct aqua_t global_aqua;

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
    char buf[REPL_SHOW_BUFLEN] = {};
    aqua__disp(global_aqua, buf, REPL_SHOW_BUFLEN);
    printf("%s\n", buf);
    return ok;
}
