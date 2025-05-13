#include "proto.h"
#include "aqua.h"
#include "debug.h"
#include "fish.h"
#include "store.h"
#include "vec2.h"
#include "vue.h"
#include <stdio.h>
#include <string.h>

char* proto__greeting(char* dst, long n, int id, int nogreeting)
{
    if (nogreeting)
        snprintf(dst, n, "no greeting\n");

    else {
        struct vue_t* vue = aqua__get_vue(id, store.global_aqua);
        DBG("%p", vue);
        char vuebuf[2048] = {};
        vue__disp(*vue, vuebuf, sizeof(vuebuf));
        snprintf(dst, n, "greeting %s", vuebuf);
    }

    return dst;
}

char* proto__get_fishes(char* dst, long n, struct fish_t* fishes, int n_fishes, struct vec2 origin)
{
    char acc[n] = {};
    char fish_buffer[n];
    for (int i = 0; i < n; ++i)
        acc[i] = '\0';

    strcpy(acc, "list ");
    long n_acc = sizeof("list ");

    for (int i = 0; i < n_fishes; ++i) {
        struct fish_t fish = fishes[i];
        int len = snprintf(fish_buffer, n, "[%s at %dx%d,%dx%d,%d] ",
            fish__get_name(fish),
            fish__get_current_pos(fish).x - origin.x,
            fish__get_current_pos(fish).y - origin.y,
            fish__get_width_height(fish).x,
            fish__get_width_height(fish).y,
            fish__get_move_duration(fish));

        if (n_acc + len < n - 1) {
            strcat(acc, fish_buffer);
            n_acc += len;
        }
    }
    strcat(acc, "\n");
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
