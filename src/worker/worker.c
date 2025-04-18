#include "proto.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define BUFLEN 1024
#define COMMWORDS 64
#define TRUE 1
#define FALSE 0

enum PROTOSTATES {
    READ_BUFF,
    PARSE_BUFF,
    PARSE_CMD,
    PING,
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
 * Separates words separated by spaces
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

    if (strncmp(command, "ping", 4) == 0) {
        retval = PING;
    } else if (strncmp(command, "log", 3) == 0) {
        retval = LOGOUT;
    } else { // add other commands
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

    // FSM - execution of the commands from the "affichage <-> controlleur" protocol
    int protostate = READ_BUFF;
    int exited = FALSE;

    while (!exited) {

        switch (protostate) {

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

        case PING:
            printf("in PING:\n"); // LOG
            char* pingval = words[1];
            char writebuf[BUFLEN];
            proto__ping(writebuf, sizeof(writebuf), pingval);
            printf("\twrite %s in socket %d\n", writebuf, sockfd); // LOG
            if (write(sockfd, writebuf, strlen(writebuf)) == -1) {
                perror("Error in writing the response to ping\n"); // LOG
                protostate = ERROR;
            }
            protostate = READ_BUFF;
            break;

        case LOGOUT:
            printf("in LOGOUT:\n"); // LOG
            if (strncmp(words[1], "out", 3) != 0) {
                fprintf(stderr, "Invalid log out command\n"); // LOG
            } else {
                char writebuf[BUFLEN];
                proto__log(writebuf, sizeof(writebuf));
                printf("\twrite %s in socket %d\n", writebuf, sockfd); // LOG
                if (write(sockfd, writebuf, strlen(writebuf)) == -1) {
                    perror("Error in writing the response to log out\n"); // LOG
                }
            }
            exited = TRUE;
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