#ifndef _NETWORK_H_
#define _NETWORK_H_

void network_init(char* host, int portno);
void network_finalize();

void* workerth(void* args);

#endif // _NETWORK_H_
