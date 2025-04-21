// --------------------------------------------------------------------------
// The tests for the mobility of a fish.
//                  It has been done with `DirectWayPoint`.
//                      See `src/mobility/mobility.h`.
// --------------------------------------------------------------------------

#include "aqua.h"
#include "figure.h"
#include "fish.h"
#include "vec2.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#ifndef VEC2_DISP_BUFFER_SIZE
#define VEC2_DISP_BUFFER_SIZE 256
#endif

/*
 *  Returns 1 if cond is non-zero, 0 otherwize
 */
int myassert(int cond, char* suffix)
{
    if (cond)
        printf("OK -- %s", suffix);

    else
        printf("KO -- %s", suffix);

    return cond != 0;
}

static int nb_fishes = 100;
static char** array_name_fishes;

int test__update_mobility_with_get_fish(void)
{
    struct aqua_t aqua = aqua__init_aqua(vec2__ones());

    int local_ret = 0;

    for (int i = 0; i < nb_fishes; ++i) {
        struct fish_t fish = fish__init_fish(
            array_name_fishes[i],
            vec2__zeros(),
            vec2__ones(),
            "DirectWayPoint");

        aqua = aqua__add_fish(fish, aqua);
    }

    // local_ret |= myassert(
    // aqua__destroy_aqua(&aqua) == 1,
    //"aqua__destroy_aqua(init_aqua) should be 1\n");

    aqua__destroy_aqua(&aqua);
    return local_ret;
}

int test__update_mobility_with_get_fishes(void)
{
    // struct aqua_t aqua = aqua__init_aqua(vec2__ones());

    int local_ret = 0;

    // local_ret |= myassert(
    // aqua__destroy_aqua(&aqua) == 1,
    //"aqua__destroy_aqua(init_aqua) should be 1\n");

    return local_ret;
}

int main(int argc, char* argv[])
{
    srand(time(NULL));
    int ret = 0;

    array_name_fishes = malloc(nb_fishes * sizeof(char*));

    for (int i = 0; i < nb_fishes; ++i) {
        array_name_fishes[i] = malloc(2048 * sizeof(char));
        strcpy(array_name_fishes[i], (char*)(&i));
    }

    ret |= test__update_mobility_with_get_fish();
    ret |= test__update_mobility_with_get_fishes();

    for (int i = 0; i < nb_fishes; ++i) {
        free(array_name_fishes[i]);
    }
    free(array_name_fishes);

    return ret;
}
