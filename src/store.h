#ifndef _STORE_H_
#define _STORE_H_
#include "aqua.h"
#include "controller.h"
extern struct store_t store;

struct store_t {
    struct controller_t controller;
    struct aqua_t global_aqua;
    int exited;
};
#endif // _STORE_H_
