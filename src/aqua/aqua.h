#ifndef __AQUA__H__
#define __AQUA__H__

#include "figure.h"
#include "fish.h"
#include "list.h"
#include "vec2.h"
#include "vue.h"

// ----------------------------------------------------------------------
// The header of the aqua(rium).
//      See `src/aqua/aqua.c`.
//      Dependencies:
//              - `vec2.h`   for the coordinates.
//              - `fish.h`   for the fishes.
//              - `vue.h`    for the views.
//              - `figure.h` for the size and the coordinates.
//              - `list.h`   to store fishes and views.
// ----------------------------------------------------------------------

/* It could be a good idea to put this struct in `src/fish/fish.c`.
 * However, we want to allocate the fishes on the stack.
 * If we put the struct in `src/fish/fish.c` we will need to use malloc.
 */
struct aqua_t {
    struct figure_t fig; // id, size and coord of the aquarium.
                         // See `src/figure/figure.h`

    List list_vues;
    List list_fishes;
};

// ----------------------------------------------------------------------
// Create an aqua(rium).
//  By default:
//      size       = (100, 100).
// ----------------------------------------------------------------------

struct aqua_t aqua__init_aqua(struct vec2 size);

// ----------------------------------------------------------------------

int aqua__get_id(const struct aqua_t* ptr_aqua);

// ----------------------------------------------------------------------
// To manipulate fishes in the aquarium.
//      For now, there is no fish limit.
//      You can set the fish everywhere.
//      However, if it is not in the aquarium, the fish will not be set.
//
//              See `src/fish/fish.h` to individually manipulate a fish.
// ----------------------------------------------------------------------

struct aqua_t
aqua__add_fish(struct fish_t* ptr_fish, const struct aqua_t* ptr_aqua);

struct aqua_t
aqua__del_fish(int id_fish, const struct aqua_t* ptr_aqua);

struct fish_t
aqua__get_fish(int id_fish, const struct aqua_t* ptr_aqua);

struct fish_t*
aqua__get_fishes(const struct aqua_t* ptr_aqua);

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
aqua__add_vue(struct vue_t* ptr_vue, const struct aqua_t* ptr_aqua);

struct aqua_t
aqua__del_vue(int id_vue, const struct aqua_t* ptr_aqua);

struct vue_t
aqua__get_vue(int id_vue, const struct aqua_t* ptr_aqua);

struct vue_t*
aqua__get_vues(const struct aqua_t* ptr_aqua);

// ----------------------------------------------------------------------
// Remove the allocated area from a aqua(rium) creation.
//      The aqua is allocated on the stack.
//      It returns 0 if it has been done with error.
// ----------------------------------------------------------------------

int aqua__destroy_aqua(struct aqua_t* ptr_aqua);

// ----------------------------------------------------------------------
#endif // __AQUA__H__
