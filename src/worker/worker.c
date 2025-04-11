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
    PARSE_CMD,
    PING,
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

    if (strncmp(command, "ping", BUFLEN) == 0) {
        retval = PING;
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
    printf("Starting worker thread...\n");

    int sockfd = (int)(long)args;
    char buffer[BUFLEN] = {}; // buffer (stores the commands from byte 0)
    int buff_offset = 0; // offset for unfinished commands
    char cmd[BUFLEN] = {}; // command read from buffer
    char* words[COMMWORDS]; // words of the command

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
                perror("Error when reading the socket");
                protostate = ERROR;
            }
            int endcmd_offset = worker__find_cmd_in_buffer(buffer, buff_offset);
            if (endcmd_offset == -1) {
                buff_offset += bytes_read;
                if (buff_offset >= BUFLEN) {
                    printf("Worker buffer overflow"); // LOG
                    protostate = ERROR;
                } else {
                    printf("Incomplete command, going back to reading the socket"); // LOG
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

        case PING:
            printf("in PING:\n");
            char* pingval = words[1];
            char writebuf[BUFLEN];
            proto__ping(writebuf, sizeof(writebuf), pingval);
            printf("\twrite %s in socket %d\n", writebuf, sockfd); // LOG
            if (write(sockfd, writebuf, strlen(writebuf)) == -1) {
                perror("Error in writing the response to ping\n");
                exited = TRUE;
            }
            protostate = READ_BUFF;
            break;
        default:
            perror("Invalid FSM protocol state");
            exited = TRUE;
        }
    }

    // Cleaning
    for (int i = 0; i < COMMWORDS; i++) {
        free(words[i]);
    }

    close(sockfd);

    printf("Exiting worker thread...\n");
    return NULL;
}