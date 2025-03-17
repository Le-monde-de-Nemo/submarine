// --------------------------------------------------------------------------
// The implementation of the FISH structure.
//      See `/src/fish/fish.h`.
//      Dependencies:
//              - `vec2.h` for the coordinates, and sizes.
//              - `figure.h` for the id, the pos and the size of a fish.
//              - `mobility.h` for the mobility functions.
//
//      The structure contains:
//          - a specie, COMMON is the by default specie.
//          - a fig, which is the id, the pos and the size of the fish.
//              See `src/figure/figure.h`
//          - mobility functions which are how the fish moves.
//              + `mobility_function_duration` gives the time needed to move.
//              + `mobility_function_target_pos` gives the next `pos`.
//
//              The call of these functions do not modify the `fish` struct.
//              To move the fish, you will need to call
//                                          `fish__set_current_pos`.
//
//              See `src/mobility/mobility.h`
//
//              You don't need to call `mobility_function_x`!
//              Use `fish__get_move_duration` and `fish__get_target_pos`.
// --------------------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>

#include "figure.h"
#include "fish.h"
#include "mobility.h"
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
fish__init_fish(enum species specie,
    int id, struct vec2 pos, struct vec2 size,
    const char* mobility_func)
{
    struct fish_t fish = {
        .specie = specie,
        .is_started = 0, // Not started by default.
        .fig = figure__init_figure(id, pos, size),
        .mobility_function_name = mobility_func
    };

    get_mobility_function_duration(mobility_func,
        fish.mobility_function_duration);
    get_mobility_function_target_pos(mobility_func,
        fish.mobility_function_target_pos);

    return fish;
}

// ----------------------------------------------------------------------

int fish__get_id(const struct fish_t* ptr_fish)
{
    // TODO
    return 0;
}

const char* fish__get_mobility_func(const struct fish_t* ptr_fish)
{
    // TODO
    return NULL;
}

// ----------------------------------------------------------------------

int fish__is_started(const struct fish_t* ptr_fish)
{
    // TODO
    return 0;
}

struct fish_t fish__start_fish(const struct fish_t* ptr_fish)
{
    // TODO
    return *ptr_fish;
}

struct fish_t fish__stop_fish(const struct fish_t* ptr_fish)
{
    // TODO
    return *ptr_fish;
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

// --------------------------------------------------------------------------

int fish__get_move_duration(const struct fish_t* fish)
{
    // TODO
    return 0;
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
