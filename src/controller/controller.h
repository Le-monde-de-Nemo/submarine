#ifndef _CONTROLLER_H_
#define _CONTROLLER_H_

struct controller_t {
    int port;
    int display_timeout_value;
    int fish_update_time;
};

/*
 *  Returns default values for controller
 *
 *  Defautl values are :
 *      .port = 5555,
 *      .display_timeout_value = 45,
 *      .fish_update_time = 1,
 */
struct controller_t controller__default();

struct controller_t controller__load_conf(char* filepath);

char* controller__disp(char* dst, long n, struct controller_t controller);

void controller__save_conf(char* filepath, struct controller_t controller);
#endif // _CONTROLLER_H_
