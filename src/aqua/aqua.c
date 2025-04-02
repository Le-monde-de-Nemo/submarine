// --------------------------------------------------------------------------
// The implementation of the aqua(rium) structure.
//      See `/src/aqua/aqua.h`.
//      Dependencies:
//              - `vec2.h` for the coordinates, and sizes.
//              - `figure.h` for the id, the pos and the size of a fish/vue.
//              - `fish.h` to manipulate a fish.
//              - `vue.h` to manipulate a vue.
//
//      The structure contains:
//          - a fig, which is the id, the coord and the size of the aqua.
//          - a single list of fishes, using `<sys/queue.h>`.
//          - a single list of views, using `<sys/queue.h>`.
//              See `src/figure/figure.h`
// --------------------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <sys/queue.h>

#include "aqua.h"
#include "figure.h"
#include "fish.h"
#include "vec2.h"
#include "vue.h"

// --------------------------------------------------------------------------

struct aqua_t aqua__init_aqua(struct vec2 size)
{
    struct aqua_t aqua = {
        .fig = figure__init_figure(0, vec2__zeros(), size),
        .nb_vues = 0,
        .nb_fishes = 0
    };

    SLIST_INIT(&(aqua.list_vues));
    SLIST_INIT(&(aqua.list_fishes));

    return aqua;
}

// --------------------------------------------------------------------------

int aqua__get_id(const struct aqua_t aqua)
{
    return figure__get_id(aqua.fig);
}

// --------------------------------------------------------------------------

struct aqua_t
aqua__add_fish(struct fish_t fish, const struct aqua_t aqua)
{
    struct slisthead_fish new_head = aqua.list_fishes;
    struct aqua__entry_fish_t* n2 = malloc(sizeof(struct aqua__entry_fish_t));
    SLIST_INSERT_HEAD(&new_head, n2, entries);

    struct aqua_t new_aqua = {
        .fig = aqua.fig,
        .list_vues = aqua.list_vues,
        .nb_vues = aqua.nb_vues,
        .list_fishes = new_head,
        .nb_fishes = aqua.nb_fishes + 1
    };

    return new_aqua;
}

struct aqua_t
aqua__del_fish(int id_fish, const struct aqua_t aqua)
{
    // TODO
    return aqua;
}

struct fish_t
aqua__get_fish(int id_fish, const struct aqua_t aqua)
{
    // TODO
    struct fish_t fish = fish__init_fish(
        COMMON,
        0, vec2__zeros(), vec2__ones(),
        NULL);

    return fish;
}

struct fish_t*
aqua__get_fishes(const struct aqua_t aqua)
{
    // TODO
    return NULL;
}

int aqua__get_nb_fishes(const struct aqua_t aqua)
{
    return aqua.nb_fishes;
}

// --------------------------------------------------------------------------

struct aqua_t
aqua__add_vue(struct vue_t vue, const struct aqua_t aqua)
{
    struct slisthead_vue new_head = aqua.list_vues;
    struct aqua__entry_vue_t* n1 = malloc(sizeof(struct aqua__entry_vue_t));
    SLIST_INSERT_HEAD(&new_head, n1, entries);

    struct aqua_t new_aqua = {
        .fig = aqua.fig,
        .list_vues = new_head,
        .nb_vues = aqua.nb_vues + 1,
        .list_fishes = aqua.list_fishes,
        .nb_fishes = aqua.nb_fishes
    };

    return new_aqua;
}

struct aqua_t
aqua__del_vue(int id_vue, const struct aqua_t aqua)
{
    return aqua;
}

struct vue_t
aqua__get_vue(int id_vue, const struct aqua_t aqua)
{
    // TODO
    struct vue_t vue = vue__init_vue(0, vec2__zeros(), vec2__ones());
    return vue;
}

struct vue_t*
aqua__get_vues(const struct aqua_t aqua)
{
    // TODO
    return NULL;
}

int aqua__get_nb_vues(const struct aqua_t aqua)
{
    return aqua.nb_vues;
}

// --------------------------------------------------------------------------

int aqua__destroy_aqua(struct aqua_t* ptr_aqua)
{
    if (!ptr_aqua) {
        return 1;
    }

    // Free the list of vues.
    struct aqua__entry_vue_t* n1;
    struct slisthead_vue head_vue = ptr_aqua->list_vues;
    while (!SLIST_EMPTY(&head_vue)) {
        n1 = SLIST_FIRST(&head_vue);
        SLIST_REMOVE_HEAD(&head_vue, entries);
        free(n1);
        ptr_aqua->nb_vues -= 1;
    }

    // Free the list of fishes.
    struct aqua__entry_fish_t* n2;
    struct slisthead_fish head_fish = ptr_aqua->list_fishes;
    while (!SLIST_EMPTY(&head_fish)) {
        n2 = SLIST_FIRST(&head_fish);
        SLIST_REMOVE_HEAD(&head_fish, entries);
        free(n2);
        ptr_aqua->nb_fishes -= 1;
    }

    return 1;
}

// ----------------------------------------------------------------------
