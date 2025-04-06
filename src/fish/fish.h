#ifndef __FISH__H__
#define __FISH__H__

#include "figure.h"
#include "vec2.h"

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

    // To know where and when the fish will go, given a former position.
    // See `src/fish/fish.c`.
    int (*mobility_function_duration)();
    struct vec2 (*mobility_function_target_pos)();
    const char* mobility_function_name;
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
fish__init_fish(enum species specie,
    int id, struct vec2 pos, struct vec2 size,
    const char* mobility_func);

// ----------------------------------------------------------------------

int fish__get_id(const struct fish_t fish);

/* Returning Examples:
 *  "RandomWayPoint";
 *  "DirectWayPoint";
 *
 *  See `src/mobility/mobility.h`.
 *
 *  Returns NULL if the fish does not exist or if the mob func is not init.
 */
const char* fish__get_mobility_func(const struct fish_t fish);

// ----------------------------------------------------------------------

int fish__is_started(const struct fish_t fish);
struct fish_t fish__start_fish(struct fish_t fish);
struct fish_t fish__stop_fish(struct fish_t fish);

// ----------------------------------------------------------------------
// By default, the type of the fish is COMMON, see `enum SPECIES`.
// ----------------------------------------------------------------------

enum species fish__get_type(const struct fish_t fish);
struct fish_t
fish__set_type(const enum species specie, const struct fish_t fish);

// ----------------------------------------------------------------------
// To display a fish, it uses snprintf.
//              Format:
//                  `"[PoissonRouge at fish_xxfish_y,size_xxsize_y,id]"`
//
//      It writes at most n chars to dst to display *fish*.
//
//      You can specify if you want the EOL:
//          `fish__disp` --> there is `\n` at the end of the format.
//      `...without_eol` --> there is not `\n` at the end of the format.
//
//                                          It returns *dst*.
// ----------------------------------------------------------------------

char* fish__disp(const struct fish_t fish, char* dst, long n);

char* fish__disp_without_eol(const struct fish_t fish, char* dst, long n);

// ----------------------------------------------------------------------
// To access to the position of the fish.
//      By default, the fish is at (0, 0).
//      You can set the fish everywhere.
//          it does not verify if the coordinates are in the aquarium.
// ----------------------------------------------------------------------

struct vec2 fish__get_current_pos(const struct fish_t fish);
struct fish_t
fish__set_current_pos(const struct vec2 pos, struct fish_t fish);

// ----------------------------------------------------------------------
// To access to the next pos of the fish, given his mobility functions.
//      By default, the fish is moving randomly, using
//                                                  `random_way_point`.
//
//      You should never call directly the mobility functions
//                                                  from the struct.
//      Use these functions below instead!
// ----------------------------------------------------------------------

struct vec2 fish__get_target_pos(const struct fish_t fish);
int fish__get_move_duration(const struct fish_t fish);

// ----------------------------------------------------------------------
// Remove the allocated area from a fish creation.
//      The fish is allocated on the stack.
//      It returns 0 if it has been done with error.
// ----------------------------------------------------------------------

int fish__destroy_fish(struct fish_t* ptr_fish);

// ----------------------------------------------------------------------
#endif // __FISH__H__
