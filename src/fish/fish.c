// --------------------------------------------------------------------------
// The implementation of the FISH structure.
//      See `/src/fish/fish.h`.
//      Dependencies:
//              - `vec2.h` for the coordinates.
// --------------------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>

#include "fish.h"
#include "vec2.h"

// --------------------------------------------------------------------------

struct Fish fish__init_fish()
{
    struct Fish fish;
    fish.specie = COMMON;
    fish.coordinates = vec2__zeros();

    return fish;
}

// --------------------------------------------------------------------------

enum Species fish__getType(const struct Fish* fish)
{
    // TODO
    return COMMON;
}

struct Fish* fish__setType(const enum Species specie, struct Fish* fish)
{
    // TODO
    return fish;
}

// --------------------------------------------------------------------------

struct vec2 fish__getPos(const struct Fish* fish)
{
    // TODO
    return vec2__zeros();
}

struct Fish* fish__setPos(const struct vec2 pos, struct Fish* fish)
{
    // TODO
    return fish;
}

// --------------------------------------------------------------------------

int fish__destroy_fish(struct Fish* ptr_fish)
{
    // TODO
    return 0;
}

// --------------------------------------------------------------------------

#ifdef DEBUG_FISH
int main(int argc, char** argv)
{
    return 0;
}
#endif // DEBUG_FISH
