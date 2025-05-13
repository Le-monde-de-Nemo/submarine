#include "aqua.h"
#include "controller.h"
#include "debug.h"
#include "network.h"
#include "repl.h"
#include "repl_add_view.h"
#include "repl_del_view.h"
#include "repl_help.h"
#include "repl_load.h"
#include "repl_quit.h"
#include "repl_save.h"
#include "repl_show.h"
#include "store.h"
#include "vec2.h"
#include "vue.h"
#include "worker.h"
#include <arpa/inet.h>
#include <asm-generic/socket.h>
#include <bits/pthreadtypes.h>
#include <netinet/in.h>
#include <poll.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <sys/poll.h>
#include <sys/queue.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

// For async accept
#include <errno.h>
#include <fcntl.h>

#ifndef BUFLEN
#define BUFLEN 2048
#endif

#ifndef WORKERC
#define WORKERC 16
#endif

#ifndef DEFAULT_AQUAW
#define DEFAULT_AQUAW 1337
#endif

#ifndef DEFAULT_AQUAH
#define DEFAULT_AQUAH 1337
#endif

#define LHOST_WORKER "127.0.0.1"

#ifndef THREAD_POOL_SIZE
#define THREAD_POOL_SIZE 5
#endif

void store__init(void);
void store__finalize(void);

static pthread_t thread_pool[THREAD_POOL_SIZE];

int main(int argc, char* argv[])
{
    store__init();
    if (argc < 2)
        store.controller = controller__default();

    else
        store.controller = controller__load_conf(argv[1]);

    printf("Controller config :\n");
    printf("\n---------------\n");
    char buf[4096] = {};
    printf("%s", controller__disp(buf, sizeof(buf), store.controller));
    printf("---------------\n");

    network_init(LHOST_WORKER, store.controller.port);

    for (int i = 0; i < THREAD_POOL_SIZE; ++i)
        pthread_create(&thread_pool[i], NULL, workerth, NULL);

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
    store.exited = 1;

    for (int i = 0; i < THREAD_POOL_SIZE; ++i)
        pthread_join(thread_pool[i], NULL);

    network_finalize();
    repl__finalize(repl);
    store__finalize();
    return 0;
}

void store__init(void)
{
    store.exited = 0;

    /* Init store.global_aqua */
    struct vec2 size = vec2__create(DEFAULT_AQUAW, DEFAULT_AQUAH);
    struct vue_t n1 = vue__init_vue(1, vec2__zeros(), vec2__create(500, 500));
    struct vue_t n2 = vue__init_vue(2, vec2__create(500, 500), vec2__create(500, 500));
    struct vue_t n3 = vue__init_vue(3, vec2__create(0, 500), vec2__create(500, 500));
    struct vue_t n4 = vue__init_vue(4, vec2__create(500, 0), vec2__create(500, 500));
    store.global_aqua = aqua__init_aqua(size);
    store.global_aqua = aqua__add_vue(n1, store.global_aqua);
    store.global_aqua = aqua__add_vue(n2, store.global_aqua);
    store.global_aqua = aqua__add_vue(n3, store.global_aqua);
    store.global_aqua = aqua__add_vue(n4, store.global_aqua);
}

void store__finalize(void)
{
    aqua__destroy_aqua(&store.global_aqua);
}
