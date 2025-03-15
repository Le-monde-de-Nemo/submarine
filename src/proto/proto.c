#include "proto.h"
#include "fish.h"
#include <stdio.h>
#include <string.h>

char* proto__greeting(char* dst, long n, char* id, int nogreeting)
{
    if (nogreeting)
        snprintf(dst, n, "no greeting\n");
    else
        snprintf(dst, n, "greeting %s\n", id);
    return dst;
}

char* proto__get_fishes(char* dst, long n, struct Fish* fishes, int n_fishes)
{
    char acc[n];
    char fish_buffer[n];
    for (int i = 0; i < n; ++i)
        acc[i] = '\0';

    strcpy(acc, "list ");
    long n_acc = sizeof("list ");

    for (int i = 0; i < n_fishes; ++i) {
        struct Fish fish = fishes[i];
        int len = snprintf(fish_buffer, n, "[%s at %dx%d,%dx%d,%d] ",
            specie__disp(fish.specie),
            fish__getPos(&fish).x,
            fish__getPos(&fish).y,
            fish__getTargetPos(&fish).x,
            fish__getTargetPos(&fish).y,
            fish__getMoveDuration(&fish));

        if (n_acc + len < n) {
            strcat(acc, fish_buffer);
            n_acc += len;
        }
    }

    strcpy(dst, acc);
    return dst;
}

char* proto__log(char* dst, long n)
{
    snprintf(dst, n, "bye\n");
    return dst;
}

char* proto__ping(char* dst, long n, char* msg)
{
    snprintf(dst, n, "pong %s\n", msg);
    return dst;
}

char* proto__add_fish(char* dst, long n, int already_exists)
{
    if (already_exists)
        snprintf(dst, n, "NOK\n");
    else
        snprintf(dst, n, "OK\n");
    return dst;
}

char* proto__del_fish(char* dst, long n, int already_exists)
{
    if (already_exists)
        snprintf(dst, n, "OK\n");
    else
        snprintf(dst, n, "NOK\n");
    return dst;
}

char* proto__start_fish(char* dst, long n, int already_exists)
{
    if (already_exists)
        snprintf(dst, n, "OK\n");
    else
        snprintf(dst, n, "NOK\n");
    return dst;
}
