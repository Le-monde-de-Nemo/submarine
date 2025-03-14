// --------------------------------------------------------------------------
// The implementation of the FISH structure.
// See `/src/fish/fish.h`.
// --------------------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>

#include "fish.h"

// --------------------------------------------------------------------------

struct Fish* fish__init_fish()
{
    return NULL;
}

// --------------------------------------------------------------------------

enum Species fish__getType(const struct Fish* fish)
{
    return COMMON;
}

enum Species fish__setType(struct Fish* fish)
{
    return COMMON;
}

// --------------------------------------------------------------------------

int fish__destroy_fish(struct Fish* ptr_fish)
{
    return 0;
}

// --------------------------------------------------------------------------

int main(int argc, char** argv)
{
    return 0;
}
