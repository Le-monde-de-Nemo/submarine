// --------------------------------------------------------------------------
// The tests for the mobility of a fish.
//                  It has been done with `DiagonalWayPoint`.
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
#include <unistd.h>

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

static int nb_fishes = 4;
static char** array_name_fishes;

int test__update_mobility_with_get_fish(void)
{
    struct aqua_t aqua = aqua__init_aqua(vec2__ones());

    int local_ret = 0;

    for (int i = 0; i < nb_fishes; ++i) {
        struct vec2 init_pos = vec2__zeros();
        if (i == 1 || i == 3) {
            init_pos.x = 100;
            init_pos.y = 100;
        }

        struct fish_t fish = fish__init_fish(
            array_name_fishes[i],
            init_pos,
            vec2__ones(),
            "DiagonalWayPoint");

        aqua = aqua__add_fish(fish, aqua);
    }

    // Fish0 --> started at (0,0).
    // Fish1 --> started at (100, 100).
    // Fish2 --> not started, at (0, 0). (by default)
    // Fish3 --> not started, at (100, 100). (by default)
    struct fish_t* ptr_fish;

    // Start Fish0.
    ptr_fish = aqua__get_fish(array_name_fishes[0], aqua);
    *ptr_fish = fish__start_fish(*ptr_fish);

    // Start Fish1.
    ptr_fish = aqua__get_fish(array_name_fishes[1], aqua);
    *ptr_fish = fish__start_fish(*ptr_fish);

    // `Fish0`.
    local_ret |= myassert(
        fish__is_started(*aqua__get_fish(array_name_fishes[0], aqua)) == 1,
        "Testing get_fish and mobility. `Fish0` should be started\n");

    local_ret |= myassert(
        fish__get_move_duration(*aqua__get_fish(array_name_fishes[0], aqua)) <= 5,
        "Testing get_fish and mobility. `Fish0` should not have reached its goal, duration 5sec.\n");

    struct vec2 vec_target = fish__get_target_pos(*aqua__get_fish(array_name_fishes[0], aqua));
    local_ret |= myassert(
        vec_target.x == 100 && vec_target.y == 100,
        "Testing get_fish and mobility. `Fish0` should not have reached its goal, (100, 100).\n");

    // `Fish1`.
    local_ret |= myassert(
        fish__is_started(*aqua__get_fish(array_name_fishes[1], aqua)) == 1,
        "Testing get_fish and mobility. `Fish1` should be started\n");

    local_ret |= myassert(
        fish__get_move_duration(*aqua__get_fish(array_name_fishes[1], aqua)) <= 10,
        "Testing get_fish and mobility. `Fish1` should not have reached its goal, duration 10sec.\n");

    vec_target = fish__get_target_pos(*aqua__get_fish(array_name_fishes[1], aqua));
    local_ret |= myassert(
        vec_target.x == 0 && vec_target.y == 0,
        "Testing get_fish and mobility. `Fish1` should not have reached its goal, (0, 0).\n");

    // `Fish2`.
    local_ret |= myassert(
        fish__is_started(*aqua__get_fish(array_name_fishes[2], aqua)) == 0,
        "Testing get_fish and mobility. `Fish2` should not be started\n");

    local_ret |= myassert(
        fish__get_move_duration(*aqua__get_fish(array_name_fishes[2], aqua)) == 0,
        "Testing get_fish and mobility. `Fish2` should not have a goal, duration 0sec.\n");

    vec_target = fish__get_target_pos(*aqua__get_fish(array_name_fishes[2], aqua));
    local_ret |= myassert(
        vec_target.x == 0 && vec_target.y == 0,
        "Testing get_fish and mobility. `Fish2` should not have a goal, (0, 0).\n");

    // `Fish3`.
    local_ret |= myassert(
        fish__is_started(*aqua__get_fish(array_name_fishes[3], aqua)) == 0,
        "Testing get_fish and mobility. `Fish3` should not be started\n");

    local_ret |= myassert(
        fish__get_move_duration(*aqua__get_fish(array_name_fishes[3], aqua)) == 0,
        "Testing get_fish and mobility. `Fish3` should not have a goal, duration 0sec.\n");

    vec_target = fish__get_target_pos(*aqua__get_fish(array_name_fishes[3], aqua));
    local_ret |= myassert(
        vec_target.x == 100 && vec_target.y == 100,
        "Testing get_fish and mobility. `Fish3` should not have a goal, (100, 100).\n");

    // sleep 5 seconds. `Fish0` should have reached his goal, but not `Fish1`.
    sleep(5);

    // `Fish0`.
    struct vec2 vec_current = fish__get_current_pos(*aqua__get_fish(array_name_fishes[1], aqua));
    local_ret |= myassert(
        vec_current.x == 100 && vec_current.y == 100,
        "testing get_fish and mobility. `fish0` should have reached its goal, current pos: (100, 100), after 5seconds.\n");

    local_ret |= myassert(
        fish__get_move_duration(*aqua__get_fish(array_name_fishes[0], aqua)) <= 10,
        "Testing get_fish and mobility. `Fish0` should have reached its goal, new duration: 10sec, after 5seconds.\n");

    vec_target = fish__get_target_pos(*aqua__get_fish(array_name_fishes[0], aqua));
    local_ret |= myassert(
        vec_target.x == 0 && vec_target.y == 0,
        "Testing get_fish and mobility. `Fish0` should have reached its goal, new goal: (0, 0), after 5seconds.\n");

    // `Fish1`.
    local_ret |= myassert(
        fish__get_move_duration(*aqua__get_fish(array_name_fishes[1], aqua)) <= 10,
        "Testing get_fish and mobility. `Fish1` should not have reached its goal, duration 10sec, after 5seconds.\n");

    vec_target = fish__get_target_pos(*aqua__get_fish(array_name_fishes[1], aqua));
    local_ret |= myassert(
        vec_target.x == 0 && vec_target.y == 0,
        "Testing get_fish and mobility. `Fish1` should not have reached its goal, (0, 0), after 5seconds.\n");

    // sleep 5 seconds. `Fish0` should not have reached his goal, but `Fish1` should.
    sleep(5);

    // `Fish0`.
    local_ret |= myassert(
        fish__get_move_duration(*aqua__get_fish(array_name_fishes[0], aqua)) <= 10,
        "Testing get_fish and mobility. `Fish0` should not have reached its goal, 10sec, after 10seconds.\n");

    vec_target = fish__get_target_pos(*aqua__get_fish(array_name_fishes[0], aqua));
    local_ret |= myassert(
        vec_target.x == 0 && vec_target.y == 0,
        "Testing get_fish and mobility. `Fish0` should not have reached its goal, (0, 0), after 10seconds.\n");

    // `Fish1`.
    vec_current = fish__get_current_pos(*aqua__get_fish(array_name_fishes[1], aqua));
    local_ret |= myassert(
        vec_current.x == 0 && vec_current.y == 0,
        "testing get_fish and mobility. `Fish1` should have reached its goal, current pos: (0, 0), after 10seconds.\n");

    local_ret |= myassert(
        fish__get_move_duration(*aqua__get_fish(array_name_fishes[1], aqua)) <= 5,
        "Testing get_fish and mobility. `Fish1` should have reached its goal, new duration: 5sec, after 10seconds.\n");

    vec_target = fish__get_target_pos(*aqua__get_fish(array_name_fishes[1], aqua));
    local_ret |= myassert(
        vec_target.x == 100 && vec_target.y == 100,
        "Testing get_fish and mobility. `Fish1` should have reached its goal, new goal: (100, 100), after 10seconds.\n");

    aqua__destroy_aqua(&aqua);
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

    for (int i = 0; i < nb_fishes; ++i) {
        free(array_name_fishes[i]);
    }
    free(array_name_fishes);

    return ret;
}
