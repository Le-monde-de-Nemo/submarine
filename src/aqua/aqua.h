#ifndef __AQUA__H__
#define __AQUA__H__

#include "figure.h"
#include "fish.h"
#include "vec2.h"
#include "vue.h"
#include <sys/queue.h>

/* To get the type of *FILE*. */
#include <bits/types/FILE.h>

// ----------------------------------------------------------------------
// The header of the aqua(rium).
//      See `src/aqua/aqua.c`.
//      Dependencies:
//              - `vec2.h`   for the coordinates.
//              - `fish.h`   for the fishes.
//              - `vue.h`    for the views.
//              - `figure.h` for the size and the coordinates.
// ----------------------------------------------------------------------

/* Struct used by SLIST in `<sys/queue.h>`. */
struct aqua__entry_fish_t {
    struct fish_t* data;
    SLIST_ENTRY(aqua__entry_fish_t)
    entries;
};
SLIST_HEAD(slisthead_fish, aqua__entry_fish_t);

struct aqua__entry_vue_t {
    struct vue_t* data;
    SLIST_ENTRY(aqua__entry_vue_t)
    entries;
};
SLIST_HEAD(slisthead_vue, aqua__entry_vue_t);

/* It could be a good idea to put this struct in `src/fish/fish.c`.
 * However, we want to allocate the fishes on the stack.
 * If we put the struct in `src/fish/fish.c` we will need to use malloc.
 */
struct aqua_t {
    struct figure_t fig; // id, size and coord of the aquarium.
                         // See `src/figure/figure.h`

    struct slisthead_vue list_vues;
    int nb_vues;
    struct slisthead_fish list_fishes;
    int nb_fishes;
};

// ----------------------------------------------------------------------
// Create an aqua(rium).
//  By default:
//      size       = (100, 100).
// ----------------------------------------------------------------------

struct aqua_t aqua__init_aqua(struct vec2 size);

// ----------------------------------------------------------------------

int aqua__get_id(const struct aqua_t aqua);

struct vec2 aqua__get_width_height(const struct aqua_t aqua);

// ----------------------------------------------------------------------
// To display an aqua(rium), it uses fprintf, just give the fd.
//              Format:
//                      - format of size, `${width}x${height}\n`
//                          `${x}` means `1` etc.. , it's an integer.
//                      - format of vues, see `src/vue/vue.h`.
//
//      For instance:
//              fd = stdout --> print in stdout, buffered on lines.
//              fd = stdin  --> print in stderr, buffered on chars.
//
//                              It does not verify fd!
// ----------------------------------------------------------------------

void aqua__disp(FILE* fd, const struct aqua_t aqua);

/* Call `vue__disp()` for every *vue* in the aqua(rium). */
void aqua__disp_vues(FILE* fd, const struct aqua_t aqua);

/* Call `fish__disp()` for every *vue* in the aqua(rium). */
void aqua__disp_fishes(FILE* fd, const struct aqua_t aqua);

void aqua__disp_fishes_without_eol(FILE* fd, const struct aqua_t aqua);

// ----------------------------------------------------------------------
// To manipulate fishes in the aquarium.
//      For now, there is no fish limit.
//      You can set the fish everywhere.
//      However, if it is not in the aquarium, the fish will not be set.
//
//              See `src/fish/fish.h` to individually manipulate a fish.
// ----------------------------------------------------------------------

struct aqua_t
aqua__add_fish(struct fish_t fish, const struct aqua_t aqua);

struct aqua_t
aqua__del_fish(int id_fish, const struct aqua_t aqua);

/* It returns the ptr of the fish, or NULL if not found. */
struct fish_t*
aqua__get_fish(int id_fish, const struct aqua_t aqua);

/* It returns NULL if there are no fishes.
 *      The length of the array is `aqua__get_nb_fishes()`.
 *
 *                      You will have to free the array once used.
 */
struct fish_t*
aqua__get_fishes(const struct aqua_t aqua);

int aqua__get_nb_fishes(const struct aqua_t aqua);

// ----------------------------------------------------------------------
// To manipulate vues in the aquarium.
//      For now, there is no vue limit.
//      You can set the vue everywhere.
//      However, if it is not in the aquarium, the vue will not be set.
//      If the vue is not fully in the aquarium, it will not be set.
//
//              See `src/vue/vue.h` to individually manipulate a vue.
// ----------------------------------------------------------------------

struct aqua_t
aqua__add_vue(struct vue_t vue, const struct aqua_t aqua);

struct aqua_t
aqua__del_vue(int id_vue, const struct aqua_t aqua);

/* It returns the ptr of the fish, or NULL if not found. */
struct vue_t*
aqua__get_vue(int id_vue, const struct aqua_t aqua);

/* It returns NULL if there are no views.
 *      The length of the array is `aqua__get_nb_vues()`.
 *
 *                  You will have to free the array once used.
 */
struct vue_t*
aqua__get_vues(const struct aqua_t aqua);

int aqua__get_nb_vues(const struct aqua_t aqua);

// ----------------------------------------------------------------------
// Remove the allocated area from a aqua(rium) creation.
//      The aqua is allocated on the stack.
//      It returns 0 if it has been done with error.
// ----------------------------------------------------------------------

int aqua__destroy_aqua(struct aqua_t* ptr_aqua);

// ----------------------------------------------------------------------
#endif // __AQUA__H__
