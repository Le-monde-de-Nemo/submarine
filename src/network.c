#include <arpa/inet.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <unistd.h>

#ifndef BUFLEN
#define BUFLEN 2048
#endif

#define MAX_EVENTS 10

extern int exited;

static struct epoll_event ev, events[MAX_EVENTS];
static int listen_sock, conn_sock, nfds, epollfd;
static socklen_t cli_socket_len;
static struct sockaddr_in serv_addr, cli_addr;

void do_use_fd(int fd);

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
}

/*
 *  Function used in a thread to deal with network requests
 *
 *  @params args: int file descriptor
 */
void* workerth(void* args)
{
    epollfd = epoll_create1(0);
    if (epollfd == -1) {
        perror("epoll_create1");
        exit(EXIT_FAILURE);
    }

    ev.events = EPOLLIN | EPOLLONESHOT;
    ev.data.fd = listen_sock;
    if (epoll_ctl(epollfd, EPOLL_CTL_ADD, listen_sock, &ev) == -1) {
        perror("epoll_ctl: listen_sock");
        exit(EXIT_FAILURE);
    }

    while (!exited) {
        nfds = epoll_wait(epollfd, events, MAX_EVENTS, 1000);
        if (nfds == -1) {
            perror("epoll_wait");
            exit(EXIT_FAILURE);
        }

        for (int n = 0; n < nfds; ++n) {
            if (events[n].data.fd == listen_sock) {
                conn_sock = accept(listen_sock,
                    (struct sockaddr*)&cli_addr, &cli_socket_len);

                if (conn_sock == -1) {
                    perror("accept");
                    exit(EXIT_FAILURE);
                }

                setnonblocking(conn_sock);
                ev.events = EPOLLIN | EPOLLET | EPOLLRDHUP | EPOLLHUP | EPOLLONESHOT;
                ev.data.fd = conn_sock;

                if (epoll_ctl(epollfd, EPOLL_CTL_ADD, conn_sock,
                        &ev)
                    == -1) {
                    perror("epoll_ctl() failed");
                    exit(EXIT_FAILURE);
                }

            }

            else {
                do_use_fd(events[n].data.fd);
            }

            if (epoll_ctl(epollfd, EPOLL_CTL_MOD, events[n].data.fd, &events[n]) == -1) {
                perror("epoll_ctl() failed");
                exit(EXIT_FAILURE);
            }
        }
    }

    return NULL;
}

void network_finalize()
{
    close(epollfd);
    close(listen_sock);
}

void do_use_fd(int fd)
{
    char readbuf[BUFLEN] = {};
    char writebuf[BUFLEN] = {};

    int n = read(fd, readbuf, BUFLEN);
    if (n < 0)
        perror("read() failed");

    strncpy(writebuf, readbuf, BUFLEN);

    n = write(fd, writebuf, strlen(writebuf));
    if (n < 0)
        perror("write() failed");
}
