// --------------------------------------------------------------------------
// The implementation of the FISH structure.
//      See `/src/fish/fish.h`.
//      Dependencies:
//              - `vec2.h` for the coordinates, and sizes.
//              - `figure.h` for the id, the pos and the size of a fish.
//
//      The structure contains:
//          - a specie, COMMON is the by default specie.
//          - a fig, which is the id, the pos and the size of the this.
//              See `src/figure/figure.h`
//          - a mobility function which is how the fish moves.
//              See `src/mobility/mobility.h`
// --------------------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>

#include "fish.h"
#include "figure.h"
#include "vec2.h"

// ----------------------------------------------------------------------

// static char* species_name[] = {
//"PoissonRouge",
//"PoissonClown",
//};

char* specie__disp(enum species specie)
{
    if (specie == COMMON) {
        return "BasicFish";
    }

    // TODO
    return NULL;
}

// --------------------------------------------------------------------------

struct fish_t
fish__init_fish(int id, int is_started, enum species specie,
                        struct vec2 pos, struct vec2 size)
{
    struct fish_t fish = {
        .specie = specie,
        .is_started = is_started,
        .fig = figure__init_figure(id, pos, size),
        //.mobility_function = NULL
    };

    return fish;
}

// --------------------------------------------------------------------------

enum species fish__get_type(const struct fish_t* fish)
{
    // TODO
    return COMMON;
}

struct fish_t* fish__set_type(const enum species specie, struct fish_t* fish)
{
    // TODO
    return fish;
}

// --------------------------------------------------------------------------

struct vec2 fish__get_current_pos(const struct fish_t* fish)
{
    // TODO
    return vec2__zeros();
}

struct fish_t* fish__set_current_pos(const struct vec2 pos, struct fish_t* fish)
{
    // TODO
    return fish;
}

// --------------------------------------------------------------------------

struct vec2 fish__get_target_pos(const struct fish_t* fish)
{
    // TODO
    return vec2__zeros();
}

struct fish_t*
fish__set_target_pos(const struct vec2 target_pos, struct fish_t* fish)
{
    // TODO
    return fish;
}

// --------------------------------------------------------------------------

int fish__get_move_duration(const struct fish_t* fish)
{
    // TODO
    return 0;
}

struct fish_t*
fish__set_move_duration(int time_duration, struct fish_t* fish)
{
    // TODO
    return fish;
}

// --------------------------------------------------------------------------

int fish__destroy_fish(struct fish_t* ptr_fish)
{
    // TODO
    return 0;
}

// --------------------------------------------------------------------------

#ifdef DEBUG_FISH
int main(int argc, char** argv)
{
    return 0;
}
#endif // DEBUG_FISH
