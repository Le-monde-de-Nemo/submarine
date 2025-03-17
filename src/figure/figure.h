#ifndef __FIGURE__H__
#define __FIGURE__H__

// --------------------------------------------------------------------------
// The header of the figure.
//      See `/src/figure/figure.c`.
//      Dependencies:
//              - `vec2.h` for the coordinates and size.
// --------------------------------------------------------------------------

#include "vec2.h"

/* see `src/figure/figure.c` for more details! */
struct figure_t;

// --------------------------------------------------------------------------
// Create a figure.
//      - `id` is the unique identifier of the figure.
//      - `pos` is the coordinates of the bottom left corner.
//      - `size` is the size of the figure
// such as `pos` + `right` is the coordinates of the upper right corner.
//
//          See `src/figure/figure.c`.
// --------------------------------------------------------------------------

struct figure_t figure__init_figure(int id, struct vec2 pos, struct vec2 size);

// --------------------------------------------------------------------------

int figure__get_id(const struct figure_t* ptr_fig);

// --------------------------------------------------------------------------
//  To manipulate the position of the figure.
//      `pos` is the coordinates of the bottom left corner.
//
//      `set_current_pos` returns a new figure, with the new pos.
// --------------------------------------------------------------------------

struct vec2 figure__get_current_pos(const struct figure_t* ptr_fig);
struct figure_t
figure__set_current_pos(const struct vec2 pos, const struct figure_t* ptr_fig);

// --------------------------------------------------------------------------
//  To manipulate the size of the figure.
//      `size` is the coordinates of the upper right corner.
//
//      `set_current_size` returns a new figure, with the new size.
// --------------------------------------------------------------------------

struct vec2 figure__get_width_height(const struct figure_t* ptr_fig);
struct figure_t
figure__set_width_height(const struct vec2 size,
        const struct figure_t* ptr_fig);

// --------------------------------------------------------------------------
// Remove the allocated area from a figure creation.
//      The figure is allocated on the stack.
//
//      It returns 0 if it has been done with error.
// --------------------------------------------------------------------------

int figure__destroy_figure(struct figure_t* ptr_fig);

// --------------------------------------------------------------------------

#endif // __FIGURE__H__
