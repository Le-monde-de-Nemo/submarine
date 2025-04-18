#include "fish.h"
#include "figure.h"
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

int test_fish__init_fish(void)
{
    char name_fish[] = "PoissonRouge1";
    struct fish_t fish = fish__init_fish(
        name_fish,
        vec2__zeros(),
        vec2__ones(),
        "RandomWayPoint");

    int local_ret = 0;

    long n = 256;
    char dst[n];
    printf("%s\n", fish__disp(fish, dst, n));

    local_ret |= myassert(
        fish__get_id(fish) == convert_name_to_id(name_fish),
        "fish__get_id(fish__init_fish(\"PoissonRouge1\")) should be correct\n");

    char cur_pos[VEC2_DISP_BUFFER_SIZE];
    vec2__disp(fish__get_current_pos(fish), cur_pos, VEC2_DISP_BUFFER_SIZE);
    local_ret |= myassert(
        strcmp(cur_pos, "(0, 0)") == 0,
        "fish__get_current_pos(fish__init_fish((0,0))) should be (0,0)\n");

    local_ret |= myassert(
        strcmp(fish__get_mobility_func(fish), "RandomWayPoint") == 0,
        "fish__get_mobility_func(fish__init_fish(Random)) should be RandomWayPoint\n");

    local_ret |= myassert(
        fish__is_started(fish) == 0,
        "fish__is_started(fish) should be False(0)\n");

    return local_ret;
}

int test_fish__start_fish(void)
{
    struct fish_t fish = fish__init_fish(
        "PoissonRouge",
        vec2__zeros(),
        vec2__ones(),
        "RandomWayPoint");

    int local_ret = 0;

    long n = 256;
    char dst[n];
    printf("%s", fish__disp(fish, dst, n));

    /* Stop. */
    local_ret |= myassert(
        fish__is_started(fish) == 0,
        "fish__is_started(fish) should be False(0)\n");

    /* Start. */
    fish = fish__start_fish(fish);
    local_ret |= myassert(
        fish__is_started(fish) == 1,
        "fish__is_started(fish started) should be True(1)\n");

    /* Stop. */
    fish = fish__stop_fish(fish);
    local_ret |= myassert(
        fish__is_started(fish) == 0,
        "fish__is_started(fish) should be False(0)\n");

    return local_ret;
}

int main(int argc, char* argv[])
{
    srand(time(NULL));
    int ret = 0;

    ret |= test_fish__init_fish();
    ret |= test_fish__start_fish();

    return ret;
}
