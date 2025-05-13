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

char* proto__get_fishes(char* dst, long n, struct fish_t* fishes, int n_fishes, struct vue_t* vue)
{
    char acc[n];
    char fish_buffer[n];
    acc[0] = '\0';  // Initialiser la chaîne vide

    strcpy(acc, "list ");
    long n_acc = strlen(acc);

    struct vec2 aqua_dim = aqua__get_width_height(store.global_aqua);
    struct vec2 vue_pos = vue__get_current_pos(*vue);
    struct vec2 vue_dim = vue__get_width_height(*vue);

    DBG("Aqua dim: %d %d", aqua_dim.x, aqua_dim.y);
    DBG("Vue pos : %d %d", vue_pos.x, vue_pos.y);
    DBG("Vue dim : %d %d", vue_dim.x, vue_dim.y);

    for (int i = 0; i < n_fishes; ++i) {
        struct fish_t fish = fishes[i];

        // Pos globale en pixels dans l'aquarium
        int global_x = (fish__get_current_pos(fish).x * aqua_dim.x) / 100;
        int global_y = (fish__get_current_pos(fish).y * aqua_dim.y) / 100;

        // Pos relative dans la vue (toujours en pixels, vue locale)
        int x_vue = (global_x - vue_pos.x) * 100 / vue_dim.x;
        int y_vue = (global_y - vue_pos.y) * 100 / vue_dim.y;

        int len = snprintf(fish_buffer, n, "[%s at %dx%d,%dx%d,%d] ",
            fish__get_name(fish),
            x_vue, y_vue,
            fish__get_width_height(fish).x,
            fish__get_width_height(fish).y,
            fish__get_move_duration(fish));

        if (n_acc + len < n - 1) {
            strcat(acc, fish_buffer);
            n_acc += len;
        }
    }

    strcat(acc, "\n");
    strncpy(dst, acc, n);
    dst[n - 1] = '\0';  // Sécurité

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
