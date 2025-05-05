/*! \file debug.h
 * \brief Debug utils
 */
#ifndef __DEBUG_H__
#define __DEBUG_H__
#include "stdio.h"

#ifndef UNUSED
#define UNUSED(x) (void)(x)
#endif

#ifdef USE_DEBUG
#define DBG(str, ...) \
    fprintf(stderr, "\033[31mDEBUG:\033[0m " str "\n", ##__VA_ARGS__)

#define TRACE(str, ...)                                              \
    fprintf(stderr, "\033[32mTRACE: %s \033[0m " str "\n", __FILE__, \
        ##__VA_ARGS__)
#else

#define DBG(str, ...)
#define TRACE(str, ...)

#endif // USE_DEBUG

//
#endif // !__DEBUG_H__
