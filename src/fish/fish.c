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
#include <string.h>

#include "figure.h"
#include "fish.h"
#include "mobility.h"
#include "vec2.h"

// ----------------------------------------------------------------------

int convert_name_to_id(const char* name)
{
    if (!name || strcmp(name, "BasicFish") == 0) {
        return BASIC_FISH_ID;
    }

    int to_return = 0;
    int s = strlen(name);
    for (int i = 0; i < s; ++i) {
        to_return += (i + 1) * name[i];
    }

    return to_return;
}

const char* get_name(const char* name)
{
    if (!name) {
        return "BasicFish";
    }

    return name;
}

// --------------------------------------------------------------------------

struct fish_t
fish__init_fish(const char* name_fish,
    struct vec2 pos, struct vec2 size,
    const char* mobility_func)
{
    int id = convert_name_to_id(name_fish);
    struct fish_t fish = {
        .name_fish = get_name(name_fish),
        .is_started = 0, // Not started by default.
        .fig = figure__init_figure(id, pos, size),
        .mobility_function_name = mobility_func
    };

    get_mobility_function_duration(mobility_func,
        &(fish.mobility_function_duration));
    get_mobility_function_target_pos(mobility_func,
        &(fish.mobility_function_target_pos));

    return fish;
}

// ----------------------------------------------------------------------

int fish__get_id(const struct fish_t fish)
{
    return figure__get_id(fish.fig);
}

const char* fish__get_name(const struct fish_t fish)
{
    return fish.name_fish;
}

const char* fish__get_mobility_func(const struct fish_t fish)
{
    return fish.mobility_function_name;
}

// ----------------------------------------------------------------------

int fish__is_started(const struct fish_t fish)
{
    return fish.is_started;
}

struct fish_t fish__start_fish(struct fish_t fish)
{
    struct fish_t new_fish = {
        .name_fish = fish.name_fish,
        .is_started = 1,
        .fig = fish.fig,
        .mobility_function_duration = fish.mobility_function_duration,
        .mobility_function_target_pos = fish.mobility_function_target_pos,
        .mobility_function_name = fish.mobility_function_name
    };

    return new_fish;
}

struct fish_t fish__stop_fish(struct fish_t fish)
{
    struct fish_t new_fish = {
        .name_fish = fish.name_fish,
        .is_started = 0,
        .fig = fish.fig,
        .mobility_function_duration = fish.mobility_function_duration,
        .mobility_function_target_pos = fish.mobility_function_target_pos,
        .mobility_function_name = fish.mobility_function_name
    };

    return new_fish;
}

// --------------------------------------------------------------------------

char* fish__disp(const struct fish_t fish, char* dst, long n)
{
    struct vec2 coord = figure__get_current_pos(fish.fig);
    struct vec2 next_coord = fish.mobility_function_target_pos();
    int duration = fish.mobility_function_duration();

    snprintf(dst, n,
        "[%s at %dx%d,%dx%d,%d]\n",
        fish.name_fish,
        coord.x, coord.y,
        next_coord.x, next_coord.y,
        duration);

    return dst;
}

char* fish__disp_without_eol(const struct fish_t fish, char* dst, long n)
{
    struct vec2 coord = figure__get_current_pos(fish.fig);
    struct vec2 next_coord = fish.mobility_function_target_pos();
    int duration = fish.mobility_function_duration();

    snprintf(dst, n,
        "[%s at %dx%d,%dx%d,%d]",
        fish.name_fish,
        coord.x, coord.y,
        next_coord.x, next_coord.y,
        duration);

    return dst;
}

// --------------------------------------------------------------------------

struct vec2 fish__get_current_pos(const struct fish_t fish)
{
    return figure__get_current_pos(fish.fig);
}

struct fish_t
fish__set_current_pos(const struct vec2 pos, const struct fish_t fish)
{
    struct fish_t new_fish = {
        .name_fish = fish.name_fish,
        .is_started = fish.is_started,
        .fig = figure__set_current_pos(pos, fish.fig),
        .mobility_function_duration = fish.mobility_function_duration,
        .mobility_function_target_pos = fish.mobility_function_target_pos,
        .mobility_function_name = fish.mobility_function_name
    };

    return new_fish;
}

// --------------------------------------------------------------------------

struct vec2 fish__get_target_pos(const struct fish_t fish)
{
    return fish.mobility_function_target_pos();
}

// --------------------------------------------------------------------------

int fish__get_move_duration(const struct fish_t fish)
{
    return fish.mobility_function_duration();
}

// --------------------------------------------------------------------------

int fish__destroy_fish(struct fish_t* ptr_fish)
{
    if (!ptr_fish) {
        return 0;
    }

    figure__destroy_figure(&(ptr_fish->fig));
    return 0;
}

// --------------------------------------------------------------------------

#ifdef DEBUG_FISH
int main(int argc, char** argv)
{
    return 0;
}
#endif // DEBUG_FISH
