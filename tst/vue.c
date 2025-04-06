#include "vue.h"
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

int test_vue__init_vue(void)
{
    struct vue_t vue = vue__init_vue(2, vec2__zeros(), vec2__ones());

    int local_ret = 0;

    local_ret |= myassert(
        vue__get_id(vue) == 2,
        "vue__get_id(vue__init_fish(id=2)) should be 2\n");

    char cur_pos[VEC2_DISP_BUFFER_SIZE];
    vec2__disp(vue__get_current_pos(vue), cur_pos, VEC2_DISP_BUFFER_SIZE);
    local_ret |= myassert(
        strcmp(cur_pos, "(0, 0)") == 0,
        "vue__get_current_pos(vue__init_vue((0,0))) should be (0,0)\n");

    return local_ret;
}

int test_vue__set_current_pos(void)
{
    struct vue_t vue = vue__init_vue(2, vec2__zeros(), vec2__ones());

    int local_ret = 0;

    char cur_pos[VEC2_DISP_BUFFER_SIZE];
    vec2__disp(vue__get_current_pos(vue), cur_pos, VEC2_DISP_BUFFER_SIZE);
    local_ret |= myassert(
        strcmp(cur_pos, "(0, 0)") == 0,
        "vue__get_current_pos(vue__init_vue((0,0))) should be (0,0)\n");

    long n = 256;
    char dst[n];
    printf("%s", vue__disp(vue, dst, n));

    vue = vue__set_current_pos(vec2__ones(), vue);
    vec2__disp(vue__get_current_pos(vue), cur_pos, VEC2_DISP_BUFFER_SIZE);
    local_ret |= myassert(
        strcmp(cur_pos, "(1, 1)") == 0,
        "vue__get_current_pos(vue__set_vue((1,1))) should be (1,1)\n");
    printf("%s", vue__disp(vue, dst, n));

    vue = vue__set_current_pos(
        vec2__sub(vec2__ones(), vue__get_current_pos(vue)),
        vue);
    vec2__disp(vue__get_current_pos(vue), cur_pos, VEC2_DISP_BUFFER_SIZE);
    local_ret |= myassert(
        strcmp(cur_pos, "(0, 0)") == 0,
        "vue__get_current_pos(vue__sub_vue((1,1)-(0,0))) should be (0,0)\n");
    printf("%s", vue__disp(vue, dst, n));

    return local_ret;
}

int test_vue__set_width_height(void)
{
    struct vue_t vue = vue__init_vue(2, vec2__zeros(), vec2__ones());

    int local_ret = 0;

    char cur_pos[VEC2_DISP_BUFFER_SIZE];
    vec2__disp(vue__get_width_height(vue), cur_pos, VEC2_DISP_BUFFER_SIZE);
    local_ret |= myassert(
        strcmp(cur_pos, "(1, 1)") == 0,
        "vue__get_width_height(vue__init_vue(s=(1,1))) should be (1,1)\n");

    vue = vue__set_width_height(
        vec2__add(vec2__ones(), vec2__ones()),
        vue);
    vec2__disp(vue__get_width_height(vue), cur_pos, VEC2_DISP_BUFFER_SIZE);
    local_ret |= myassert(
        strcmp(cur_pos, "(2, 2)") == 0,
        "vue__get_width_height(vue__set_vue(s=(2,2))) should be (2,2)\n");

    vue = vue__set_width_height(
        vec2__sub(
            vec2__sub(vec2__ones(), vec2__ones()),
            vue__get_current_pos(vue)),
        vue);
    vec2__disp(vue__get_width_height(vue), cur_pos, VEC2_DISP_BUFFER_SIZE);
    local_ret |= myassert(
        strcmp(cur_pos, "(0, 0)") == 0,
        "vue__get_width_height(vue__set_vue(s=(0,0))) should be (0,0)\n");

    return local_ret;
}

int main(int argc, char* argv[])
{
    srand(time(NULL));
    int ret = 0;

    ret |= test_vue__init_vue();
    ret |= test_vue__set_current_pos();
    ret |= test_vue__set_width_height();

    return ret;
}
