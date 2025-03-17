#ifndef __FISH__H__
#define __FISH__H__

#include "vec2.h"
#include "figure.h"

// ----------------------------------------------------------------------
// The header of the fish.
//      See `src/fish/fish.c`.
//      Dependencies:
//              - `figure.h` for the id, coordinates, and size.
//              - `vec2.h` for the coordinates.
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

/* It could be a good idea to put this struct in `src/fish/fish.c`.
 * However, we want to allocate the fishes on the stack.
 * If we put the struct in `src/fish/fish.c` we will need to use malloc.
*/
struct fish_t {
    enum species specie;
    int is_started; // 0 means not, 1 or other things mean yes.
    struct figure_t fig;
    //void* (*mobility_function)(void*);
};

// ----------------------------------------------------------------------
// Create a fish.
//  By default:
//      id         = 0.
//      specie     = COMMON.
//      is_started = 0. 0 means not. 1 means yes.
//      pos        = (0, 0). See `src/figure/figure.h`
//      size       = (1, 1). See `src/figure/figure.h`
//
//      The pos is actually a `vec2`. See `src/utils/vec2.h`.
// ----------------------------------------------------------------------

struct fish_t
fish__init_fish(int id, int is_started, enum species specie,
                        struct vec2 pos, struct vec2 size);

// ----------------------------------------------------------------------

int fish__get_id(const struct fish_t* ptr_fish);
int fish__is_started(const struct fish_t* ptr_fish);

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
fish__set_move_duration(int time_duration, struct fish_t* fish);

// ----------------------------------------------------------------------
// Remove the allocated area from a fish creation.
//      The fish is allocated on the stack.
//      It returns 0 if it has been done with error.
// ----------------------------------------------------------------------

int fish__destroy_fish(struct fish_t* ptr_fish);

// ----------------------------------------------------------------------
#endif // __FISH__H__
