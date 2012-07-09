#ifndef _ENTRANCE_GUARD_H
#define _ENTRANCE_GUARD_H

#define IS_OPENED   1
#define IS_CLOSED   0

void *pthread_entrance_guard(void *arg);

typedef struct 
{
    char if_entrance_guard_alive;
    char if_open_door; 
    char if_close_door;
    unsigned char door_status;
}entrance_guard_arg;

extern entrance_guard_arg entrance_guard_data;

#endif
