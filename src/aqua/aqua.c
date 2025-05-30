// --------------------------------------------------------------------------
// The implementation of the aqua(rium) structure.
//      See `/src/aqua/aqua.h`.
//      Dependencies:
//              - `vec2.h` for the coordinates, and sizes.
//              - `figure.h` for the id, the pos and the size of a fish/vue.
//              - `fish.h` to manipulate a fish.
//              - `vue.h` to manipulate a vue.
//
//      The structure contains:
//          - a fig, which is the id, the coord and the size of the aqua.
//          - a single list of fishes, using `<sys/queue.h>`.
//          - a single list of views, using `<sys/queue.h>`.
//              See `src/figure/figure.h`
// --------------------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/queue.h>

#include "aqua.h"
#include "debug.h"
#include "figure.h"
#include "fish.h"
#include "vec2.h"
#include "vue.h"

#ifndef VIEW_CONF_BUF_SIZE
#define VIEW_CONF_BUF_SIZE 4096
#endif

#ifndef MAX_BUFFER_SIZE
#define MAX_BUFFER_SIZE 256
#endif

// --------------------------------------------------------------------------
// Used to fastly find a fish or a vue in the current list implementation.
//                                                  See `sys/queue.h`.
// --------------------------------------------------------------------------

struct aqua__entry_vue_t*
aqua__find_list_elt_vue(int id_vue, struct aqua_t aqua)
{
    struct aqua__entry_vue_t* np;
    struct slisthead_vue head_vue = aqua.list_vues;

    DBG("FIND FIND");
    SLIST_FOREACH(np, &head_vue, entries)
    {
        DBG("FIND %p", np);
        if (vue__get_id(*(np->data)) == id_vue)
            return np;
    }

    return NULL;
}

struct aqua__entry_fish_t*
aqua__find_list_elt_fish(int id_fish, const struct aqua_t aqua)
{
    struct aqua__entry_fish_t* np;
    struct slisthead_fish head_fish = aqua.list_fishes;

    SLIST_FOREACH(np, &head_fish, entries)
    {
        if (fish__get_id(*(np->data)) == id_fish) {
            return np;
        }
    }

    return NULL;
}

// --------------------------------------------------------------------------

struct aqua_t aqua__init_aqua(struct vec2 size)
{
    struct aqua_t aqua = {
        .fig = figure__init_figure(0, vec2__zeros(), size),
        .nb_vues = 0,
        .nb_fishes = 0,
        .available_vues = malloc(sizeof(struct tailqhead_vue)),
    };

    SLIST_INIT(&aqua.list_vues);
    SLIST_INIT(&aqua.list_fishes);
    TAILQ_INIT(aqua.available_vues);

    return aqua;
}

// --------------------------------------------------------------------------

int aqua__get_id(const struct aqua_t aqua)
{
    return figure__get_id(aqua.fig);
}

struct vec2 aqua__get_width_height(const struct aqua_t aqua)
{
    return figure__get_width_height(aqua.fig);
}

// --------------------------------------------------------------------------

char* aqua__disp(const struct aqua_t aqua, char* dst, long n)
{
    struct vec2 size = figure__get_width_height(aqua.fig);
    int res = snprintf(dst, n, "%dx%d\n", size.x, size.y);

    if (res < 0 || n <= res)
        return dst;

    long size_printed = res;
    aqua__disp_vues(aqua, dst + size_printed, n - size_printed);

    return dst;
}

char* aqua__disp_vues(const struct aqua_t aqua, char* dst, long n)
{
    struct slisthead_vue head = aqua.list_vues;
    struct aqua__entry_vue_t* np;
    long written = 0;

    SLIST_FOREACH(np, &head, entries)
    {
        char cars_vue[MAX_BUFFER_SIZE];
        vue__disp(*(np->data), cars_vue, MAX_BUFFER_SIZE);

        int res = snprintf(dst + written, n - written, "%s", cars_vue);
        if (res < 0 || res >= (n - written)) {
            // res < 0 means characters have not been written, including `\0`.
            return dst;
        }

        written += res;
    }

    return dst;
}

char* aqua__disp_fishes(const struct aqua_t aqua, char* dst, long n)
{
    struct slisthead_fish head = aqua.list_fishes;
    struct aqua__entry_fish_t* np;
    long written = 0;

    SLIST_FOREACH(np, &head, entries)
    {
        char cars_fish[MAX_BUFFER_SIZE];
        fish__disp(*(np->data), cars_fish, MAX_BUFFER_SIZE);

        int res = snprintf(dst + written, n - written, "%s", cars_fish);
        if (res < 0 || res >= (n - written)) {
            // res < 0 means characters have not been written, including `\0`.
            return dst;
        }

        written += res;
    }

    return dst;
}

// --------------------------------------------------------------------------

struct aqua_t
aqua__add_fish(struct fish_t fish, const struct aqua_t aqua)
{
    struct slisthead_fish new_head = aqua.list_fishes;

    // <fish already in the aqua?> do not add if it is already existing.
    if (aqua__get_fish(fish__get_name(fish), aqua) != NULL) {
        return aqua;
    } // </fish already in the aqua?>

    // <fish copy>, copy on the heap.
    struct aqua__entry_fish_t* n2 = malloc(sizeof(struct aqua__entry_fish_t));
    n2->data = (struct fish_t*)malloc(sizeof(struct fish_t));
    if (!(n2->data)) {
        TRACE("Impossible to alloc the fish.");
        exit(EXIT_FAILURE);
    }
    memcpy(n2->data, &fish, sizeof(struct fish_t));
    // </fish copy done>.

    SLIST_INSERT_HEAD(&new_head, n2, entries);

    struct aqua_t new_aqua = {
        .fig = aqua.fig,
        .list_vues = aqua.list_vues,
        .available_vues = aqua.available_vues,
        .nb_vues = aqua.nb_vues,
        .list_fishes = new_head,
        .nb_fishes = aqua.nb_fishes + 1
    };

    return new_aqua;
}

struct aqua_t
aqua__del_fish(const char* name_fish, const struct aqua_t aqua)
{
    if (aqua.nb_fishes <= 0) {
        return aqua;
    }
    int id_fish = convert_name_to_id(name_fish);

    // `aqua__find_list_elt_fish` returns NULL if id_fish is not found.
    struct aqua__entry_fish_t* n1 = aqua__find_list_elt_fish(id_fish, aqua);
    if (n1 == NULL) {
        return aqua;
    }

    struct slisthead_fish new_head = aqua.list_fishes;
    SLIST_REMOVE(&new_head, n1, aqua__entry_fish_t, entries);
    fish__destroy_fish(n1->data);
    free(n1->data); // Fish is copied outside the stack.
    free(n1);

    struct aqua_t new_aqua = {
        .fig = aqua.fig,
        .list_vues = aqua.list_vues,
        .available_vues = aqua.available_vues,
        .nb_vues = aqua.nb_vues,
        .list_fishes = new_head,
        .nb_fishes = aqua.nb_fishes - 1
    };

    return new_aqua;
}

struct fish_t*
aqua__get_fish(const char* name_fish, const struct aqua_t aqua)
{
    if (aqua.nb_fishes <= 0) {
        return NULL;
    }
    int id_fish = convert_name_to_id(name_fish);

    // `aqua__find_list_elt_fish` returns NULL if id_fish is not found.
    struct aqua__entry_fish_t* n1 = aqua__find_list_elt_fish(id_fish, aqua);
    if (n1 == NULL) {
        return NULL;
    }

    *(n1->data) = fish__update_mobility(*(n1->data));
    return n1->data;
}

struct fish_t*
aqua__get_fishes(const struct aqua_t aqua)
{
    if (aqua.nb_fishes <= 0) {
        return NULL;
    }

    struct fish_t*
        array
        = (struct fish_t*)malloc(aqua.nb_fishes * sizeof(struct fish_t));
    if (!array) {
        TRACE("Impossible to alloc the array.");
        exit(EXIT_FAILURE);
    }

    int index_array = 0;
    struct slisthead_fish head = aqua.list_fishes;
    struct aqua__entry_fish_t* np;
    SLIST_FOREACH(np, &head, entries)
    {
        *(np->data) = fish__update_mobility(*(np->data));
        array[index_array++] = *(np->data);
    }

    return array;
}

int aqua__get_nb_fishes(const struct aqua_t aqua)
{
    return aqua.nb_fishes;
}

// --------------------------------------------------------------------------

struct aqua_t
aqua__add_vue(struct vue_t vue, struct aqua_t aqua)
{
    // <vue already in the aqua?> do not add if it is already existing.
    if (aqua__get_vue(vue__get_id(vue), aqua) != NULL)
        return aqua;

    // <vue copy>, copied on the heap.
    struct aqua__entry_vue_t* n1 = malloc(sizeof(struct aqua__entry_vue_t));
    n1->data = (struct vue_t*)malloc(sizeof(struct vue_t));
    if (!(n1->data)) {
        TRACE("Impossible to alloc the vue.");
        exit(EXIT_FAILURE);
    }
    *n1->data = vue;
    // </vue copy done>.

    SLIST_INSERT_HEAD(&aqua.list_vues, n1, entries);
    aqua.nb_vues++;
    aqua__add_available_vue(n1->data, &aqua);

    return aqua;
}

struct aqua_t
aqua__del_vue(int id_vue, struct aqua_t aqua)
{
    if (aqua.nb_vues <= 0)
        return aqua;

    // `aqua__find_list_elt_vue` returns NULL if id_fish is not found.
    struct aqua__entry_vue_t* n1 = aqua__find_list_elt_vue(id_vue, aqua);
    if (n1 == NULL)
        return aqua;

    SLIST_REMOVE(&aqua.list_vues, n1, aqua__entry_vue_t, entries);
    aqua__get_available_vue(id_vue, &aqua);

    free(n1->data); // Vue is copied outside the stack.
    free(n1);
    aqua.nb_vues--;

    return aqua;
}

struct vue_t*
aqua__get_vue(int id_vue, const struct aqua_t aqua)
{
    DBG("GETVUE%d", aqua.nb_vues);
    if (aqua.nb_vues <= 0)
        return NULL;

    // `aqua__find_list_elt_vue` returns NULL if id_fish is not found.
    struct aqua__entry_vue_t* n1 = aqua__find_list_elt_vue(id_vue, aqua);
    if (n1 == NULL)
        return NULL;

    return n1->data;
}

struct vue_t*
aqua__get_available_vue(int id_vue, struct aqua_t* aqua)
{
    struct aqua__available_vue_t* vue = TAILQ_FIRST(aqua->available_vues);
    DBG("GETAVAILABLEVUE");

    while (vue != TAILQ_LAST(aqua->available_vues, tailqhead_vue)) {
        DBG("VUE%p", vue);

        if (vue__get_id(*vue->data) == id_vue) {
            TAILQ_REMOVE(aqua->available_vues, vue, entries);
            struct vue_t* out = vue->data;
            free(vue);
            return out;
        }

        vue = TAILQ_NEXT(vue, entries);
    }

    if (vue__get_id(*vue->data) == id_vue) {
        TAILQ_REMOVE(aqua->available_vues, vue, entries);
        struct vue_t* out = vue->data;
        free(vue);
        return out;
    }

    return NULL;
}

struct vue_t*
aqua__get_first_available_vue(struct aqua_t* aqua)
{
    struct aqua__available_vue_t* vue = TAILQ_FIRST(aqua->available_vues);
    DBG("first %p", vue);
    if (vue == NULL)
        return NULL;

    TAILQ_REMOVE(aqua->available_vues, vue, entries);
    struct vue_t* out = vue->data;
    free(vue);
    return out;
}

void aqua__add_available_vue(struct vue_t* vue, struct aqua_t* aqua)
{
    DBG("ADD AVAILABLE VUE : %p", vue);
    struct aqua__available_vue_t* newvue = malloc(sizeof(struct aqua__available_vue_t));

    newvue->data = vue;
    TAILQ_INSERT_TAIL(aqua->available_vues, newvue, entries);
    DBG("%p %p %p", vue, newvue, TAILQ_FIRST(aqua->available_vues));
}

struct vue_t*
aqua__get_vues(const struct aqua_t aqua)
{
    if (aqua.nb_vues <= 0)
        return NULL;

    struct vue_t*
        array
        = (struct vue_t*)malloc(aqua.nb_vues * sizeof(struct vue_t));
    if (!array) {
        TRACE("Impossible to alloc the array.");
        exit(EXIT_FAILURE);
    }

    int index_array = 0;
    struct slisthead_vue head = aqua.list_vues;
    struct aqua__entry_vue_t* np;
    SLIST_FOREACH(np, &head, entries)
    array[index_array++] = *(np->data);

    return array;
}

int aqua__get_nb_vues(const struct aqua_t aqua)
{
    return aqua.nb_vues;
}

// --------------------------------------------------------------------------

int aqua__destroy_aqua(struct aqua_t* ptr_aqua)
{
    if (!ptr_aqua) {
        return 1;
    }

    // Free the list of vues.
    struct aqua__entry_vue_t* n1;
    struct slisthead_vue head_vue = ptr_aqua->list_vues;
    while (!SLIST_EMPTY(&head_vue)) {
        n1 = SLIST_FIRST(&head_vue);
        SLIST_REMOVE_HEAD(&head_vue, entries);
        free(n1->data);
        free(n1);
        ptr_aqua->nb_vues -= 1;
    }

    // Free the list of fishes.
    struct aqua__entry_fish_t* n2;
    struct slisthead_fish head_fish = ptr_aqua->list_fishes;
    while (!SLIST_EMPTY(&head_fish)) {
        n2 = SLIST_FIRST(&head_fish);
        SLIST_REMOVE_HEAD(&head_fish, entries);
        fish__destroy_fish(n2->data);
        free(n2->data);
        free(n2);
        ptr_aqua->nb_fishes -= 1;
    }

    struct aqua__available_vue_t* vue;
    while (!TAILQ_EMPTY(ptr_aqua->available_vues)) {
        vue = TAILQ_FIRST(ptr_aqua->available_vues);
        TAILQ_REMOVE(ptr_aqua->available_vues, vue, entries);
        free(vue);
    }
    free(ptr_aqua->available_vues);

    return 1;
}

struct aqua_t aqua__from_file(char* pathname)
{
    char read_buf[VIEW_CONF_BUF_SIZE] = {};

    FILE* file = fopen(pathname, "r");

    // Read the dimensions of the aquarium, this is the only required line
    if (fgets(read_buf, VIEW_CONF_BUF_SIZE, file) == NULL) {
        TRACE("Failed to parse view from file %s", pathname);
        exit(1);
    }

    int width, height;
    sscanf(read_buf, "%dx%d", &width, &height);
    struct aqua_t aqua = aqua__init_aqua(
        vec2__create(width, height));

    // Iterate through the lines, and get all configured views
    int id, x, y;
    while (fgets(read_buf, VIEW_CONF_BUF_SIZE, file) != NULL) {
        sscanf(read_buf, "N%d %dx%d+%d+%d", &id, &x, &y, &width, &height);

        struct vec2 pos = vec2__create(x, y);
        struct vec2 size = vec2__create(width, height);
        struct vue_t vue = vue__init_vue(id, pos, size);
        aqua = aqua__add_vue(vue, aqua);
    }

    fclose(file);

    return aqua;
}

void aqua__save_file(char* pathname, struct aqua_t aqua)
{
    FILE* file = fopen(pathname, "w");

    struct vec2 vec = aqua__get_width_height(aqua);
    fprintf(file, "%dx%d\n", vec.x, vec.y);

    struct vue_t* vues = aqua__get_vues(aqua);
    if (vues != NULL)
        for (int i = 0; i < aqua__get_nb_vues(aqua); ++i) {
            int id = vue__get_id(vues[i]);
            struct vec2 size = vue__get_width_height(vues[i]);
            struct vec2 pos = vue__get_current_pos(vues[i]);
            fprintf(file, "N%d %dx%d+%d+%d\n", id, pos.x, pos.y, size.x, size.y);
        }

    free(vues);
    fclose(file);
}

// ----------------------------------------------------------------------
