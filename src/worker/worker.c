#include "proto.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define BUFLEN 512
#define COMMWORDS 64
#define TRUE 1
#define FALSE 1

enum PROTOSTATES {
    READ_COMMAND,
    PING,
    ERROR
};

// Refactoring required (change location to utils package)
int parse_argv2(char* input, char** argv)
{
    int argc = 0, k = 0;
    int in_len = strlen(input) - 1; // -1 to skip the last newline

    // Split it in argv
    for (int i = 0; i < in_len; ++i) {
        switch (input[i]) {
        case ' ':
            if (k != 0) // skip if argv[argc] is empty
            {
                k = 0;
                argc++;
            }
            break;
        default:
            argv[argc][k++] = input[i];
        }
    }

    // Add 1 to argc to take the last element in account
    return argc += (k > 0);
}

enum PROTOSTATES get_command_state(char* command)
{

    enum PROTOSTATES retval;
    const int COMM_SIZE_MAX = 64;

    if (strncmp(command, "ping", COMM_SIZE_MAX) == 0) {
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

    int sockfd = (int)(long)args;
    char buffer[BUFLEN] = {};
    char* words[COMMWORDS];

    for (int i = 0; i < COMMWORDS; i++) {
        words[i] = malloc(BUFLEN * sizeof(char));
        // strcpy(words[i], "empty");
    }

    // FSM - execution of the commands from the "affichage <-> controlleur" protocol
    int protostate = READ_COMMAND;
    int exited = FALSE;

    while (!exited) {
        if (read(sockfd, buffer, BUFLEN) == -1) { // wait until a command is received
            fprintf(stderr, "Error when reading the socket\n");
            exited = TRUE;
        }
        switch (protostate) {
        case READ_COMMAND:
            parse_argv2(buffer, words);
            int w = 0;
            // printf("in READ_COMMAND: ");
            // while (strcmp(words[w], "empty") != 0 && w < COMMWORDS) {
            //     printf("%s ", words[w++]);
            // }
            // printf("\n");
            char* command = words[0];
            protostate = get_command_state(command);
            // Error handling
            if (protostate == ERROR) {
                fprintf(stderr, "Invalid protocol command");
                exited = TRUE;
            }
            break;
        case PING:
            char* pingval = words[1];
            char writebuf[BUFLEN];
            proto__ping(writebuf, sizeof(writebuf), pingval);
            if (write(sockfd, writebuf, strlen(writebuf)) == -1) {
                fprintf(stderr, "Error in writing the response to ping");
            }
            break;
        default:
            fprintf(stderr, "Invalid FSM protocol state\n");
            exited = TRUE;
        }
    }

    // Cleaning
    for (int i = 0; i < COMMWORDS; i++) {
        free(words[i]);
    }

    close(sockfd);

    return NULL;
}