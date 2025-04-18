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

int test__aqua_add_fish(void)
{
    struct aqua_t aqua = aqua__init_aqua(vec2__ones());
    int local_ret = 0;
    int i = 0;

    long n = 4096;
    char dst[n];
    printf("%s", aqua__disp(aqua, dst, n));
    dst[0] = '\0';

    printf("%s", aqua__disp_fishes(aqua, dst, n));
    dst[0] = '\0';

    for (i = 0; i < nb_fishes; ++i) {
        struct fish_t fish = fish__init_fish(
            array_name_fishes[i],
            vec2__zeros(),
            vec2__ones(),
            "RandomWayPoint");

        aqua = aqua__add_fish(fish, aqua);
        local_ret |= myassert(
            aqua__get_nb_fishes(aqua) == i + 1,
            "aqua__get_nb_fishes(init_aqua + i fish) should be i\n");
    }
    printf("%s\n", aqua__disp_fishes(aqua, dst, n));
    dst[0] = '\0';

    for (i = 0; i < nb_fishes; ++i) {
        aqua = aqua__del_fish(array_name_fishes[i], aqua);

        local_ret |= myassert(
            aqua__get_nb_fishes(aqua) == 100 - i - 1,
            "aqua__get_nb_fishes(init_aqua - i fish) should be i\n");
    }

    for (i = 0; i < nb_fishes; ++i) {
        struct fish_t* fish = aqua__get_fish(array_name_fishes[i], aqua);
        local_ret |= myassert(
            fish == NULL,
            "aqua__get_fish(i fish, NULL) should be NULL after deletion\n");
    }

    local_ret |= myassert(
        aqua__get_fishes(aqua) == NULL,
        "aqua__get_fishes(init_aqua) should be NULL\n");

    for (i = 0; i < nb_fishes; ++i) {
        struct fish_t fish = fish__init_fish(
            array_name_fishes[i],
            vec2__zeros(),
            vec2__ones(),
            "RandomWayPoint");

        aqua = aqua__add_fish(fish, aqua);
    }
    printf("%s\n", aqua__disp_fishes_without_eol(aqua, dst, n));
    dst[0] = '\0';

    struct fish_t* fish = aqua__get_fish(array_name_fishes[50], aqua);

    local_ret |= myassert(
        fish != NULL,
        "aqua__get_fish(50, aqua) should not be NULL\n");

    if (fish != NULL) {
        local_ret |= myassert(
            fish__get_id(*fish) == 50,
            "fish_id(aqua__get_fish)(50, aqua) should not be 50\n");
    }

    local_ret |= myassert(
        aqua__destroy_aqua(&aqua) == 1,
        "aqua__destroy_aqua(init_aqua) should be 1\n");

    aqua = aqua__init_aqua(vec2__ones());
    for (i = 0; i < 100; ++i) {
        struct fish_t fish = fish__init_fish(
            array_name_fishes[i],
            vec2__zeros(),
            vec2__ones(),
            "RandomWayPoint");

        aqua = aqua__add_fish(fish, aqua);
    }

    struct fish_t* array_fishes = aqua__get_fishes(aqua);
    for (i = 0; i < 100; ++i) {
        int id = fish__get_id(array_fishes[i]);

        local_ret |= myassert(
            0 <= id && id != BASIC_FISH_ID,
            "aqua__get_id(aqua__get_fishes(100 fishes))(i) should be  0<i\n");
    }
    free(array_fishes);

    local_ret |= myassert(
        aqua__destroy_aqua(&aqua) == 1,
        "aqua__destroy_aqua(init_aqua) should be 1\n");

    return local_ret;
}

int test__aqua_add_vue(void)
{
    struct aqua_t aqua = aqua__init_aqua(vec2__ones());
    int local_ret = 0;
    int i = 0;

    long n = 4096;
    char dst[n];
    printf("%s\n", aqua__disp(aqua, dst, n));
    dst[0] = '\0';

    for (i = 0; i < 100; ++i) {
        struct vue_t vue = vue__init_vue(i + 1, vec2__zeros(), vec2__ones());

        aqua = aqua__add_vue(vue, aqua);
        local_ret |= myassert(
            aqua__get_nb_vues(aqua) == i + 1,
            "aqua__get_nb_vues(init_aqua + i vue) should be i\n");
    }
    printf("%s\n", aqua__disp(aqua, dst, n));
    dst[0] = '\0';

    for (i = 0; i < 100; ++i) {
        aqua = aqua__del_vue(i + 1, aqua);

        local_ret |= myassert(
            aqua__get_nb_vues(aqua) == 100 - i - 1,
            "aqua__get_nb_vues(init_aqua - i vue) should be i\n");
    }

    for (i = 0; i < 100; ++i) {
        struct vue_t* vue = aqua__get_vue(i, aqua);
        local_ret |= myassert(
            vue == NULL,
            "aqua__get_vue(i vue, NULL) should be NULL after deletion\n");
    }

    local_ret |= myassert(
        aqua__get_vues(aqua) == NULL,
        "aqua__get_vues(init_aqua) should be NULL\n");

    for (i = 0; i < 100; ++i) {
        struct vue_t vue = vue__init_vue(i + 1, vec2__zeros(), vec2__ones());

        aqua = aqua__add_vue(vue, aqua);
    }

    struct vue_t* vue = aqua__get_vue(50, aqua);

    local_ret |= myassert(
        vue != NULL,
        "aqua__get_vue(50, aqua) should not be NULL\n");

    if (vue != NULL) {
        local_ret |= myassert(
            vue__get_id(*vue) == 50,
            "vue_id(aqua__get_vue)(50, aqua) should not be 50\n");
    }

    local_ret |= myassert(
        aqua__destroy_aqua(&aqua) == 1,
        "aqua__destroy_aqua(init_aqua) should be 1\n");

    aqua = aqua__init_aqua(vec2__ones());
    for (i = 0; i < 100; ++i) {
        struct vue_t vue = vue__init_vue(i + 1, vec2__zeros(), vec2__ones());

        aqua = aqua__add_vue(vue, aqua);
    }

    struct vue_t* array_vues = aqua__get_vues(aqua);
    for (i = 0; i < 100; ++i) {
        int id = vue__get_id(array_vues[i]);
        local_ret |= myassert(
            id <= 100 && 0 < id,
            "aqua__get_id(aqua__get_vues(100 vues))(i) should be i<=100, 0<i\n");
    }
    free(array_vues);

    local_ret |= myassert(
        aqua__destroy_aqua(&aqua) == 1,
        "aqua__destroy_aqua(init_aqua) should be 1\n");

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

    ret |= test__aqua_init_aqua();
    ret |= test__aqua_add_fish();
    ret |= test__aqua_add_vue();

    for (int i = 0; i < nb_fishes; ++i) {
        free(array_name_fishes[i]);
    }
    free(array_name_fishes);

    return ret;
}
