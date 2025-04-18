#include "aqua.h"
#include "fish.h"
#include "proto.h"
#include "vec2.h"
#include "vue.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>

#define BUFLEN 4096
#define COMMWORDS 64
#define TRUE 1
#define FALSE 0

extern struct aqua_t global_aqua;

enum PROTOSTATES {
    READ_BUFF,
    PARSE_CMD,
    PING,
    HELLO, /* hello */
    GET_FISHES, /* getFishes */
    ADD_FISH, /* getFishes */
    DEL_FISH, /* getFishes */
    START_FISH, /* getFishes */
    LOGOUT,
    ERROR
};

/**
 * Verifies if the buffer contains a full cmd (i.e. contains a \n).
 * Returns -1 if the buffer doesn't contain a command, and the
 * offset indicating the \n character otherwise
 */
int worker__find_cmd_in_buffer(char* buffer, int buff_offset)
{
    for (int i = buff_offset; i < BUFLEN; i++) {
        if (buffer[i] == '\n') {
            return i;
        }
    }
    return -1;
}

/**
 * Separates words separated by spaces
 */
void worker__parse_words(char* input, char* words[])
{
    int inp_size = strlen(input);
    int widx = 0;
    int woff = 0;
    for (int i = 0; i < inp_size; i++) {
        switch (input[i]) {
        case ' ':
            widx++;
            woff = 0;
            break;
        case '\n':
            // ignore;
            break;
        default:
            words[widx][woff++] = input[i];
        }
    }
}

enum PROTOSTATES worker__get_command_state(char* command)
{
    enum PROTOSTATES retval;

    fprintf(stderr, "cmd: %s\n", command);

    if (strncmp(command, "ping", BUFLEN) == 0) {
        retval = PING;
    } else if (strncmp(command, "log", BUFLEN) == 0) { // add other commands
        retval = LOGOUT;
    } else if (strncmp(command, "hello", BUFLEN) == 0) {
        retval = HELLO;
    } else if (strncmp(command, "getFishes", BUFLEN) == 0) {
        retval = GET_FISHES;
    } else if (strncmp(command, "addFish", BUFLEN) == 0) {
        retval = ADD_FISH;
    } else if (strncmp(command, "delFish", BUFLEN) == 0) {
        retval = DEL_FISH;
    } else if (strncmp(command, "startFish", BUFLEN) == 0) {
        retval = START_FISH;
    } else {
        retval = ERROR;
    }

    return retval;
}

/*
 * :param args: an integer, it is a file descriptor.
 * `worker` will write in that file descriptor.
 * :return: nothing.
 */
void* worker(void* args)
{
    printf("Starting worker thread...\n"); // LOG

    int sockfd = (int)(long)args;
    char buffer[BUFLEN] = {}; // buffer (stores the commands from byte 0)
    int buff_offset = 0; // offset for unfinished commands
    char cmd[BUFLEN] = {}; // command read from buffer
    char* words[COMMWORDS]; // words of the command
    char writebuf[BUFLEN];
    struct vue_t* current_vue = NULL;

    for (int i = 0; i < COMMWORDS; i++) {
        words[i] = malloc(BUFLEN * sizeof(char));
        strcpy(words[i], "\0");
    }

    // FSM - execution of the commands from the "affichage <-> controlleur" protocol
    int protostate = READ_BUFF;
    int exited = FALSE;

    while (!exited) {
        switch (protostate) {
        case READ_BUFF:
            printf("in READ_BUFF:\n"); // LOG
            int bytes_read = read(sockfd, buffer + buff_offset, BUFLEN - buff_offset);

            // wait until a command is received
            if (bytes_read == -1) {
                perror("Error when reading the socket\n"); // LOG
                protostate = ERROR;
            }

            int endcmd_offset = worker__find_cmd_in_buffer(buffer, buff_offset);
            if (endcmd_offset == -1) {
                buff_offset += bytes_read;

                if (buff_offset >= BUFLEN) {
                    printf("Worker buffer overflow\n"); // LOG
                    protostate = ERROR;
                } else {
                    printf("Incomplete command, going back to reading the socket\n"); // LOG
                    protostate = READ_BUFF;
                    // if the command is incomplete, the worker remains reading the
                    // bytes sent to the socket
                }

            } else {
                memset(cmd, 0, BUFLEN);
                strncpy(cmd, buffer, endcmd_offset + 1); // stores the command (max of BUFLEN bytes)
                char new_buffer[BUFLEN] = {};

                if (endcmd_offset < BUFLEN - 1) {
                    // the \n is not the end character of the buffer
                    strncpy(new_buffer, buffer + endcmd_offset + 1, BUFLEN - (endcmd_offset + 1));
                }

                strncpy(buffer, new_buffer, BUFLEN);
                protostate = PARSE_CMD;
            }

            break;

        case PARSE_CMD:
            printf("in PARSE_CMD:\n"); // LOG
            worker__parse_words(cmd, words);
            protostate = worker__get_command_state(words[0]);
            break;

        case HELLO:
            bzero(writebuf, sizeof(writebuf));
            int id = -1;
            int nogreeting = 0;
            sscanf(words[3], "N%d", &id);

            current_vue = aqua__get_vue(id, global_aqua);
            proto__greeting(writebuf, sizeof(writebuf), id, nogreeting);

            if (write(sockfd, writebuf, strlen(writebuf)) == -1) {
                perror("Error in writing the response to ping\n"); // LOG
                exited = TRUE;
            }

            protostate = READ_BUFF;
            break;

        case ADD_FISH:
            bzero(writebuf, sizeof(writebuf));

            if (current_vue == NULL) {
                fprintf(stderr, "%p\n", current_vue);
                protostate = READ_BUFF;
                break;
            }

            // PoissonRouge at 90x40,10x4, RandomWayPoint
            int x_pos = -1, y_pos = -1;
            int x_target = -1, y_target = -1;
            struct vec2 pos = vec2__create(x_pos, y_pos);
            struct vec2 target = vec2__create(x_target, y_target);

            // struct fish_t fish2add = fish__init_fish(words[1], pos, size, const char *mobility_func)

            // proto__add_fish(writebuf, BUFLEN, fishes, nb_fishes);

            if (write(sockfd, writebuf, strlen(writebuf)) == -1) {
                perror("Error in writing the response to ping\n"); // LOG
                exited = TRUE;
            }

            protostate = READ_BUFF;
            break;

        case DEL_FISH:
            bzero(writebuf, sizeof(writebuf));

            if (current_vue == NULL) {
                fprintf(stderr, "%p\n", current_vue);
                protostate = READ_BUFF;
                break;
            }

            struct fish_t* fish2add = aqua__get_fish(words[1], global_aqua);
            int already_exists = (fish2add != NULL);
            if (already_exists)
                aqua__del_fish(words[1], global_aqua);

            proto__del_fish(writebuf, BUFLEN, already_exists);

            if (write(sockfd, writebuf, strlen(writebuf)) == -1) {
                perror("Error in writing the response to ping\n"); // LOG
                exited = TRUE;
            }

            protostate = READ_BUFF;
            break;

        case START_FISH:
            bzero(writebuf, sizeof(writebuf));

            if (current_vue == NULL) {
                fprintf(stderr, "%p\n", current_vue);
                protostate = READ_BUFF;
                break;
            }

            struct fish_t* fish2start = aqua__get_fish(words[1], global_aqua);
            already_exists = (fish2start != NULL);
            if (already_exists)
                *fish2start = fish__start_fish(*fish2start);

            proto__start_fish(writebuf, BUFLEN, already_exists);

            if (write(sockfd, writebuf, strlen(writebuf)) == -1) {
                perror("Error in writing the response to ping\n"); // LOG
                exited = TRUE;
            }

            protostate = READ_BUFF;
            break;

        case GET_FISHES:
            bzero(writebuf, sizeof(writebuf));

            if (current_vue == NULL) {
                fprintf(stderr, "%p\n", current_vue);
                protostate = READ_BUFF;
                break;
            }

            struct fish_t* fishes = aqua__get_fishes(global_aqua);
            int nb_fishes = aqua__get_nb_fishes(global_aqua);

            proto__get_fishes(writebuf, BUFLEN, fishes, nb_fishes);

            if (write(sockfd, writebuf, strlen(writebuf)) == -1) {
                perror("Error in writing the response to ping\n"); // LOG
                exited = TRUE;
            }

            protostate = READ_BUFF;
            break;

        case PING:
            printf("in PING:\n"); // LOG
            char* pingval = words[1];
            char writebuf[BUFLEN];
            proto__ping(writebuf, sizeof(writebuf), pingval);
            printf("\twrite %s in socket %d\n", writebuf, sockfd); // LOG
            if (write(sockfd, writebuf, strlen(writebuf)) == -1) {
                perror("Error in writing the response to ping\n"); // LOG
                exited = TRUE;
            }
            protostate = READ_BUFF;
            break;

        case LOGOUT:
            printf("in LOGOUT:\n"); // LOG
            if (strncpy(words[1], "out", BUFLEN) != 0) {
                fprintf(stderr, "Invalid log out command\n"); // LOG
                exited = TRUE;
            } else {
                exited = TRUE;
                // à finir!
            }
            break;

        default:
            fprintf(stderr, "Invalid FSM protocol state\n"); // LOG
            exited = TRUE;
        }
    }

    // Cleaning
    for (int i = 0; i < COMMWORDS; i++) {
        free(words[i]);
    }

    close(sockfd);

    printf("Exiting worker thread...\n"); // LOG
    return NULL;
}
