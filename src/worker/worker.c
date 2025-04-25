#include "worker.h"
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

extern struct aqua_t global_aqua;

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
 * Separates words separated by spaces. The input string must end with \n.
 * Returns the number of words in the words array
 */
int worker__parse_words(char* input, char words[COMMWORDS][BUFLEN])
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
    return widx + 1;
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

struct worker__fsm_state* worker__new_fsm_state()
{
    struct worker__fsm_state* state = malloc(sizeof(struct worker__fsm_state));
    worker__init_fsm_state(state);
    return state;
}

void worker__init_fsm_state(struct worker__fsm_state* state)
{
    // State:
    state->protostate = READ_BUFF;
    // Vars:
    bzero(state->vars.buffer, sizeof(state->vars.buffer));
    bzero(state->vars.cmd, sizeof(state->vars.cmd));
    bzero(state->vars.words, sizeof(state->vars.words));
    state->vars.buff_offset = 0;
    state->vars.nwords = 0;
    state->vars.ncmds = 0;
    state->vars.nbytes_socket = 0;
    state->vars.current_vue = NULL;
}

void worker__finalize_fsm_state(struct worker__fsm_state* state)
{
    free(state);
}

int worker__clean_vars(struct worker__fsm_state* state)
{
    int halt = FALSE;
    memset(state->vars.cmd, 0, sizeof(state->vars.cmd));
    memset(state->vars.words, 0, sizeof(state->vars.words));
    state->protostate = READ_BUFF;
    return halt;
}

int worker__read_buffer(int sockfd, struct worker__fsm_state* state)
{
    printf("in READ_BUFF:\n"); // LOG
    int halt = FALSE;
    // Reads from socket only if there are no commands on the buffer
    if (state->vars.ncmds == 0) {
        state->vars.nbytes_socket = read(sockfd, state->vars.buffer + state->vars.buff_offset, BUFLEN - state->vars.buff_offset);
        state->vars.ncmds = worker__count_cmds(state->vars.buffer, state->vars.buff_offset);
        if (state->vars.nbytes_socket <= 0) {
            fprintf(stderr, "Error when reading the socket\n"); // LOG
            state->protostate = ERROR;
        } else {
            state->protostate = PARSE_BUFF;
        }
    } else {
        // There are still other commands to read on the buffer
        state->protostate = PARSE_BUFF;
    }
    return halt;
}

int worker__parse_buffer(struct worker__fsm_state* state)
{
    printf("in PARSE_BUFF:\n"); // LOG
    int halt = FALSE;
    int endcmd_offset = worker__find_cmd_in_buffer(state->vars.buffer, state->vars.buff_offset);
    if (endcmd_offset == -1) {
        state->vars.buff_offset += state->vars.nbytes_socket;
        if (state->vars.buff_offset >= BUFLEN) {
            printf("Worker buffer overflow\n"); // LOG
            state->protostate = ERROR;
        } else {
            // if the command is incomplete, the worker remains reading the
            // bytes sent to the socket
            printf("Incomplete command, going back to reading the socket\n"); // LOG
            state->protostate = READ_BUFF;
        }

    } else {
        memset(state->vars.cmd, 0, BUFLEN);
        // stores the command (max of BUFLEN bytes)
        strncpy(state->vars.cmd, state->vars.buffer, endcmd_offset + 1);
        // saves the content of the buffer located after the command
        char new_buffer[BUFLEN] = {};
        strncpy(new_buffer, state->vars.buffer + endcmd_offset + 1, BUFLEN - (endcmd_offset + 1));
        strncpy(state->vars.buffer, new_buffer, BUFLEN);
        state->vars.ncmds--; // command consumed from the buffer
        state->vars.buff_offset = 0; // new command located at the beginning of the buffer
        state->protostate = PARSE_CMD;
    }
    return halt;
}

int worker__parse_cmd(struct worker__fsm_state* state)
{
    printf("in PARSE_CMD:\n"); // LOG
    int halt = FALSE;
    state->vars.nwords = worker__parse_words(state->vars.cmd, state->vars.words);
    printf("\tnwords = %d\n", state->vars.nwords); // LOG
    state->protostate = worker__get_command_state(state->vars.words[0]);
    return halt;
}

int worker__hello(int sockfd, struct worker__fsm_state* state)
{
    int halt = FALSE;
    int id = -1;
    int nogreeting = 0;
    sscanf(state->vars.words[3], "N%d", &id);

    char writebuf[BUFLEN] = {};
    state->vars.current_vue = aqua__get_vue(id, global_aqua);
    proto__greeting(writebuf, sizeof(writebuf), id, nogreeting);

    if (write(sockfd, writebuf, strlen(writebuf)) == -1) {
        perror("Error in writing the response of hello\n"); // LOG
        halt = TRUE;
    }

    state->protostate = CLEAN_VARS;
    return halt;
}

int worker__add_fish(int sockfd, struct worker__fsm_state* state)
{
    int halt = FALSE;
    char writebuf[BUFLEN] = {};
    if (state->vars.current_vue == NULL) {
        fprintf(stderr, "%p\n", state->vars.current_vue);
        state->protostate = CLEAN_VARS;
        return halt;
    }

    struct fish_t* fishExists = aqua__get_fish(state->vars.words[1], global_aqua);
    int already_exists = (fishExists != NULL);

    // PoissonRouge at 90x40,10x4, RandomWayPoint
    int x_pos = -1, y_pos = -1;
    int width = -1, height = -1;
    struct vec2 pos, size;
    struct fish_t fish2add;

    if (!already_exists) {
        sscanf(state->vars.words[3], "%dx%d,%dx%d", &x_pos, &y_pos, &width, &height);

        pos = vec2__create(x_pos, y_pos);
        size = vec2__create(width, height);

        fish2add = fish__init_fish(state->vars.words[1], pos, size, state->vars.words[4]);
        global_aqua = aqua__add_fish(fish2add, global_aqua);
    }

    proto__add_fish(writebuf, BUFLEN, already_exists);

    if (write(sockfd, writebuf, strlen(writebuf)) == -1) {
        perror("Error in writing the response of addFish\n"); // LOG
        halt = TRUE;
    }

    state->protostate = CLEAN_VARS;
    return halt;
}

int worker__del_fish(int sockfd, struct worker__fsm_state* state)
{
    int halt = FALSE;
    char writebuf[BUFLEN] = {};
    if (state->vars.current_vue == NULL) {
        fprintf(stderr, "%p\n", state->vars.current_vue);
        state->protostate = CLEAN_VARS;
        return halt;
    }

    struct fish_t* fish2del = aqua__get_fish(state->vars.words[1], global_aqua);
    int already_exists = (fish2del != NULL);
    if (already_exists)
        global_aqua = aqua__del_fish(state->vars.words[1], global_aqua);

    proto__del_fish(writebuf, BUFLEN, already_exists);

    if (write(sockfd, writebuf, strlen(writebuf)) == -1) {
        perror("Error in writing the response to delFish\n"); // LOG
        halt = TRUE;
    }

    state->protostate = CLEAN_VARS;
    return halt;
}

int worker__start_fish(int sockfd, struct worker__fsm_state* state)
{
    int halt = FALSE;
    char writebuf[BUFLEN] = {};
    if (state->vars.current_vue == NULL) {
        fprintf(stderr, "%p\n", state->vars.current_vue);
        state->protostate = CLEAN_VARS;
        return halt;
    }

    struct fish_t* fish2start = aqua__get_fish(state->vars.words[1], global_aqua);
    int already_exists = (fish2start != NULL);
    if (already_exists)
        *fish2start = fish__start_fish(*fish2start);

    proto__start_fish(writebuf, BUFLEN, already_exists);

    if (write(sockfd, writebuf, strlen(writebuf)) == -1) {
        perror("Error in writing the response to startFish\n"); // LOG
        halt = TRUE;
    }

    state->protostate = CLEAN_VARS;
    return halt;
}

int worker__get_fishes(int sockfd, struct worker__fsm_state* state)
{
    int halt = FALSE;
    char writebuf[BUFLEN] = {};
    if (state->vars.current_vue == NULL) {
        fprintf(stderr, "%p\n", state->vars.current_vue);
        state->protostate = CLEAN_VARS;
        return halt;
    }

    struct fish_t* fishes = aqua__get_fishes(global_aqua);
    int nb_fishes = aqua__get_nb_fishes(global_aqua);

    struct vec2 origin = vue__get_current_pos(*state->vars.current_vue);

    proto__get_fishes(writebuf, BUFLEN, fishes, nb_fishes, origin);

    if (write(sockfd, writebuf, strlen(writebuf)) == -1) {
        perror("Error in writing the response to ping\n"); // LOG
        halt = TRUE;
    }

    free(fishes);

    state->protostate = CLEAN_VARS;
    return halt;
}

int worker__ping(int sockfd, struct worker__fsm_state* state)
{
    int halt = FALSE;
    char writebuf[BUFLEN] = {};
    char* pingval = state->vars.words[1];
    proto__ping(writebuf, sizeof(writebuf), pingval);
    printf("\twrite %s in socket %d\n", writebuf, sockfd); // LOG
    if (write(sockfd, writebuf, strlen(writebuf)) == -1) {
        perror("Error in writing the response to ping\n"); // LOG
        state->protostate = ERROR;
    }
    state->protostate = CLEAN_VARS;
    return halt;
}

int worker__log_out(int sockfd, struct worker__fsm_state* state)
{
    printf("in LOGOUT:\n"); // LOG
    int halt = FALSE;
    char writebuf[BUFLEN] = {};
    if (strncmp(state->vars.words[1], "out", BUFLEN) != 0) {
        fprintf(stderr, "Invalid log out command\n"); // LOG
        state->protostate = ERROR;
    } else {
        proto__log(writebuf, sizeof(writebuf));
        printf("\twrite %s in socket %d\n", writebuf, sockfd); // LOG
        if (write(sockfd, writebuf, strlen(writebuf)) == -1) {
            perror("Error in writing the response to log out\n"); // LOG
            state->protostate = ERROR;
        }
        halt = TRUE;
    }
    return halt;
}

int worker__run_fsm_step(int sockfd, struct worker__fsm_state* state)
{
    int halt = FALSE;
    switch (state->protostate) {

    case CLEAN_VARS:
        halt = worker__clean_vars(state);
        break;

    case READ_BUFF:
        halt = worker__read_buffer(sockfd, state);
        break;

    case PARSE_BUFF:
        halt = worker__parse_buffer(state);
        break;

    case PARSE_CMD:
        halt = worker__parse_cmd(state);
        break;

    case HELLO:
        halt = worker__hello(sockfd, state);
        break;

    case ADD_FISH:
        halt = worker__add_fish(sockfd, state);
        break;

    case DEL_FISH:
        halt = worker__del_fish(sockfd, state);
        break;

    case START_FISH:
        halt = worker__start_fish(sockfd, state);
        break;

    case GET_FISHES:
        halt = worker__get_fishes(sockfd, state);
        break;

    case PING:
        halt = worker__ping(sockfd, state);
        break;

    case LOGOUT:
        halt = worker__log_out(sockfd, state);
        break;

    // Executed if there was an error with the usage of the protocol,
    // which is different than an error from a libc function (read, write, etc.)
    // This may evolve in the future for error handling of the protocol
    case ERROR:
        halt = TRUE;
        break;

    default:
        fprintf(stderr, "Invalid FSM protocol state\n"); // LOG
        halt = TRUE;
    }

    return halt;
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
    struct worker__fsm_state* state = worker__new_fsm_state();

    int exited = FALSE;
    while (!exited) {
        exited = worker__run_fsm_step(sockfd, state);
    }

    // Cleaning
    close(sockfd);
    worker__finalize_fsm_state(state);

    printf("Exiting worker thread...\n"); // LOG
    return NULL;
}
