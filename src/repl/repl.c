#include "repl.h"
#include <assert.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Should be plenty of space for what's needed
#define SSCANF_FORMAT_SIZE 128

struct repl repl__create(struct repl_entry* repl_entries, size_t n)
{
    struct repl repl = {
        .repls = malloc(sizeof(struct repl_entry) * n),
        .replc = n
    };

    for (int i = 0; i < n; ++i)
        repl.repls[i] = repl_entries[i];

    return repl;
}

void reset_argv(int argc, char** argv)
{
    int k;

    for (int i = 0; i < argc; ++i) {
        k = 0;
        // Make sure doesn't overflow if all elements are not 0
        while (argv[i][k] != '\0' && k < REPL_BUF_SIZE)
            argv[i][k++] = '\0';
    }
}

int parse_argv(char* input, char** argv)
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

void free_argv(char** argv)
{
    for (int i = 0; i < REPL_BUF_SIZE; ++i)
        free(argv[i]);
}

int repl__run(struct repl repl)
{
    // is set to 0 on exit
    int c = 1;

    char sscanf_format[SSCANF_FORMAT_SIZE];

    char input[REPL_BUF_SIZE] = {};
    char* argv[REPL_BUF_SIZE] = {};
    for (int i = 0; i < REPL_BUF_SIZE; ++i) {
        // Use calloc to set 0 everywhere
        argv[i] = calloc(REPL_BUF_SIZE, sizeof(char));
        assert(argv[i] != NULL);
    }

    int argc = 0;
    int k = 0; // Index to iterate in argv[argc]
    int in_len;

    while (c) {
        reset_argv(argc, argv);

        // Get raw input
        printf("> ");
        char* res = fgets(input, REPL_BUF_SIZE, stdin);
        if (res == NULL) {
            fprintf(stderr, "Found EOF while reading input, exiting...\n");
            free_argv(argv);
            return EOF;
        }

        argc = parse_argv(input, argv);

        int matched = 0;
        for (int i = 0; i < repl.replc; ++i) {
            struct repl_entry entry = repl.repls[i];

            if (entry.match(argc, argv)) {
                matched = 1;
                entry.exec(argc, argv);
                break;
            }
        }

        if (!matched)
            printf("NOK NOK -- GITGUD\n");
    }

    free_argv(argv);
    return 0;
}

void repl__finalize(struct repl repl)
{
    free(repl.repls);
}
