#include "aqua.h"
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

int test__aqua_init_aqua(void)
{
    struct aqua_t aqua = aqua__init_aqua(vec2__ones());

    int local_ret = 0;

    local_ret |= myassert(
        aqua__get_id(aqua) == 0,
        "aqua__get_id(aqua__init_aqua(id=0)) should be 0\n");

    char cur_pos[VEC2_DISP_BUFFER_SIZE];
    vec2__disp(aqua__get_width_height(aqua), cur_pos, VEC2_DISP_BUFFER_SIZE);
    local_ret |= myassert(
        strcmp(cur_pos, "(1, 1)") == 0,
        "aqua__get_width_height(aqua__init_aqua(s=(1,1))) should be (1,1)\n");

    local_ret |= myassert(
        aqua__get_nb_fishes(aqua) == 0,
        "aqua__get_nb_fishes(init_aqua) should be 0\n");

    local_ret |= myassert(
        aqua__get_nb_vues(aqua) == 0,
        "aqua__get_nb_vues(init_aqua) should be 0\n");

    local_ret |= myassert(
        aqua__get_fishes(aqua) == NULL,
        "aqua__get_fishes(init_aqua) should be NULL\n");

    local_ret |= myassert(
        aqua__get_vues(aqua) == NULL,
        "aqua__get_vues(init_aqua) should be NULL\n");

    local_ret |= myassert(
        aqua__destroy_aqua(&aqua) == 1,
        "aqua__destroy_aqua(init_aqua) should be 1\n");

    return local_ret;
}

int main(int argc, char* argv[])
{
    srand(time(NULL));
    int ret = 0;

    ret |= test__aqua_init_aqua();

    return ret;
}
