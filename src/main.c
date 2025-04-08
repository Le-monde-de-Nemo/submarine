#include "aqua.h"
#include "controller.h"
#include "repl.h"
#include "repl_add_view.h"
#include "repl_del_view.h"
#include "repl_help.h"
#include "repl_load.h"
#include "repl_quit.h"
#include "repl_save.h"
#include "repl_show.h"
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

#define LHOST_WORKER "127.0.0.1"

struct aqua_t global_aqua;

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

static int exited = 0;
static struct controller_t controller;

struct worker_t {
    int socketfd;
};

/*
 *  Thread functions
 */
void* master(void* args);

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
    printf("---------------\n\n");

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
    aqua__destroy_aqua(&global_aqua);
    return 0;
}

void master__finalize()
{
}

void* master(void* args)
{
    pthread_t worker_thread;

    socklen_t cli_socket_len;
    struct sockaddr_in serv_addr, cli_addr;

    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
        error("ERROR opening socket");

    // Make the port reusable
    int enabled = 1;
    int rc;
    rc = setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR,
        (char*)&enabled, sizeof(enabled));
    if (rc < 0) {
        perror("setsockopt() failed");
        close(sockfd);
        exit(-1);
    }

    bzero(&serv_addr, sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    // serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    inet_pton(AF_INET, LHOST_WORKER, &serv_addr.sin_addr.s_addr);

    int portno = controller.port;
    serv_addr.sin_port = htons(portno);

    if (bind(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0)
        error("ERROR on bind()");

    rc = fcntl(sockfd, F_SETFL, O_NONBLOCK);
    if (rc < 0) {
        perror("fcntl() failed");
        close(sockfd);
        exit(-1);
    }

    fds[0].fd = sockfd;
    fds[0].events = POLLIN;

    long timeout = 1000;

    listen(sockfd, 5);

    /*
     *  Loop accepting connections
     */
    while (!exited) {
        int rc = poll(fds, nfds, timeout);
        if (rc <= 0) {
            // No new connection
            continue;
        }

        int workerfd = 0;
        cli_socket_len = sizeof(cli_addr);

        do {
            printf("accept\n");
            workerfd = accept(sockfd, (struct sockaddr*)&cli_addr, &cli_socket_len);

            if (workerfd < 0) {
                printf("leave\n");
                break;
            }

            pthread_create(&worker_thread, NULL, worker, (void*)(long)workerfd);
        } while (workerfd != EWOULDBLOCK);
    }

    // Clean up
    close(sockfd);
    fprintf(stderr, "Master exited!\n");

    return NULL;
}

/*
 * :param args: an integer, it is a file descriptor.
 * `worker` will write in that file descriptor.
 * :return: nothing.
 */
// void* worker(void* args)
// {
//     if (!args) {
//         return NULL;
//     }

//     char buffer[BUFLEN] = {};
//     // bzero(buffer, BUFLEN);
//     int n;
//     int newsockfd = (int)args;

//     while (!exited) {
//         n = read(newsockfd, buffer, BUFLEN);
//         if (n < 0)
//             error("ERROR reading from socket");
//         printf("Here is the message: (size: %d bytes) %s\n", n, buffer);

//         n = write(newsockfd, "I got your message", 18);
//         if (n < 0)
//             error("ERROR writing to socket");
//     }

//     close(newsockfd);
//     fprintf(stderr, "exited!\n");
//     return NULL;
// }
