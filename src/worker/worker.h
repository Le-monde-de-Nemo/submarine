#ifndef WORKER_H
#define WORKER_H

/**
 * Provides the worker function used for interpreting the
 * commands sent from the viewer to the controller
 */

/*
 * :param args: an integer, it is a file descriptor.
 * `worker` will write in that file descriptor.
 * :return: nothing.
 */
void* worker(void* args);

#endif