#include "controller.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CONF_BUF_SIZE 4096

enum conf_item {
    CONTROLLER_PORT,
    DISPLAY_TIMEOUT_VALUE,
    FISH_UPDATE_INTERVAL,
};

char* conf_item__disp(enum conf_item item)
{
    switch (item) {
    case CONTROLLER_PORT:
        return "controller-port";

    case DISPLAY_TIMEOUT_VALUE:
        return "display-timeout-value";

    case FISH_UPDATE_INTERVAL:
        return "fish-update-interval";
    }
}

struct controller_t controller__default()
{
    struct controller_t out = {
        .port = 5555,
        .display_timeout_value = 45,
        .fish_update_time = 1,
    };

    return out;
}

struct controller_t controller__load_conf(char* filepath)
{
    struct controller_t out = controller__default();
    char read_buf[CONF_BUF_SIZE] = {};
    char key[CONF_BUF_SIZE] = {};
    char value[CONF_BUF_SIZE] = {};

    char* ht_pos = NULL;
    char* eq_pos = NULL;

    FILE* file = fopen(filepath, "r");
    if (file == NULL) {
        fprintf(stderr, "The config file you gave does not exist!\n");
        exit(EXIT_FAILURE);
    }

    while (fgets(read_buf, CONF_BUF_SIZE, file) != NULL) {
        if ((ht_pos = strchr(read_buf, '#')))
            *ht_pos = '\0';

        if (!(eq_pos = strchr(read_buf, '=')))
            continue;

        sscanf(read_buf, "%s = %s", key, value);

        if (strcmp(key, conf_item__disp(CONTROLLER_PORT)) == 0)
            out.port = atoi(value);
        else if (strcmp(key, conf_item__disp(DISPLAY_TIMEOUT_VALUE)) == 0)
            out.display_timeout_value = atoi(value);
        else if (strcmp(key, conf_item__disp(FISH_UPDATE_INTERVAL)) == 0)
            out.fish_update_time = atoi(value);
    }

    fclose(file);

    return out;
}

char* controller__disp(char* dst, long n, struct controller_t controller)
{
    snprintf(dst, n,
        "controller-port = %d\n"
        "display-timeout-value = %d\n"
        "fish-update-interval = %d\n",
        controller.port, controller.display_timeout_value, controller.fish_update_time);
    return dst;
}

void controller__save_conf(char* filepath, struct controller_t controller)
{
    FILE* file = fopen(filepath, "w");

    // I'm not using controller__disp to not bother with buffer sizes
    fprintf(file, "controller-port = %d\n", controller.port);
    fprintf(file, "display-timeout-value = %d\n", controller.display_timeout_value);
    fprintf(file, "fish-update-interval = %d\n", controller.fish_update_time);

    fclose(file);
}
