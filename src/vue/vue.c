// --------------------------------------------------------------------------
// The implementation of the vue structure.
//      See `/src/vue/vue.h`.
//      Dependencies:
//              - `vec2.h` for the coordinates, and sizes.
//              - `figure.h` for the id, the pos and the size of a fish.
//
//      The structure contains:
//          - a fig, which is the id, the pos and the size of the vue.
//              See `src/figure/figure.h`
// --------------------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>

#include "figure.h"
#include "vec2.h"
#include "vue.h"

// --------------------------------------------------------------------------
// Create a vue.
//  By default:
//      id         = 0.
//      pos        = (0, 0). See `src/figure/figure.h`
//      size       = (1, 1). See `src/figure/figure.h`
//
//      The pos is actually a `vec2`. See `src/utils/vec2.h`.
// --------------------------------------------------------------------------

struct vue_t
vue__init_vue(int id, struct vec2 pos, struct vec2 size)
{
    struct figure_t fig = {
        .id = id,
        .coordinates = pos,
        .size = size
    };

    struct vue_t vue = {
        .fig = fig
    };

    return vue;
}

// ----------------------------------------------------------------------

int vue__get_id(const struct vue_t vue)
{
    return figure__get_id(vue.fig);
}

// ----------------------------------------------------------------------

void vue__disp(FILE* fd, const struct vue_t vue)
{
    int id_vue = figure__get_id(vue.fig);
    struct vec2 coord = figure__get_current_pos(vue.fig);
    struct vec2 size = figure__get_width_height(vue.fig);

    size_t s_str = 256;
    char cars[s_str];
    snprintf(cars, s_str,
        "N%d %dx%d+%d+%d\n",
        id_vue,
        coord.x, coord.y,
        size.x, size.y);

    fprintf(fd, cars);
}

// ----------------------------------------------------------------------
// To access to the position of the vue.
//      By default, the vue is at (0, 0).
//      You can set the vue everywhere.
//          it does not verify if the coordinates are in the aquarium.
// ----------------------------------------------------------------------

struct vec2 vue__get_current_pos(const struct vue_t vue)
{
    return figure__get_current_pos(vue.fig);
}

struct vue_t
vue__set_current_pos(const struct vec2 pos, struct vue_t vue)
{
    struct vue_t new_vue = {
        .fig = figure__set_current_pos(pos, vue.fig),
    };

    return new_vue;
}

// ----------------------------------------------------------------------
// To access to the size of the vue.
//      By default, the vue is size (1, 1).
// ----------------------------------------------------------------------

struct vec2 vue__get_width_height(const struct vue_t vue)
{
    return figure__get_width_height(vue.fig);
}

struct vue_t
vue__set_width_height(const struct vec2 pos, const struct vue_t vue)
{
    struct vue_t new_vue = {
        .fig = figure__set_width_height(pos, vue.fig),
    };

    return new_vue;
}

// ----------------------------------------------------------------------
// Remove the allocated area from a fish creation.
//      The fish is allocated on the stack.
//      It returns 0 if it has been done with error.
// ----------------------------------------------------------------------

int vue__destroy_vue(struct vue_t* ptr_vue)
{
    if (!ptr_vue) {
        return 1;
    }

    figure__destroy_figure(&(ptr_vue->fig));
    return 1;
}

// ----------------------------------------------------------------------

#ifdef DEBUG_VUE
int main(int argc, char** argv)
{
    struct vue_t vue = vue__init_vue(0, vec2__zeros(), vec2__ones());
    vue__disp(stdout, vue);
    return 0;
}
#endif // DEBUG_VUE

// ----------------------------------------------------------------------
