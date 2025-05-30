#include "aqua.h"
#include "debug.h"
#include "store.h"
#include "worker.h"
#include <arpa/inet.h>
#include <asm-generic/errno-base.h>
#include <bits/pthreadtypes.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <sys/epoll.h>
#include <sys/queue.h>
#include <sys/socket.h>
#include <unistd.h>

#ifndef BUFLEN
#define BUFLEN 2048
#endif

#define MAX_EVENTS 50

STAILQ_HEAD(events_fifo_t, events_entry_t);

struct events_entry_t {
    STAILQ_ENTRY(events_entry_t)
    link;
    struct epoll_event event;
    struct worker__fsm_state* state;
    pthread_mutex_t conn_mutex;
    int fd;
};

static struct events_fifo_t available_events;
static pthread_mutex_t available_events_mutex;
static struct events_entry_t events_entries[MAX_EVENTS];
static int listen_sock, nfds, epollfd;
static socklen_t cli_socket_len;
static struct sockaddr_in serv_addr, cli_addr;

int do_use_fd(struct events_entry_t* event);

void setnonblocking(int fd)
{
    int rc = fcntl(fd, F_SETFL, fcntl(fd, F_GETFL, 0) | O_NONBLOCK);

    if (rc < 0) {
        perror("fcntl() failed");
        close(fd);
        exit(-1);
    }
}

void network_init(char* host, int portno)
{
    pthread_mutex_init(&available_events_mutex, NULL);

    STAILQ_INIT(&available_events);
    for (int i = 0; i < MAX_EVENTS; ++i) {
        events_entries[i].state = malloc(sizeof(*events_entries[i].state));
        events_entries[i].event.data.ptr = &events_entries[i];
        STAILQ_INSERT_TAIL(&available_events, &events_entries[i], link);
    }

    listen_sock = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
    if (listen_sock < 0)
        perror("socket() failed");

    // Make the port reusable
    int enabled = 1;
    int rc;
    rc = setsockopt(listen_sock, SOL_SOCKET, SO_REUSEADDR,
        (char*)&enabled, sizeof(enabled));
    if (rc < 0) {
        perror("setsockopt() failed");
        close(listen_sock);
        exit(-1);
    }

    bzero(&serv_addr, sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    // serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    inet_pton(AF_INET, host, &serv_addr.sin_addr.s_addr);

    serv_addr.sin_port = htons(portno);

    if (bind(listen_sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0)
        perror("bind() failed");

    setnonblocking(listen_sock);
    listen(listen_sock, 0);

    epollfd = epoll_create1(0);
    if (epollfd == -1) {
        perror("epoll_create1");
        exit(EXIT_FAILURE);
    }

    pthread_mutex_lock(&available_events_mutex);
    struct events_entry_t* event = STAILQ_FIRST(&available_events);
    STAILQ_REMOVE_HEAD(&available_events, link);
    pthread_mutex_unlock(&available_events_mutex);

    event->event.events = EPOLLIN | EPOLLONESHOT;
    event->fd = listen_sock;
    if (epoll_ctl(epollfd, EPOLL_CTL_ADD, listen_sock, &event->event) == -1) {
        perror("epoll_ctl: listen_sock");
        exit(EXIT_FAILURE);
    }
}

void accept_new_conn(int epollfd, int listen_sock, struct events_entry_t* event)
{
    int conn_sock = accept(listen_sock,
        (struct sockaddr*)&cli_addr, &cli_socket_len);

    if (conn_sock == -1) {
        perror("accept() failed");
        exit(EXIT_FAILURE);
    }

    setnonblocking(conn_sock);

    pthread_mutex_lock(&available_events_mutex);
    struct events_entry_t* new_event = STAILQ_FIRST(&available_events);
    STAILQ_REMOVE_HEAD(&available_events, link);
    pthread_mutex_unlock(&available_events_mutex);

    new_event->event.events = EPOLLIN | EPOLLONESHOT;
    new_event->fd = conn_sock;
    worker__init_fsm_state(new_event->state);
    pthread_mutex_init(&new_event->conn_mutex, NULL);

    if (epoll_ctl(epollfd, EPOLL_CTL_ADD, conn_sock, &new_event->event) == -1) {
        perror("epoll_ctl() failed");
        exit(EXIT_FAILURE);
    }

    if (epoll_ctl(epollfd, EPOLL_CTL_MOD, event->fd, &event->event) == -1) {
        perror("epoll_ctl() failed");
        exit(EXIT_FAILURE);
    }
}

/*
 *  Function used in a thread to deal with network requests
 *
 *  @params args: int file descriptor
 */
void* workerth(void* args)
{
    struct epoll_event events[MAX_EVENTS];
    struct events_entry_t* event;

    while (!store.exited) {
        nfds = epoll_wait(epollfd, events, MAX_EVENTS, 1000);
        if (nfds == -1) {
            perror("epoll_wait");
            exit(EXIT_FAILURE);
        }

        for (int n = 0; n < nfds; ++n) {
            event = events[n].data.ptr;

            if (event->fd == listen_sock)
                accept_new_conn(epollfd, listen_sock, event);

            else if (events[n].events & (EPOLLHUP | EPOLLRDHUP)) {
                epoll_ctl(epollfd, EPOLL_CTL_DEL, event->fd, &events[n]);

                aqua__add_available_vue(event->state->vars.current_vue, &store.global_aqua);

                pthread_mutex_lock(&available_events_mutex);
                STAILQ_INSERT_TAIL(&available_events, event, link);
                pthread_mutex_unlock(&available_events_mutex);
            } // End client disconnection

            else {
                int rc = do_use_fd(event);

                if (rc || epoll_ctl(epollfd, EPOLL_CTL_MOD, event->fd, &events[n]) == -1) {
                    // Connection lost
                    pthread_mutex_lock(&available_events_mutex);
                    STAILQ_INSERT_TAIL(&available_events, event, link);
                    pthread_mutex_unlock(&available_events_mutex);
                }
            } // End client read/write
        }
    }

    return NULL;
}

void network_finalize()
{
    close(epollfd);
    close(listen_sock);
    for (int i = 0; i < MAX_EVENTS; ++i)
        free(events_entries[i].state);
}

int do_use_fd(struct events_entry_t* event)
{
    int rc = 0;
    if (pthread_mutex_trylock(&event->conn_mutex) & EBUSY)
        return 0;

    do {
        rc = worker__run_fsm_step(event->fd, event->state);
        if (rc)
            break;
    } while (event->state->protostate & ~READ_BUFF);
    pthread_mutex_unlock(&event->conn_mutex);

    return 0;
}
