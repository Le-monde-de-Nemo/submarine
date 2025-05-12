#ifndef WORKER_H
#define WORKER_H

/**
 * Provides the worker function used for interpreting the
 * commands sent from the viewer to the controller
 */

#define BUFLEN 4096
#define COMMWORDS 64
#define TRUE 1
#define FALSE 0

enum PROTOSTATES {
    CLEAN_VARS,
    READ_BUFF,
    PARSE_BUFF,
    PARSE_CMD,
    PING,
    HELLO, /* hello */
    GET_FISHES, /* getFishes */
    GET_FISHES_CONTINUOUSLY, /* getFishesContinuously */
    LS,  /* ls */
    ADD_FISH, /* addFish */
    DEL_FISH, /* delFish */
    START_FISH, /* startFish */
    LOGOUT,
    ERROR
};

struct worker__fsm_vars {
    char buffer[BUFLEN]; // buffer (stores the commands from byte 0)
    char cmd[BUFLEN]; // command read from buffer
    char words[COMMWORDS][BUFLEN]; // words of the command
    int buff_offset; // offset for unfinished commands
    int nwords; // number of words in the words array
    int ncmds; // number of commands stored in the buffer
    int nbytes_socket; // number of bytes read on the last call of read(sockfd)
    struct vue_t* current_vue;
};

struct worker__fsm_state {
    enum PROTOSTATES protostate;
    struct worker__fsm_vars vars;
};

// FSM constructor
struct worker__fsm_state* worker__new_fsm_state();

// FSM initializer
void worker__init_fsm_state(struct worker__fsm_state* state);

// FSM destructor
void worker__finalize_fsm_state(struct worker__fsm_state* state);

// Executes a step/transition of the FSM. Returns if the FSM should halt
int worker__run_fsm_step(int sockfd, struct worker__fsm_state* state);

/*
 * :param args: an integer, it is a file descriptor.
 * `worker` will write in that file descriptor.
 * :return: nothing.
 */
void* worker(void* args);

#endif