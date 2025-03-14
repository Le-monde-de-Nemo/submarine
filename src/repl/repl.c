#include "repl.h"
#include <stdio.h>
#include <string.h>

// Should be plenty of space for what's needed
#define SSCANF_FORMAT_SIZE 128

int repl__run()
{
  // continue boolean
  int c = 1;

  char sscanf_format[SSCANF_FORMAT_SIZE];

  char input[REPL_BUF_SIZE] = {};
  char argv[REPL_BUF_SIZE][REPL_BUF_SIZE] = {};
  int argc = 0;
  int k = 0;  // Index of argv[argc]
  int in_len;

  while (c)
  {
    // Reset all argv
    for (int i = 0 ; i < argc ; ++i)
    {
      k = 0;
      // Make sure doesn't overflow if all elements are not 0
      while (argv[i][k] != 0 && k < REPL_BUF_SIZE)
        argv[i][k++] = '\0';
    }

    // Get raw input
    argc = 0;
    k = 0;
    char* res = fgets(input, REPL_BUF_SIZE, stdin);
    if (res == NULL)
    {
      fprintf(stderr, "Found EOF while reading input, exiting...\n");
      return EOF;
    }

    in_len = strlen(input) - 1;  // -1 to skip the last newline

    // Split it in argv
    for (int i = 0 ; i < in_len ; ++i)
    {
      switch (input[i]) {
        case ' ':
          if (k != 0)  // skip if argv[argc] is empty
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
    argc += (k > 0);

    if (strcmp("show", argv[0]) == 0)
    {
    }
    else if (strcmp("load", argv[0]) == 0)
    {
    }
    else if (strcmp("save", argv[0]) == 0)
    {
    }
    else if (argc > 1 && strcmp("add", argv[0]) == 0 && strcmp("view", argv[1]) == 0)
    {
    }
    else if (argc > 1 && strcmp("del", argv[0]) == 0 && strcmp("view", argv[1]) == 0)
    {
    }
    else
    {
      printf("NOK NOK -- GITGUD\n");
    }
  }

  return 0;
}
