// --------------------------------------------------------------------------
// The implementation of the FISH structure.
//      See `/src/fish/fish.h`.
//      Dependencies:
//              - `vec2.h` for the coordinates, and sizes.
//              - `figure.h` for the id, the pos and the size of a fish.
//              - `mobility.h` for the mobility functions.
//
//      The structure contains:
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
#include <time.h>

#include "figure.h"
#include "fish.h"
#include "mobility.h"
#include "vec2.h"

// --------------------------------------------------------------------------

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

/* It does locally the copy of the name to store it in the struct. */
char* get_name(const char* name)
{
    char* new_name;

    if (!name) {
        new_name = (char*)malloc(MAX_SIZE_NAME_FISH * sizeof(char));
        if (!new_name) {
            fprintf(stderr, "ptr for new_name is NULL.\n");
        }

        const char name_to_put[] = "BasicFish";
        strcpy(new_name, name_to_put);
    } else {
        new_name = (char*)malloc(MAX_SIZE_NAME_FISH * sizeof(char));
        if (!new_name) {
            fprintf(stderr, "ptr for new_name is NULL.\n");
        }

        strcpy(new_name, name);
    }

    return new_name;
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
        .mob = init_mobility(mobility_func, pos)
    };

    return fish;
}

// --------------------------------------------------------------------------

int fish__get_id(const struct fish_t fish)
{
    return figure__get_id(fish.fig);
}

char* fish__get_name(const struct fish_t fish)
{
    return fish.name_fish;
}

const char* fish__get_mobility_func(const struct fish_t fish)
{
    return fish.mob.mobility_function_name;
}

// --------------------------------------------------------------------------

struct vec2 fish__get_width_height(const struct fish_t fish)
{
    return figure__get_width_height(fish.fig);
}

// --------------------------------------------------------------------------

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
        .mob = fish.mob,
    };

    return new_fish;
}

struct fish_t fish__stop_fish(struct fish_t fish)
{
    struct fish_t new_fish = {
        .name_fish = fish.name_fish,
        .is_started = 0,
        .fig = fish.fig,
        .mob = fish.mob,
    };

    return new_fish;
}

// --------------------------------------------------------------------------

char* fish__disp(const struct fish_t fish, char* dst, long n)
{
    struct vec2 size = fish__get_width_height(fish);
    struct vec2 next_coord = fish__get_target_pos(fish);
    int duration = fish__get_move_duration(fish);

    snprintf(dst, n,
        "[%s at %dx%d,%dx%d,%d]",
        fish.name_fish,
        next_coord.x, next_coord.y,
        size.x, size.y,
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
        .mob = fish.mob
    };

    return new_fish;
}

// --------------------------------------------------------------------------

struct fish_t fish__update_mobility(const struct fish_t fish)
{
    if (!fish__is_started(fish)) {
        return fish;
    }

    time_t current_timestamp = mobility_get_timestamp();
    struct fish_t new_fish = fish__set_current_pos(
        fish__get_target_pos(fish),
        fish);
    new_fish.mob.last_timestamp = current_timestamp;

    // <is the last mobility finished?>
    time_t current_mob_duration = current_timestamp - fish.mob.last_timestamp;
    if (current_mob_duration < fish__get_move_duration(fish)) {
        return new_fish;
    } // </is the last mobility finished?>

    // Now the fish reached his target, because move duration is over.
    // Then, give another goal to the fish.
    new_fish.mob.last_coordinates = fish__get_current_pos(fish);
    new_fish.mob.duration_to_move = fish.mob.mobility_function_duration(fish.mob);
    new_fish.mob.next_coordinates = fish.mob.mobility_function_target_pos(fish.mob);

    return new_fish;
}

// --------------------------------------------------------------------------

struct vec2 fish__get_target_pos(const struct fish_t fish)
{
    return fish.mob.next_coordinates;
}

// --------------------------------------------------------------------------

int fish__get_move_duration(const struct fish_t fish)
{
    return fish.mob.duration_to_move;
}

// --------------------------------------------------------------------------

int fish__destroy_fish(struct fish_t* ptr_fish)
{
    if (!ptr_fish) {
        return 0;
    }

    if (ptr_fish->name_fish) {
        free(ptr_fish->name_fish);
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
