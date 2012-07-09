#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include "global.h"
#include "net.h"
#include "uart.h"
#include "entrance_guard.h"

#define DEBUG
#ifdef DEBUG
#define printf_debug(fmt, arg...) printf(fmt, ##arg)
#else
#define printf_debug(fmt, arg...) do{}while(0)
#endif

pthread_mutex_t entrance_guard_mutex = PTHREAD_MUTEX_INITIALIZER;

entrance_guard_arg entrance_guard_data = {NO, NO, NO, IS_CLOSED};

//host code
unsigned char search_entrance_guard_code[] = {0xfa,0xfa,0xfa,0x2a,0x01,0x00,0x02,0xcc,0x57};
unsigned char open_door_code[] = {0xfa,0xfa,0xfa,0x2a,0x03,0x00,0x63,0x20,0xc2,0x6e,0x1b};
unsigned char close_door_code[] = {0xfa,0xfa,0xfa,0x2a,0x03,0x00,0x63,0x20,0xc3,0x7e,0x3a};

//entrance guard code
unsigned char return_search_entrance_guard_code[] = {0xfa,0xfa,0xfa,0x2a,0x03,0x00,0x63,0x20,0xc3,0x7e,0x3a};
unsigned char return_setup_success_code[] = {0xfa,0xfa,0xfa,0x2a,0x01,0x00,0x81,0x6d,0xbc};

void *pthread_entrance_guard_handshake(void *arg);

void *pthread_entrance_guard(void *arg)
{
    int com_fd;
    int recv_ret = 0;
    unsigned int recv_size = 0;
    unsigned char recv_buffer[1024] = {0};
    pthread_t tid_handshake;
    
    com_fd = InitCom(UART_DEVICE_ttyS1, BOARD_RATE);
    //com_fd = InitCom(UART_DEVICE_ttyUSB0, BOARD_RATE);
    if (com_fd == -1) 
    {
        printf("FUNC[%s] LINE[%d]\tInit uart failed!\n",__FUNCTION__, __LINE__);
        exit(1);
    }
    //SendDataToCom(com_fd, init_array, strlen(init_array));

    if (pthread_create(&tid_handshake, NULL, pthread_entrance_guard_handshake, &com_fd) != 0) 
    {
        printf("FUNC[%s] LINE[%d]\tCan't create entrance guard handshake thread !\n",__FUNCTION__, __LINE__);
        exit(1);
    }

    while (1) 
    {
        pthread_mutex_lock(&entrance_guard_mutex);
        if (entrance_guard_data.if_open_door == YES) 
        {
            if (SendDataToCom(com_fd, open_door_code, 11) == -1)
            {
                printf("FUNC[%s] LINE[%d]\tSend data to com error!\n",__FUNCTION__, __LINE__);
                break;
                //exit(1);
            }
        }
        if (entrance_guard_data.if_close_door == YES) 
        {
            if (SendDataToCom(com_fd, close_door_code, 11) == -1)
            {
                printf("FUNC[%s] LINE[%d]\tSend data to com error!\n",__FUNCTION__, __LINE__);
                break;
                //exit(1);
            }
        }

        if (entrance_guard_data.if_open_door + entrance_guard_data.if_close_door) 
        {
            //recv_size = MAX_RECV_SIZE;
            recv_size = 9;
            recv_ret = RecvDataFromCom(com_fd, recv_buffer, &recv_size, RECV_TIMEOUT);
            switch (recv_ret)
            {
                case 0:
                    if (strcmp(return_setup_success_code, recv_buffer) == 0) 
                    {
                        if (entrance_guard_data.if_open_door == YES) 
                        {
                            entrance_guard_data.door_status = IS_OPENED;
                            entrance_guard_data.if_open_door = NO;
                            printf_debug("FUNC[%s] LINE[%d]\tThe door is opened!\n",__FUNCTION__, __LINE__);
                        }

                        if (entrance_guard_data.if_close_door == YES) 
                        {
                            entrance_guard_data.door_status = IS_CLOSED;
                            entrance_guard_data.if_close_door = NO;
                            printf_debug("FUNC[%s] LINE[%d]\tThe door is closed!\n",__FUNCTION__, __LINE__);
                        }
                    }
                    break;
                case -1:
                    if (recv_size == 0) 
                    {
                        //printf("Timeout, didn't recv any data !\r\n");
                        printf_debug("FUNC[%s] LINE[%d]\tTimeout, entrance guard is not alive !\n",__FUNCTION__, __LINE__);
                        entrance_guard_data.if_entrance_guard_alive = NO;
                    }
                    else
                    {
                        printf_debug("FUNC[%s] LINE[%d]\tDidn't recv enough data ! recv_size = %d\n",__FUNCTION__, __LINE__, recv_size);
                    }
                    break;
                case -2:
                    printf("FUNC[%s] LINE[%d]\tRecv data error!\n",__FUNCTION__, __LINE__);
                    exit(1);
                default: 
                    break;
            }
        }
        usleep(5000);
        pthread_mutex_unlock(&entrance_guard_mutex);
    }

    return (void *)1;
}

void *pthread_entrance_guard_handshake(void *arg)
{
    int recv_ret = 0;
    unsigned int recv_size = 0;
    unsigned char recv_buffer[1024] = {0};
    int com_fd = *(int *)arg;

    while (1) 
    {
        sleep(5);
        pthread_mutex_lock(&entrance_guard_mutex);
        if (SendDataToCom(com_fd, search_entrance_guard_code, 9) == -1)
        {
            printf("FUNC[%s] LINE[%d]\tSend data to com error!\n",__FUNCTION__, __LINE__);
            break;
            //exit(1);
        }
        recv_size = MAX_RECV_SIZE;
        recv_ret = RecvDataFromCom(com_fd, recv_buffer, &recv_size, RECV_TIMEOUT);
        switch (recv_ret)
        {
            case 0:
                if (strcmp(return_search_entrance_guard_code, recv_buffer) == 0) 
                {
                    printf_debug("FUNC[%s] LINE[%d]\tEntrance guard is alive !\n",__FUNCTION__, __LINE__);
                    entrance_guard_data.if_entrance_guard_alive = YES;
                }
                break;
            case -1:
                if (recv_size == 0) 
                {
                    //printf("Timeout, didn't recv any data !\r\n");
                    printf_debug("FUNC[%s] LINE[%d]\tTimeout, entrance guard is not alive !\n",__FUNCTION__, __LINE__);
                    entrance_guard_data.if_entrance_guard_alive = NO;
                }
                else
                {
                    printf_debug("FUNC[%s] LINE[%d]\tDidn't recv enough data ! recv_size = %d\n",__FUNCTION__, __LINE__, recv_size);
                }
                break;
            case -2:
                printf("FUNC[%s] LINE[%d]\tRecv data error!\n",__FUNCTION__, __LINE__);
                exit(1);
            default: 
                break;
        }
        pthread_mutex_unlock(&entrance_guard_mutex);
    }

    return (void *)1;
}
