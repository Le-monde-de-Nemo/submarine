#include "vec2.h"
#include "figure.h"
#include "fish.h"
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
    struct fish_t fish = fish__init_fish(
            POISSON_ROUGE,
            2,
            vec2__zeros(),
            vec2__ones(),
            "RandomWayPoint"
            );

    int local_ret = 0;

    printf("%d\n", fish__get_id(fish));
    local_ret |= myassert(
            fish__get_id(fish) == 2,
            "fish__get_id(fish__init_fish(id=2)) should be 2\n"
            );

    /* When using `specie_disp`. */
    local_ret |= myassert(
            fish__get_type(fish) == POISSON_ROUGE && \
            strcmp(specie__disp(fish__get_type(fish)), "PoissonRouge") == 0,
            "fish__get_id(fish__init_fish(P_ROUGE)) should be P_ROUGE\n"
            );

    char cur_pos[VEC2_DISP_BUFFER_SIZE];
    vec2__disp(fish__get_current_pos(fish), cur_pos, VEC2_DISP_BUFFER_SIZE);
    local_ret |= myassert(
            strcmp(cur_pos, "(0, 0)") == 0,
            "fish__get_current_pos(fish__init_fish((0,0))) should be (0,0)\n"
            );

    local_ret |= myassert(
            strcmp(fish__get_mobility_func(fish), "RandomWayPoint") == 0,
            "fish__get_mobility_func(fish__init_fish(Random)) should be RandomWayPoint\n"
            );

    local_ret |= myassert(
            fish__is_started(&fish) == 0,
            "fish__is_started(fish) should be False(0)\n"
            );

    return local_ret;
}

int test_fish__start_fish(void)
{
    struct fish_t fish = fish__init_fish(
            POISSON_ROUGE,
            2,
            vec2__zeros(),
            vec2__ones(),
            "RandomWayPoint"
            );

    int local_ret = 0;

    /* Stop. */
    local_ret |= myassert(
            fish__is_started(&fish) == 0,
            "fish__is_started(fish) should be False(0)\n"
            );

    /* Start. */
    fish = fish__start_fish(fish);
    local_ret |= myassert(
            fish__is_started(&fish) == 1,
            "fish__is_started(fish started) should be True(1)\n"
            );

    /* Stop. */
    fish = fish__stop_fish(fish);
    local_ret |= myassert(
            fish__is_started(&fish) == 0,
            "fish__is_started(fish) should be False(0)\n"
            );

    return local_ret;
}

int test_fish__set_type(void)
{
    struct fish_t fish = fish__init_fish(
            POISSON_ROUGE,
            2,
            vec2__zeros(),
            vec2__ones(),
            "RandomWayPoint"
            );

    int local_ret = 0;

    /* NOW he is POISSON_CLOWN */
    fish = fish__set_type(POISSON_CLOWN, fish);

    /* When using `specie_disp`. */
    local_ret |= myassert(
            fish__get_type(fish) == POISSON_ROUGE && \
            strcmp(specie__disp(fish__get_type(fish)), "PoissonClown") == 0,
            "fish__get_id(fish__init_fish(P_CLOWN)) should be P_CLOWN\n"
            );

    return local_ret;
}

int main(int argc, char* argv[])
{
    srand(time(NULL));
    int ret = 0;

    ret |= test_fish__init_fish();
    ret |= test_fish__start_fish();
    ret |= test_fish__set_type();

    return ret;
}
