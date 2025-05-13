#ifndef __MOBILITY__H__
#define __MOBILITY__H__

#include "vec2.h"
#include <sys/queue.h>
#include <time.h>

// ----------------------------------------------------------------------
// To know how a fish moves.
//      See `src/mobility/mobility.c`.
//      Dependencies:
//              - `vec2.h` for the coordinates.
// ----------------------------------------------------------------------

enum mobility_functions {
    RANDOM_WAY_POINT, // 0
    DIAGONAL_WAY_POINT, // 1
    NUM_MOBILITY_FUNCTIONS
};

// ----------------------------------------------------------------------
// To store a position to reach for the fish.
//      See `src/mobility/mobility.c`.
// ----------------------------------------------------------------------

struct mobility_entry_t {
    // This is a `FILO` from `<sys/queue.h>`.
    TAILQ_ENTRY(mobility_entry_t)
    entries;

    // The displayer could call `get_fishes`
    //          before one fish reached his goal.
    // The idea here is to compare timestamps
    //          to ensure the move duration is over.
    time_t last_timestamp;

    // the next pos to move.
    int duration_to_move; // Time in seconds.
    struct vec2 next_coordinates;
};

// ----------------------------------------------------------------------
// To store the mobility information for a fish.
//      See `src/mobility/mobility.c`.
// ----------------------------------------------------------------------

// This is a `FILO` of position to reach for the fish.
TAILQ_HEAD(stail_mobilities, mobility_entry_t);

struct mobility_t {
    // Could be "RandomWayPoint" or "DirectWayPoint".
    const char* mobility_function_name;

    // To know where and when the fish will go, given a former position.
    // See `src/fish/fish.c`.
    int (*mobility_function_duration)(const struct mobility_t);
    struct vec2 (*mobility_function_target_pos)(const struct mobility_t);

    struct stail_mobilities head;
};

// ----------------------------------------------------------------------
// A function to get the current timestamp.
//                   It returns seconds since `January 1, 1970`.
// ----------------------------------------------------------------------

time_t mobility_get_timestamp();

// ----------------------------------------------------------------------
// To get the right mobility functions.
//      Give a name such as "RandomWayPoint" or "DirectWayPoint".
//      It will modify the given ptr with
//          the right mobility function for duration or movement.
//
//      If the name is not recognized, then the "RandomWayPoint" is used.
// ----------------------------------------------------------------------

void get_mobility_function_duration(const char* name,
    int (**ptr_func)(const struct mobility_t));
void get_mobility_function_target_pos(const char* name,
    struct vec2 (**ptr_func)(const struct mobility_t));

// ----------------------------------------------------------------------
//          name could be `RandomWayPoint` or `DirectWayPoint`, etc...
//              If it is not recognized, `RandomWayPoint` will be used.
//
// It is the function that will do:
//          - `get_mobility_function_duration`,
//          - `get_mobility_function_target_pos`.
//
//              It is used by the fish.
// ----------------------------------------------------------------------

struct mobility_t
init_mobility(const char* name, const struct vec2 init_pos);

/* Returns 1 if it has been done with erros, 0 otherwise. */
int destroy_mobility(const struct mobility_t* ptr_mob);

// ----------------------------------------------------------------------
// Mobility Function
//      id: 0.
//      name: "RandomWayPoint".
//      spec: give a random position with a random duration.
//
//  It is the by default mobility function for a fish.
//  See `src/fish/fish.c`
// ----------------------------------------------------------------------

int random_way_point_duration(const struct mobility_t mob);
struct vec2 random_way_point_target_pos(const struct mobility_t mob);

// ----------------------------------------------------------------------
// Mobility Function
//      id: 1.
//      name: "DiagonalWayPoint".
//      spec: It does a diagonal from (0, 0) to (100, 100).
//
//  See `src/fish/fish.c`
// ----------------------------------------------------------------------

int diagonal_way_point_duration(const struct mobility_t mob);
struct vec2 diagonal_way_point_target_pos(const struct mobility_t mob);

// ----------------------------------------------------------------------

#endif // __MOBILITY__H__
