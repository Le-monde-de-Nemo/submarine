#ifndef __FISH__H__
#define __FISH__H__

    // ----------------------------------------------------------------------
    // The FISH structure!
    // This one contains 2 type of fish :
    //      - `PoissonRouge`
    //      - `PoissonClown`
    //      - `COMMON` = -1, by default
    // ----------------------------------------------------------------------

    enum Species {
        POISSON_ROUGE, // 0
        POISSON_CLOWN, // 1
        COMMON = -1
    };

    // ----------------------------------------------------------------------

    struct Fish {
        enum Species specie;
        // TODO
    };

    // ----------------------------------------------------------------------
    // Create a fish.
    // ----------------------------------------------------------------------

    // TODO
    struct Fish* fish__init_fish();

    // ----------------------------------------------------------------------
    // By default, the type of the fish is COMMON.
    // ----------------------------------------------------------------------

    enum Species fish__getType(struct Fish fish);
    enum Species fish__setType(struct Fish fish);

    // ----------------------------------------------------------------------
    // Remove the allocated area from a fish creation.
    // ----------------------------------------------------------------------

    // TODO
    int fish__destroy_fish(struct Fish* ptr_fish);

    // ----------------------------------------------------------------------

#endif // __FISH__H__
