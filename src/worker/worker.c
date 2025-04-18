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
    CLEAN_VARS,
    READ_BUFF,
    PARSE_BUFF,
    PARSE_CMD,
    PING,
    HELLO, /* hello */
    GET_FISHES, /* getFishes */
    ADD_FISH, /* addFish */
    DEL_FISH, /* delFish */
    START_FISH, /* startFish */
    LOGOUT,
    ERROR
};

int worker__count_cmds(char* buffer, int buff_offset)
{
    int ncmds = 0;
    for (int i = buff_offset; i < BUFLEN; i++) {
        if (buffer[i] == '\n') {
            ncmds++;
        }
    }
    return ncmds;
}

/**
 * Verifies if the buffer contains a full cmd (i.e. contains a \n).
 * Returns -1 if the buffer doesn't contain a command, and the
 * offset indicating the first \n character otherwise
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
 * Separates words separated by spaces. The input string must end with \n
 */
void worker__parse_words(char* input, char words[COMMWORDS][BUFLEN])
{
    int inp_size = strlen(input);
    int widx = 0;
    int woff = 0;
    for (int i = 0; i < inp_size; i++) {
        switch (input[i]) {
        case ' ':
            words[widx][woff] = '\0';
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

/**
 * Returns the FSM state that corresponds to the given command
 */
enum PROTOSTATES worker__get_command_state(char* command)
{
    enum PROTOSTATES retval;

    if (strncmp(command, "ping", BUFLEN) == 0) {
        retval = PING;
    } else if (strncmp(command, "log", BUFLEN) == 0) {
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
    char words[COMMWORDS][BUFLEN] = {}; // words of the command
    int ncmds = 0; // number of commands stored in the buffer
    int nbytes_socket = 0; // number of bytes read on the last call of read(sockfd)
    char writebuf[BUFLEN];
    struct vue_t* current_vue = NULL;

    // FSM - execution of the commands from the "affichage <-> controlleur" protocol
    int protostate = READ_BUFF;
    int exited = FALSE;

    while (!exited) {

        switch (protostate) {

        case CLEAN_VARS:
            memset(cmd, 0, sizeof(cmd));
            memset(words, 0, sizeof(words));
            protostate = READ_BUFF;
            break;

        case READ_BUFF:
            printf("in READ_BUFF:\n"); // LOG
            // Reads from socket only if there are no commands on the buffer
            if (ncmds == 0) {
                nbytes_socket = read(sockfd, buffer + buff_offset, BUFLEN - buff_offset);
                ncmds = worker__count_cmds(buffer, buff_offset);
                if (nbytes_socket == -1) {
                    perror("Error when reading the socket\n"); // LOG
                    protostate = ERROR;
                }
            } else {
                // There are still other commands to read on the buffer
                protostate = PARSE_BUFF;
            }
            break;

        case PARSE_BUFF:
            int endcmd_offset = worker__find_cmd_in_buffer(buffer, buff_offset);
            if (endcmd_offset == -1) {
                buff_offset += nbytes_socket;
                if (buff_offset >= BUFLEN) {
                    printf("Worker buffer overflow\n"); // LOG
                    protostate = ERROR;
                } else {
                    // if the command is incomplete, the worker remains reading the
                    // bytes sent to the socket
                    printf("Incomplete command, going back to reading the socket\n"); // LOG
                    protostate = READ_BUFF;
                }

            } else {
                memset(cmd, 0, BUFLEN);
                // stores the command (max of BUFLEN bytes)
                strncpy(cmd, buffer, endcmd_offset + 1);
                // saves the content of the buffer located after the command
                char new_buffer[BUFLEN] = {};
                strncpy(new_buffer, buffer + endcmd_offset + 1, BUFLEN - (endcmd_offset + 1));
                strncpy(buffer, new_buffer, BUFLEN);
                ncmds--; // command consumed from the buffer
                buff_offset = 0; // new command located at the beginning of the buffer
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

            struct fish_t* fishExists = aqua__get_fish(words[1], global_aqua);
            int already_exists = (fishExists != NULL);

            // PoissonRouge at 90x40,10x4, RandomWayPoint
            int x_pos = -1, y_pos = -1;
            int width = -1, height = -1;
            struct vec2 pos, size;
            struct fish_t fish2add;

            if (!already_exists) {
                sscanf(words[3], "%dx%d,%dx%d", &x_pos, &y_pos, &width, &height);

                pos = vec2__create(x_pos, y_pos);
                size = vec2__create(width, height);

                fish2add = fish__init_fish(words[1], pos, size, words[4]);
                global_aqua = aqua__add_fish(fish2add, global_aqua);
            }

            proto__add_fish(writebuf, BUFLEN, already_exists);

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

            struct fish_t* fish2del = aqua__get_fish(words[1], global_aqua);
            already_exists = (fish2del != NULL);
            if (already_exists)
                global_aqua = aqua__del_fish(words[1], global_aqua);

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

            struct vec2 origin = vue__get_current_pos(*current_vue);

            proto__get_fishes(writebuf, BUFLEN, fishes, nb_fishes, origin);

            if (write(sockfd, writebuf, strlen(writebuf)) == -1) {
                perror("Error in writing the response to ping\n"); // LOG
                exited = TRUE;
            }

            free(fishes);

            protostate = READ_BUFF;
            break;

        case PING:
            bzero(writebuf, sizeof(writebuf));

            char* pingval = words[1];
            proto__ping(writebuf, sizeof(writebuf), pingval);
            printf("\twrite %s in socket %d\n", writebuf, sockfd); // LOG
            if (write(sockfd, writebuf, strlen(writebuf)) == -1) {
                perror("Error in writing the response to ping\n"); // LOG
                protostate = ERROR;
            }
            protostate = CLEAN_VARS;
            break;

        case LOGOUT:
            printf("in LOGOUT:\n"); // LOG
            if (strncmp(words[1], "out", BUFLEN) != 0) {
                fprintf(stderr, "Invalid log out command\n"); // LOG
                protostate = ERROR;
            } else {
                char writebuf[BUFLEN];
                proto__log(writebuf, sizeof(writebuf));
                printf("\twrite %s in socket %d\n", writebuf, sockfd); // LOG
                if (write(sockfd, writebuf, strlen(writebuf)) == -1) {
                    perror("Error in writing the response to log out\n"); // LOG
                    protostate = ERROR;
                }
                exited = TRUE;
            }
            break;

        case ERROR:
            exited = TRUE;
            break;

        default:
            fprintf(stderr, "Invalid FSM protocol state\n"); // LOG
            exited = TRUE;
        }
    }

    // Cleaning
    close(sockfd);

    printf("Exiting worker thread...\n"); // LOG
    return NULL;
}
