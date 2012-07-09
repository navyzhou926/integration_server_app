#include <unistd.h>
#include <stdio.h>
#include "net.h"

//#define DEBUG
#ifdef DEBUG
#define printf_debug(fmt, arg...) printf(fmt, ##arg)
#else
#define printf_debug(fmt, arg...) do{}while(0)
#endif

void *pthread_matrix_control(void *arg)
{
    while (1) 
    {
        usleep(10000);
        //printf("matrix control\n");
    }

    return (void *)1;
}
