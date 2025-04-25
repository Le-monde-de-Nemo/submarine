#include <arpa/inet.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <unistd.h>

#define MAX_EVENTS 10

static struct epoll_event ev, events[MAX_EVENTS];
static int listen_sock, conn_sock, nfds, epollfd;
static socklen_t cli_socket_len;
static struct sockaddr_in serv_addr, cli_addr;

void setnonblocking(int fd)
{
    int rc = fcntl(fd, F_SETFL, O_NONBLOCK);

    if (rc < 0) {
        perror("fcntl() failed");
        close(fd);
        exit(-1);
    }
}

void network_init(char* host, int portno)
{
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
        perror("socket() failed");

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
    inet_pton(AF_INET, host, &serv_addr.sin_addr.s_addr);

    serv_addr.sin_port = htons(portno);

    if (bind(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0)
        perror("bind() failed");

    setnonblocking(sockfd);
    listen(sockfd, MAX_EVENTS - 1);
}

/*
 *  Function used in a thread to deal with network requests
 *
 *  @params args: int file descriptor
 */
void* workernetwork(void* args)
{
    epollfd = epoll_create1(0);
    if (epollfd == -1) {
        perror("epoll_create1");
        exit(EXIT_FAILURE);
    }

    ev.events = EPOLLIN;
    ev.data.fd = listen_sock;
    if (epoll_ctl(epollfd, EPOLL_CTL_ADD, listen_sock, &ev) == -1) {
        perror("epoll_ctl: listen_sock");
        exit(EXIT_FAILURE);
    }

    for (;;) {
        nfds = epoll_wait(epollfd, events, MAX_EVENTS, -1);
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
                ev.events = EPOLLIN | EPOLLET;
                ev.data.fd = conn_sock;

                if (epoll_ctl(epollfd, EPOLL_CTL_ADD, conn_sock,
                        &ev)
                    == -1) {
                    perror("epoll_ctl: conn_sock");
                    exit(EXIT_FAILURE);
                }
            }

            else {
                continue;
                // do_use_fd(events[n].data.fd, state);
            }
        }
    }

    return NULL;
}
