// --------------------------------------------------------------------------
// The implementation of the mobility functions.
//      See `/src/mobility/mobility.h`.
//      Dependencies:
//              - `vec2.h` for the coordinates, and sizes.
// --------------------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "mobility.h"
#include "vec2.h"

// --------------------------------------------------------------------------
// Those arrays are needed to find the mobility function given a name.
// --------------------------------------------------------------------------

/* The names in that array need to be different! */
static const char* array_mobility_function_names[NUM_MOBILITY_FUNCTIONS] = {
    "RandomWayPoint",
    "DiagonalWayPoint"
};

static int (*array_mobility_function_duration[NUM_MOBILITY_FUNCTIONS])(const struct mobility_t) = {
    random_way_point_duration,
    diagonal_way_point_duration
};

static struct vec2 (*array_mobility_function_target_pos[NUM_MOBILITY_FUNCTIONS])(const struct mobility_t) = {
    random_way_point_target_pos,
    diagonal_way_point_target_pos
};

// --------------------------------------------------------------------------

time_t mobility_get_timestamp()
{
    return time(NULL);
}

// --------------------------------------------------------------------------

void get_mobility_function_duration(const char* name, int (**ptr_func)(const struct mobility_t))
{
    // ind could be for instance RANDOM_WAY_POINT or DIRECT_WAY_POINT.
    for (int ind = 0; ind < NUM_MOBILITY_FUNCTIONS; ++ind) {
        if (strcmp(name, array_mobility_function_names[ind]) == 0) {
            *ptr_func = array_mobility_function_duration[ind];
            return;
        }
    }

    // If the name does not exist, take "RandomWayPoint".
    *ptr_func = array_mobility_function_duration[RANDOM_WAY_POINT];
}

void get_mobility_function_target_pos(const char* name,
    struct vec2 (**ptr_func)(const struct mobility_t mob))
{
    // ind could be for instance RANDOM_WAY_POINT or DIRECT_WAY_POINT.
    for (int ind = 0; ind < NUM_MOBILITY_FUNCTIONS; ++ind) {
        if (strcmp(name, array_mobility_function_names[ind]) == 0) {
            *ptr_func = array_mobility_function_target_pos[ind];
            return;
        }
    }

    // If the name does not exist, take "RandomWayPoint".
    *ptr_func = array_mobility_function_target_pos[RANDOM_WAY_POINT];
}

// --------------------------------------------------------------------------

struct mobility_t
init_mobility(const char* name, const struct vec2 init_pos)
{
    struct mobility_t mob = {
        .mobility_function_name = name,
        .last_coordinates = init_pos,
        .last_timestamp = mobility_get_timestamp(),
        .duration_to_move = 0, // 0 seconds by default when fish is created.
                               // Because it already reached its target pos.
        .next_coordinates = init_pos // By default the fish is stopped.
    };

    get_mobility_function_duration(name,
        &(mob.mobility_function_duration));

    get_mobility_function_target_pos(name,
        &(mob.mobility_function_target_pos));

    return mob;
}

// --------------------------------------------------------------------------

int random_way_point_duration(const struct mobility_t mob)
{
    return 5;
}

struct vec2 random_way_point_target_pos(const struct mobility_t mob)
{
    srand(mobility_get_timestamp());
    struct vec2 to_return = vec2__zeros();
    to_return.x = rand() % 100;
    to_return.y = rand() % 100;

    return to_return;
}

// --------------------------------------------------------------------------

int diagonal_way_point_duration(const struct mobility_t mob)
{
    if (mob.next_coordinates.x != 100 || mob.next_coordinates.y != 100) {
        return 5;
    }

    return 10;
}

struct vec2 diagonal_way_point_target_pos(const struct mobility_t mob)
{
    // The fish has not been updated yet even if he reached `next_coordinates`.
    struct vec2 current_pos = mob.next_coordinates;
    struct vec2 to_return = vec2__zeros();

    if (current_pos.x == 0 && current_pos.y == 0) {
        to_return.x = 100;
        to_return.y = 100;
    }

    return to_return;
}

// --------------------------------------------------------------------------
