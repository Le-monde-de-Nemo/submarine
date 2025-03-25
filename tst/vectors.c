#include "vec2.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

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

int main(int argc, char* argv[])
{
    srand(time(NULL));
    int x = rand(), y = rand();
    int ret = 0;

    struct vec2 v = vec2__create(0, 0);
    ret |= myassert(v.x == 0 && v.y == 0, "vec2__create(0, 0) creates vec 0\n");

    v = vec2__create(x, y);
    ret |= myassert(v.x == x && v.y == y, "vec2__create(rand) creates random vec\n");

    v = vec2__zeros();
    ret |= myassert(v.x == 0 && v.y == 0, "vec2__zero creates vec 0\n");

    v = vec2__ones();
    ret |= myassert(v.x == 1 && v.y == 1, "vec2__ones creates vec 1\n");

    x = rand();
    v = vec2__rmul(x, vec2__ones());
    ret |= myassert(v.x == x && v.y == x, "vec2__rmul(rand(), vec2__ones) creates vec\n");

    v = vec2__add(vec2__ones(), vec2__zeros());
    ret |= myassert(v.x == 1 && v.y == 1, "vec2__add(0, 1) should return (1, 1)\n");

    x = rand() % (RAND_MAX / 2), y = rand() % (RAND_MAX / 2);
    int x1 = rand() % (RAND_MAX / 2), y1 = rand() % (RAND_MAX / 2);
    v = vec2__add(vec2__create(x, y), vec2__create(x1, y1));
    ret |= myassert(v.x == x + x1 && v.y == y + y1, "vec2__add(rand, rand) should return (x0 + x1, y0 + y1)\n");

    v = vec2__sub(vec2__ones(), vec2__ones());
    ret |= myassert(v.x == 0 && v.y == 0, "vec2__sub(1, 1) should return (0, 0)\n");

    x = rand() % (RAND_MAX / 2), y = rand() % (RAND_MAX / 2);
    x1 = rand() % (RAND_MAX / 2), y1 = rand() % (RAND_MAX / 2);
    v = vec2__sub(vec2__create(x, y), vec2__create(x1, y1));
    ret |= myassert(v.x == x - x1 && v.y == y - y1, "vec2__sub(rand, rand) should return (x0 - x1, y0 - y1)\n");
    return ret;
}
