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
#include <netinet/in.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#ifndef BUFLEN
#define BUFLEN 2048
#endif

#define LHOST_WORKER "127.0.0.1"

/* Used by the worker. */
void error(char* msg)
{
    perror(msg);
    exit(1);
}

static int exited = 0;

static struct controller_t controller;

struct worker_t {
    int socketfd;
};

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
    return 0;
}

void* master(void* args)
{
    pthread_t worker_thread;

    int sockfd, newsockfd, portno, cli_socket_len;
    struct sockaddr_in serv_addr, cli_addr;
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
        error("ERROR opening socket");

    bzero((char*)&serv_addr, sizeof(serv_addr));

    portno = controller.port;

    serv_addr.sin_family = AF_INET;
    // serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    inet_pton(AF_INET, LHOST_WORKER, &serv_addr.sin_addr.s_addr);
    serv_addr.sin_port = htons(portno);

    if (bind(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0)
        error("ERROR on bind()");

    while (!exited) {
        listen(sockfd, 5);

        cli_socket_len = sizeof(cli_addr);
        newsockfd = accept(sockfd, (struct sockaddr*)&cli_addr, &cli_socket_len);
        if (newsockfd < 0)
            error("ERROR on accept()");

        pthread_create(&worker_thread, NULL, worker, (void*)newsockfd);
    }

    close(sockfd);
    fprintf(stderr, "exited!\n");
    return NULL;
}

//
// printf("=> read() ? ...");
// getchar();
// printf("OK\n");

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
