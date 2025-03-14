#ifndef _REPL_H_
#define _REPL_H_
#include <stddef.h>

#define REPL_BUF_SIZE 2048

struct repl_entry {
    // Returns non-zero if the entry match argv, zero otherwize
    int (*match)(int argc, char* argv[]);
    void (*exec)(int argc, char* argv[]);
};

struct repl {
    struct repl_entry* repls;
    size_t replc;
};

/*
 *  Create an reple using the an array of repl_entries
 */
struct repl repl__create(struct repl_entry* repl_entries, size_t n);

/*
 *  Run the repl
 */
int repl__run(struct repl repl);

/*
 *  Call to free the repl
 */
void repl__finalize(struct repl repl);
#endif
