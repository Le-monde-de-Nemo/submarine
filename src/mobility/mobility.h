#ifndef __MOBILITY__H__
#define __MOBILITY__H__

#include "vec2.h"

// ----------------------------------------------------------------------
// To know how a fish moves.
//      See `src/mobility/mobility.c`.
//      Dependencies:
//              - `vec2.h` for the coordinates.
// ----------------------------------------------------------------------

enum mobility_functions {
    RANDOM_WAY_POINT, // 0
    DIRECT_WAY_POINT, // 1
    NUM_MOBILITY_FUNCTIONS
};

// ----------------------------------------------------------------------
// To get the right mobility functions.
//      Give a name such as "RandomWayPoint" or "DirectWayPoint".
//      It will modify the given ptr with
//          the right mobility function for duration or movement.
//
//      If the name is not recognized, then the "RandomWayPoint" is used.
// ----------------------------------------------------------------------

void
get_mobility_function_duration(const char* name, int (*ptr_func)());
void
get_mobility_function_target_pos(const char* name,
                                 struct vec2 (*ptr_func)());

// ----------------------------------------------------------------------
// Mobility Function
//      id: 0.
//      name: "RandomWayPoint".
//      spec: give a random position with a random duration.
//
//  It is the by default mobility function for a fish.
//  See `src/fish/fish.c`
// ----------------------------------------------------------------------

int random_way_point_duration();
struct vec2 random_way_point_target_pos();

// ----------------------------------------------------------------------
// Mobility Function
//      id: 0.
//      name: "DirectWayPoint".
//      spec: give the most direct way to go to a given position.
//
//  See `src/fish/fish.c`
// ----------------------------------------------------------------------

int direct_way_point_duration();
struct vec2 direct_way_point_target_pos();

// ----------------------------------------------------------------------

#endif // __MOBILITY__H__
