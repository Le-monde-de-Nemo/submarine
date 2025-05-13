#ifndef _PROTO_H_
#define _PROTO_H_
// Functions to generate potocol responses, it doens't not more than creating
// the strings.
//
// If some processing or sanitizing needs to be done, it must be done before
// using these functions
//
// All of these functions returns dst
#include "fish.h"
#include "vue.h"

char* proto__greeting(char* dst, long n, int id, int nogreeting);

char* proto__get_fishes(char* dst, long n, struct fish_t* fishes, int n_fishes, struct vue_t* vue);

char* proto__log(char* dst, long n);

char* proto__ping(char* dst, long n, char* msg);

char* proto__add_fish(char* dst, long n, int already_exists);

char* proto__del_fish(char* dst, long n, int already_exists);

char* proto__start_fish(char* dst, long n, int already_exists);
#endif
