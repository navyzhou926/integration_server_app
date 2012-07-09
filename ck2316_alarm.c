#include <unistd.h>
#include <stdio.h>
#include <time.h>
#include "net.h"

//#define DEBUG
#ifdef DEBUG
#define printf_debug(fmt, arg...) printf(fmt, ##arg)
#else
#define printf_debug(fmt, arg...) do{}while(0)
#endif

void *pthread_ck2316_alarm(void *arg)
{
    char time_buffer[100];
    time_t timer;
    struct tm *tblock;

    timer = time(NULL);
    tblock = localtime(&timer);
    sprintf(time_buffer, "%s", asctime(tblock));

    while (1) 
    {
    #ifdef DEBUG
        timer = time(NULL);
        tblock = localtime(&timer);
        printf("Start Time:\t%s",time_buffer);
        printf("Now Time:\t%s",asctime(tblock));
        usleep(1000);
        //printf("ck2316 alarm\n");
    #endif
    }

    return (void *)1;
}
