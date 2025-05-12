#include "worker.h"
#include "aqua.h"
#include "debug.h"
#include "fish.h"
#include "proto.h"
#include "store.h"
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
    } else if (strncmp(command, "getFishesContinuously", BUFLEN) == 0) {
        retval = GET_FISHES_CONTINUOUSLY;
    } else if (strncmp(command, "ls", BUFLEN) == 0) {
        retval = LS;
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
    TRACE("in READ_BUFF:"); // LOG
    int halt = FALSE;
    // Reads from socket only if there are no commands on the buffer
    if (state->vars.ncmds == 0) {
        state->vars.nbytes_socket = read(sockfd, state->vars.buffer + state->vars.buff_offset, BUFLEN - state->vars.buff_offset);
        state->vars.ncmds = worker__count_cmds(state->vars.buffer, state->vars.buff_offset);

        if (state->vars.nbytes_socket <= 0) {
            TRACE("Error when reading the socket (or connection closed)"); // LOG
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
    TRACE("in PARSE_BUFF:"); // LOG
    int halt = FALSE;
    int endcmd_offset = worker__find_cmd_in_buffer(state->vars.buffer, state->vars.buff_offset);
    if (endcmd_offset == -1) {
        state->vars.buff_offset += state->vars.nbytes_socket;
        if (state->vars.buff_offset >= BUFLEN) {
            TRACE("Worker buffer overflow"); // LOG
            state->protostate = ERROR;
        } else {
            // if the command is incomplete, the worker remains reading the
            // bytes sent to the socket
            TRACE("Incomplete command, going back to reading the socket"); // LOG
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
    TRACE("in PARSE_CMD:"); // LOG
    int halt = FALSE;
    state->vars.nwords = worker__parse_words(state->vars.cmd, state->vars.words);
    TRACE("\tnwords = %d", state->vars.nwords); // LOG
    state->protostate = worker__get_command_state(state->vars.words[0]);
    return halt;
}

int worker__hello(int sockfd, struct worker__fsm_state* state)
{
    int halt = FALSE;
    int id = -1;
    int nogreeting = 0;
    char writebuf[BUFLEN] = {};

    switch(state->vars.nwords) {
    case 1:  // gets the first available vue id
        if(store.global_aqua.list_vues.slh_first != NULL) {
            id = store.global_aqua.list_vues.slh_first->data->fig.id;
        } else {
            TRACE("Empty vue, returning nothing in hello");
            nogreeting = 1;
            proto__greeting(writebuf, sizeof(writebuf), id, nogreeting);
            if (write(sockfd, writebuf, strlen(writebuf)) == -1) {
                perror("Error in writing the response of hello"); // LOG
                halt = TRUE;
            }
            state->protostate = CLEAN_VARS;
            return halt;
        }
    break;
    case 4:  // hello in as [id]
        sscanf(state->vars.words[3], "N%d", &id);
    break;
    default:
        TRACE("Invalid call to hello");
        strncpy(writebuf, "usage: hello [in as N$ID]\n", BUFLEN);
        if (write(sockfd, writebuf, strlen(writebuf)) == -1) {
            perror("Error in writing the response of hello"); // LOG
            halt = TRUE;
        }
        state->protostate = CLEAN_VARS;
        return halt;
    break;
    }

    state->vars.current_vue = aqua__get_vue(id, store.global_aqua);
    if(state->vars.current_vue == NULL) {
        if(store.global_aqua.list_vues.slh_first != NULL) {
            id = store.global_aqua.list_vues.slh_first->data->fig.id;
        } else {
            TRACE("Empty vue, returning nothing in hello");
            nogreeting = 1;
            proto__greeting(writebuf, sizeof(writebuf), id, nogreeting);
            if (write(sockfd, writebuf, strlen(writebuf)) == -1) {
                perror("Error in writing the response of hello"); // LOG
                halt = TRUE;
            }
            state->protostate = CLEAN_VARS;
            return halt;
        }
    }
    proto__greeting(writebuf, sizeof(writebuf), id, nogreeting);

    if (write(sockfd, writebuf, strlen(writebuf)) == -1) {
        perror("Error in writing the response of hello"); // LOG
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
        TRACE("%p", state->vars.current_vue);
        state->protostate = CLEAN_VARS;
        return halt;
    }

    struct fish_t* fishExists = aqua__get_fish(state->vars.words[1], store.global_aqua);
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
        store.global_aqua = aqua__add_fish(fish2add, store.global_aqua);
    }

    proto__add_fish(writebuf, BUFLEN, already_exists);

    if (write(sockfd, writebuf, strlen(writebuf)) == -1) {
        perror("Error in writing the response of addFish"); // LOG
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
        TRACE("%p", state->vars.current_vue);
        state->protostate = CLEAN_VARS;
        return halt;
    }

    struct fish_t* fish2del = aqua__get_fish(state->vars.words[1], store.global_aqua);
    int already_exists = (fish2del != NULL);
    if (already_exists)
        store.global_aqua = aqua__del_fish(state->vars.words[1], store.global_aqua);

    proto__del_fish(writebuf, BUFLEN, already_exists);

    if (write(sockfd, writebuf, strlen(writebuf)) == -1) {
        perror("Error in writing the response to delFish"); // LOG
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
        TRACE("%p", state->vars.current_vue);
        state->protostate = CLEAN_VARS;
        return halt;
    }

    struct fish_t* fish2start = aqua__get_fish(state->vars.words[1], store.global_aqua);
    int already_exists = (fish2start != NULL);
    if (already_exists)
        *fish2start = fish__start_fish(*fish2start);

    proto__start_fish(writebuf, BUFLEN, already_exists);

    if (write(sockfd, writebuf, strlen(writebuf)) == -1) {
        perror("Error in writing the response to startFish"); // LOG
        halt = TRUE;
    }

    state->protostate = CLEAN_VARS;
    return halt;
}

int get_fishes(int sockfd, struct worker__fsm_state* state)
{
    int halt = FALSE;
    char writebuf[BUFLEN] = {};
    if (state->vars.current_vue == NULL) {
        TRACE("%p", state->vars.current_vue);
        state->protostate = CLEAN_VARS;
        return halt;
    }

    struct fish_t* fishes = aqua__get_fishes(store.global_aqua);
    int nb_fishes = aqua__get_nb_fishes(store.global_aqua);

    struct vec2 origin = vue__get_current_pos(*state->vars.current_vue);

    proto__get_fishes(writebuf, BUFLEN, fishes, nb_fishes, origin);

    if (write(sockfd, writebuf, strlen(writebuf)) == -1) {
        perror("Error in writing the response to getFishes"); // LOG
        halt = TRUE;
    }

    free(fishes);
    return halt;
}

int worker__get_fishes(int sockfd, struct worker__fsm_state* state)
{
    int halt = get_fishes(sockfd, state);
    state->protostate = CLEAN_VARS;
    return halt;
}

int worker__get_fishes_continuously(int sockfd, struct worker__fsm_state* state)
{
    int halt = get_fishes(sockfd, state);
    sleep(3);
    state->protostate = GET_FISHES_CONTINUOUSLY;
    return halt;
}

// Currently, the ls command works only with a given number of steps
int worker__ls(int sockfd, struct worker__fsm_state* state)
{
    int halt = FALSE;
    for(int i = 0; i < atoi(state->vars.words[1]); i++) {
        halt = get_fishes(sockfd, state);
        if(halt) break;
    }

    state->protostate = CLEAN_VARS;
    return halt;
}

int worker__ping(int sockfd, struct worker__fsm_state* state)
{
    int halt = FALSE;
    char writebuf[BUFLEN] = {};
    char* pingval = state->vars.words[1];
    proto__ping(writebuf, sizeof(writebuf), pingval);
    TRACE("\twrite %s in socket %d", writebuf, sockfd); // LOG
    if (write(sockfd, writebuf, strlen(writebuf)) == -1) {
        perror("Error in writing the response to ping"); // LOG
        state->protostate = ERROR;
    }
    state->protostate = CLEAN_VARS;
    return halt;
}

int worker__log_out(int sockfd, struct worker__fsm_state* state)
{
    TRACE("in LOGOUT:"); // LOG
    int halt = FALSE;
    char writebuf[BUFLEN] = {};

    if (strncmp(state->vars.words[1], "out", BUFLEN) != 0) {
        TRACE("Invalid log out command"); // LOG
        state->protostate = ERROR;
    } else {
        proto__log(writebuf, sizeof(writebuf));
        TRACE("\twrite %s in socket %d", writebuf, sockfd); // LOG
        if (write(sockfd, writebuf, strlen(writebuf)) == -1) {
            perror("Error in writing the response to log out"); // LOG
            state->protostate = ERROR;
        }
        halt = TRUE;
    }

    close(sockfd);

    return halt;
}

int worker__run_fsm_step(int sockfd, struct worker__fsm_state* state)
{
    int halt = FALSE;
    TRACE("%d", state->protostate);
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

    case GET_FISHES_CONTINUOUSLY:
        halt = worker__get_fishes_continuously(sockfd, state);
        break;

    case LS:
        halt = worker__ls(sockfd, state);
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
        TRACE("Invalid FSM protocol state"); // LOG
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
    TRACE("Starting worker thread..."); // LOG

    int sockfd = (int)(long)args;
    struct worker__fsm_state* state = worker__new_fsm_state();

    int exited = FALSE;
    while (!exited) {
        exited = worker__run_fsm_step(sockfd, state);
    }

    // Cleaning
    close(sockfd);
    worker__finalize_fsm_state(state);

    TRACE("Exiting worker thread..."); // LOG
    return NULL;
}
