// --------------------------------------------------------------------------
// The implementation of the mobility functions.
//      See `/src/mobility/mobility.h`.
//      Dependencies:
//              - `vec2.h` for the coordinates, and sizes.
// --------------------------------------------------------------------------

#include <string.h>

#include "mobility.h"
#include "vec2.h"

// --------------------------------------------------------------------------
// Those arrays are needed to find the mobility function given a name.
// --------------------------------------------------------------------------

/* The names in that array need to be different! */
static const char* array_mobility_function_names[NUM_MOBILITY_FUNCTIONS] = {
    "RandomWayPoint",
    "DirectWayPoint"
};

static int (*array_mobility_function_duration[NUM_MOBILITY_FUNCTIONS])(const char*, int (*ptr_func)()) = {
    random_way_point_duration,
    direct_way_point_duration
};

static struct vec2 (*array_mobility_function_target_pos[NUM_MOBILITY_FUNCTIONS])(const char*, int (*ptr_func)()) = {
    random_way_point_target_pos,
    direct_way_point_target_pos
};

// --------------------------------------------------------------------------

void get_mobility_function_duration(const char* name, int (*ptr_func)())
{
    // ind could be for instance RANDOM_WAY_POINT or DIRECT_WAY_POINT.
    for (int ind = 0; ind < NUM_MOBILITY_FUNCTIONS; ++ind) {
        if (strcmp(name, array_mobility_function_names[ind]) == 0) {
            ptr_func = array_mobility_function_duration[ind];
            return;
        }
    }

    // If the name does not exist, take "RandomWayPoint".
    ptr_func = array_mobility_function_duration[RANDOM_WAY_POINT];
}

void get_mobility_function_target_pos(const char* name,
    struct vec2 (*ptr_func)())
{
    // ind could be for instance RANDOM_WAY_POINT or DIRECT_WAY_POINT.
    for (int ind = 0; ind < NUM_MOBILITY_FUNCTIONS; ++ind) {
        if (strcmp(name, array_mobility_function_names[ind]) == 0) {
            ptr_func = array_mobility_function_target_pos[ind];
            return;
        }
    }

    // If the name does not exist, take "RandomWayPoint".
    ptr_func = array_mobility_function_target_pos[RANDOM_WAY_POINT];
}

// --------------------------------------------------------------------------

int random_way_point_duration()
{
    // TODO
    return 0;
}

struct vec2 random_way_point_target_pos()
{
    // TODO
    return vec2__zeros();
}

// --------------------------------------------------------------------------

int direct_way_point_duration()
{
    // TODO
    return 0;
}

struct vec2 direct_way_point_target_pos()
{
    // TODO
    return vec2__zeros();
}

// --------------------------------------------------------------------------
