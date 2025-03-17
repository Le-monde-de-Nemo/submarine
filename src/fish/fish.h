#ifndef __FISH__H__
#define __FISH__H__

#include "vec2.h"

// ----------------------------------------------------------------------
// The FISH structure!
// This one contains 2 type of fish :
//      - `PoissonRouge`
//      - `PoissonClown`
//      - `COMMON` = -1, by default
// ----------------------------------------------------------------------

enum species {
    POISSON_ROUGE, // 0
    POISSON_CLOWN, // 1
    NUM_SPECIES, // without COMMON
    COMMON = -1
};

/* Associate a string to a specie.
 * Example:
 *      POISSON_ROUGE --> "PoissonRouge";
 *      POISSON_CLOWN --> "PoissonClown";
 *      COMMON        --> "BasicFish";
*/
char* specie__disp(enum species specie);

// ----------------------------------------------------------------------

struct fish_t {
    enum species specie;
    struct vec2 current_pos; // get_pos | set_pos
    struct vec2 target_pos; // get_target_pos | set_target_pos
    //int time_duration; // in seconds.
    //int fish_speed; // in `meter/seconds`.
};

// ----------------------------------------------------------------------
// Create a fish.
//      type = COMMON.
//      current_pos   = (0, 0).
//      target_pos    = (1, 1).
//      time_duration = 1.
//      fish_speed    = 1.
//
//      The pos is actually a `vec2`. See `src/utils/vec2.h`.
// ----------------------------------------------------------------------

struct fish_t fish__init_fish();

// ----------------------------------------------------------------------
// By default, the type of the fish is COMMON, see `enum SPECIES`.
// ----------------------------------------------------------------------

enum species fish__get_type(const struct fish_t* fish);
struct fish_t*
fish__set_type(const enum species specie, struct fish_t* fish);

// ----------------------------------------------------------------------
// To access to the position of the fish.
//      By default, the fish is at (0, 0).
//      You can set the fish everywhere.
//          it does not verify if the coordinates are in the aquarium.
// ----------------------------------------------------------------------

struct vec2 fish__get_current_pos(const struct fish_t* fish);
struct fish_t*
fish__set_current_pos(const struct vec2 pos, struct fish_t* fish);

struct vec2 fish__get_target_pos(const struct fish_t* fish);
struct fish_t*
fish__set_target_pos(const struct vec2 target_pos, struct fish_t* fish);

int fish__get_move_duration(const struct fish_t* fish);
struct fish_t*
fish__set_move_duration(int time_duration, const struct fish_t* fish);

// ----------------------------------------------------------------------
// Remove the allocated area from a fish creation.
//      The fish is allocated on the stack.
// ----------------------------------------------------------------------

int fish__destroy_fish(struct fish_t* ptr_fish);

// ----------------------------------------------------------------------
#endif // __FISH__H__
