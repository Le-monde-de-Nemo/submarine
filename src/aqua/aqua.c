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
//          - a list of fishes, using `src/utils/list.h`.
//          - a list of views, using `src/utils/list.h`.
//              See `src/figure/figure.h`
// --------------------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>

#include "aqua.h"
#include "figure.h"
#include "fish.h"
#include "vec2.h"
#include "vue.h"

// --------------------------------------------------------------------------

struct aqua_t aqua__init_aqua(struct vec2 size)
{
    struct aqua_t aqua = {
        .fig = figure__init_figure(0, vec2__zeros(), size)
    };

    return aqua;
}

// --------------------------------------------------------------------------

int aqua__get_id(const struct aqua_t* ptr_aqua)
{
    // TODO
    return 0;
}

// --------------------------------------------------------------------------

struct aqua_t
aqua__add_fish(struct fish_t* ptr_fish, const struct aqua_t* ptr_aqua)
{
    // TODO
    return *ptr_aqua;
}

struct aqua_t
aqua__del_fish(int id_fish, const struct aqua_t* ptr_aqua)
{
    // TODO
    return *ptr_aqua;
}

struct fish_t
aqua__get_fish(int id_fish, const struct aqua_t* ptr_aqua)
{
    // TODO
    struct fish_t fish = fish__init_fish(
        COMMON,
        0, vec2__zeros(), vec2__ones(),
        NULL);

    return fish;
}

struct fish_t*
aqua__get_fishes(const struct aqua_t* ptr_aqua)
{
    // TODO
    return NULL;
}

// --------------------------------------------------------------------------

struct aqua_t
aqua__add_vue(struct vue_t* ptr_vue, const struct aqua_t* ptr_aqua)
{
    return *ptr_aqua;
}

struct aqua_t
aqua__del_vue(int id_vue, const struct aqua_t* ptr_aqua)
{
    return *ptr_aqua;
}

struct vue_t
aqua__get_vue(int id_vue, const struct aqua_t* ptr_aqua)
{
    // TODO
    struct vue_t vue = vue__init_vue(0, vec2__zeros(), vec2__ones());
    return vue;
}

struct vue_t*
aqua__get_vues(const struct aqua_t* ptr_aqua)
{
    // TODO
    return NULL;
}

// --------------------------------------------------------------------------

int aqua__destroy_aqua(struct aqua_t* ptr_aqua)
{
    // TODO
    return 1;
}

// ----------------------------------------------------------------------
