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

enum Species {
    POISSON_ROUGE, // 0
    POISSON_CLOWN, // 1
    COMMON = -1
};

/* Associate a string to a specie.
 * Example:
 *      POISSON_ROUGE --> "PoissonRouge";
 *      POISSON_CLOWN --> "PoissonClown".
*/
char* specie__disp(enum Species specie);

// ----------------------------------------------------------------------

struct Fish {
    enum Species specie;
    struct vec2 coordinates;
};

// ----------------------------------------------------------------------
// Create a fish.
//      type = COMMON.
//      pos  = (0, 0).
//
//      The pos is actually a `vec2`. See `src/utils/vec2.h`.
// ----------------------------------------------------------------------

struct Fish fish__init_fish();

// ----------------------------------------------------------------------
// By default, the type of the fish is COMMON, see `enum SPECIES`.
// ----------------------------------------------------------------------

enum Species fish__getType(const struct Fish* fish);
struct Fish* fish__setType(const enum Species specie, struct Fish* fish);

// ----------------------------------------------------------------------
// To access to the position of the fish.
//      By default, the fish is at (0, 0).
//      You can set the fish everywhere.
//          it does not verify if the coordinates are in the aquarium.
// ----------------------------------------------------------------------

struct vec2 fish__get_pos(const struct Fish* fish);
struct Fish* fish__set_pos(const struct vec2 pos, struct Fish* fish);

struct vec2 fish__getTargetPos(const struct Fish* fish);
struct Fish* fish__setTargetPos(const struct vec2 target_pos, struct Fish* fish);

int fish__getMoveDuration(const struct Fish* fish);
struct Fish* fish__setMoveDuration(const struct Fish* fish, int duration);

// ----------------------------------------------------------------------
// Remove the allocated area from a fish creation.
//      The fish is allocated on the stack.
// ----------------------------------------------------------------------

int fish__destroy_fish(struct Fish* ptr_fish);

// ----------------------------------------------------------------------
#endif // __FISH__H__
