// ----------------------------------------------------------------------
// Implementation of the figure structure!
// This one contains :
//      - `id` the unique identifier of the current figure.
//      - `coordinates` using a vec2.
//      - `size` using a vec2 such as { width(x), height(y) }.
// ----------------------------------------------------------------------

#include "figure.h"
#include "vec2.h"

// --------------------------------------------------------------------------

struct figure_t figure__init_figure(int id, struct vec2 pos, struct vec2 size)
{
    struct figure_t fig = {
        .id = id,
        .coordinates = pos,
        .size = size
    };

    return fig;
}

// --------------------------------------------------------------------------

int figure__get_id(const struct figure_t* ptr_fig)
{
    // TODO
    return 0;
}

// --------------------------------------------------------------------------


struct vec2 figure__get_current_pos(const struct figure_t* ptr_fig)
{
    // TODO
    return vec2__zeros();
}

struct figure_t
figure__set_current_pos(const struct vec2 pos, const struct figure_t* ptr_fig)
{
    // TODO
    return *ptr_fig;
}

// --------------------------------------------------------------------------

struct vec2 figure__get_width_height(const struct figure_t* ptr_fig)
{
    // TODO
    return vec2__zeros();
}

struct figure_t
figure__set_width_height(const struct vec2 size,
        const struct figure_t* ptr_fig)
{
    // TODO
    return *ptr_fig;
}

// --------------------------------------------------------------------------

int figure__destroy_figure(struct figure_t* ptr_fig)
{
    // TODO
    return 1;
}

// --------------------------------------------------------------------------

