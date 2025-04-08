#ifndef __VUE__H__
#define __VUE__H__

#include "figure.h"
#include "vec2.h"

// ----------------------------------------------------------------------
// The header of the vue.
//      See `src/vue/vue.c`.
//      Dependencies:
//              - `figure.h` for the id, coordinates, and size.
//              - `vec2.h` for the coordinates.
// ----------------------------------------------------------------------

/* It could be a good idea to put this struct in `src/vue/vue.c`.
 * However, we want to allocate the fishes on the stack.
 * If we put the struct in `src/vue/vue.c` we will need to use malloc.
 */
struct vue_t {
    struct figure_t fig;
};

// ----------------------------------------------------------------------
// Create a vue.
//  By default:
//      id         = 0.
//      pos        = (0, 0). See `src/figure/figure.h`
//      size       = (1, 1). See `src/figure/figure.h`
//
//      The pos is actually a `vec2`. See `src/utils/vec2.h`.
// ----------------------------------------------------------------------

struct vue_t
vue__init_vue(int id, struct vec2 pos, struct vec2 size);

// ----------------------------------------------------------------------

int vue__get_id(const struct vue_t vue);

// ----------------------------------------------------------------------
// To display a vue, it uses snprintf.
//              Format:
//                  `"vue_xxvue_y+vue_width+vue_height\n"`
//
//      It writes at most n chars to dst to display *vue*.
//
//                                          It returns *dst*.
// ----------------------------------------------------------------------

char* vue__disp(const struct vue_t vue, char* dst, long n);

// ----------------------------------------------------------------------
// To access to the position of the vue.
//      By default, the vue is at (0, 0).
//      You can set the vue everywhere.
//          it does not verify if the coordinates are in the aquarium.
// ----------------------------------------------------------------------

struct vec2 vue__get_current_pos(const struct vue_t vue);
struct vue_t
vue__set_current_pos(const struct vec2 pos, const struct vue_t vue);

// ----------------------------------------------------------------------
// To access to the size of the vue.
//      By default, the vue is size (1, 1).
// ----------------------------------------------------------------------

struct vec2 vue__get_width_height(const struct vue_t vue);
struct vue_t
vue__set_width_height(const struct vec2 pos, const struct vue_t vue);

// ----------------------------------------------------------------------
// Remove the allocated area from a fish creation.
//      The fish is allocated on the stack.
//      It returns 0 if it has been done with error.
// ----------------------------------------------------------------------

int vue__destroy_vue(struct vue_t* ptr_vue);

// ----------------------------------------------------------------------
#endif // __VUE__H__
