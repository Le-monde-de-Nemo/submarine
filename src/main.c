#include "aqua.h"
#include "controller.h"
#include "network.h"
#include "repl.h"
#include "repl_add_view.h"
#include "repl_del_view.h"
#include "repl_help.h"
#include "repl_load.h"
#include "repl_quit.h"
#include "repl_save.h"
#include "repl_show.h"
#include "vec2.h"
#include "vue.h"
#include "worker.h"
#include <arpa/inet.h>
#include <asm-generic/socket.h>
#include <netinet/in.h>
#include <poll.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <sys/poll.h>
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

extern struct aqua_t global_aqua;

static struct pollfd fds[WORKERC];
// Use poll because they say it's better
static int nfds = 1; // At the beginning, theres only one fd

/* Used by the worker. */
void error(char* msg)
{
    perror(msg);
    exit(1);
}

/*
 * Global variables
 */

int exited = 0;
static struct controller_t controller;

struct worker_t {
    int socketfd;
};

/*
 *  Thread functions
 */

int main(int argc, char* argv[])
{
    if (argc < 2)
        controller = controller__default();

    else
        controller = controller__load_conf(argv[1]);

    printf("Controller config :\n");
    printf("\n---------------\n");
    char buf[4096] = {};
    printf("%s", controller__disp(buf, sizeof(buf), controller));
    printf("---------------\n");

    struct vec2 size = vec2__create(DEFAULT_AQUAW, DEFAULT_AQUAH);
    struct vue_t n1 = vue__init_vue(1, vec2__zeros(), vec2__create(500, 500));
    struct vue_t n2 = vue__init_vue(2, vec2__create(500, 500), vec2__create(500, 500));
    struct vue_t n3 = vue__init_vue(3, vec2__create(0, 500), vec2__create(500, 500));
    struct vue_t n4 = vue__init_vue(4, vec2__create(500, 0), vec2__create(500, 500));
    global_aqua = aqua__init_aqua(size);
    global_aqua = aqua__add_vue(n1, global_aqua);
    global_aqua = aqua__add_vue(n2, global_aqua);
    global_aqua = aqua__add_vue(n3, global_aqua);
    global_aqua = aqua__add_vue(n4, global_aqua);

    network_init(LHOST_WORKER, controller.port);

    pthread_t threads[5];
    for (int i = 0; i < 5; ++i)
        pthread_create(&threads[i], NULL, workerth, NULL);

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
    for (int i = 0; i < 5; ++i)
        pthread_join(threads[i], NULL);
    network_finalize();

    repl__finalize(repl);
    aqua__destroy_aqua(&global_aqua);
    return 0;
}
