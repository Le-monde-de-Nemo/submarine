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

int figure__get_id(const struct figure_t fig)
{
    return fig.id;
}

// --------------------------------------------------------------------------

struct vec2 figure__get_current_pos(const struct figure_t fig)
{
    return fig.coordinates;
}

struct figure_t
figure__set_current_pos(const struct vec2 pos, const struct figure_t fig)
{
    struct figure_t new_fig = {
        .id = fig.id,
        .coordinates = pos,
        .size = fig.size,
    };

    return new_fig;
}

// --------------------------------------------------------------------------

struct vec2 figure__get_width_height(const struct figure_t fig)
{
    return fig.size;
}

struct figure_t
figure__set_width_height(const struct vec2 size,
    const struct figure_t fig)
{
    struct figure_t new_fig = {
        .id = fig.id,
        .coordinates = fig.coordinates,
        .size = size,
    };

    return new_fig;
}

// --------------------------------------------------------------------------

int figure__destroy_figure(struct figure_t* ptr_fig)
{
    if (!ptr_fig) {
        return 1;
    }

    return 1;
}

// --------------------------------------------------------------------------
