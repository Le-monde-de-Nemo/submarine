#include "repl.h"
#include "repl_add_view.h"
#include "repl_del_view.h"
#include "repl_help.h"
#include "repl_load.h"
#include "repl_quit.h"
#include "repl_save.h"
#include "repl_show.h"
#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

static int exited = 0;

void* master(void* args);

int main(int argc, char* argv[])
{
    pthread_t master_thread;
    pthread_create(&master_thread, NULL, master, NULL);

    struct repl_entry entries[] = {
        repl_entry_load,
        repl_entry_save,
        repl_entry_show,
        repl_entry_add_view,
        repl_entry_del_view,
        repl_entry_help,
        repl_entry_quit,
    };

    int n = sizeof(entries) / sizeof(struct repl_entry);
    struct repl repl = repl__create(entries, n);

    repl__run(repl);
    exited = 1;
    pthread_join(master_thread, NULL);

    repl__finalize(repl);

    return 0;
}

void* master(void* args)
{
    while (!exited) {
        fprintf(stderr, "salut je suis le maître !!\n");
        sleep(2);
    }

    fprintf(stderr, "exited!\n");
    return NULL;
}
