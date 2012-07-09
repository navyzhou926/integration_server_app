#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "net.h"
#include "uart.h"

#define DEBUG
#ifdef DEBUG
#define printf_debug(fmt, arg...) printf(fmt, ##arg)
#else
#define printf_debug(fmt, arg...) do{}while(0)
#endif

void *pthread_entrance_guard(void *arg)
{
    /*
    while (1) 
    {
        usleep(10000);
        printf("entrance guard\n");
    }
    */
    int com_fd;
    int recv_ret = 0, send_ret = 0;
    unsigned int recv_size = 0;
    unsigned char init_array[] = "Start to uart test\r\n";
    unsigned char recv_buffer[1024] = {0};
    
    com_fd = InitCom(UART_DEVICE_ttyS1, BOARD_RATE);
    //com_fd = InitCom(UART_DEVICE_ttyUSB0, BOARD_RATE);
    if (com_fd == -1) 
    {
        printf("Init uart failed !\r\n");
        exit(1);
    }
    SendDataToCom(com_fd, init_array, strlen(init_array));

    while (1) 
    {
        recv_size = MAX_RECV_SIZE;
        recv_ret = RecvDataFromCom(com_fd, recv_buffer, &recv_size, RECV_TIMEOUT);
        switch (recv_ret)
        {
            case 0:
                #if 0
                if (strncmp(recv_buffer, "quit", 4) == 0) 
                {
                    exit(0);
                }
                #endif
                SendDataToCom(com_fd, recv_buffer, recv_size);
                if (send_ret == -1) 
                {
                    printf_debug("Send data to com error!\n");                   
                    break;
                    //exit(1);
                }
                //SendDataToCom(com_fd, "\r\n", 2);
                break;
            case -1:
                if (recv_size == 0) 
                {
                    printf("Timeout, didn't recv any data !\r\n");
                }
                else
                {
                #if 0
                    if (strncmp(recv_buffer, "quit", 4) == 0) 
                    {
                        exit(0);
                    }
                #endif
                    //printf_debug("recv_size = %d\r\n",recv_size);
                    SendDataToCom(com_fd, recv_buffer, recv_size);
                    if (send_ret == -1) 
                    {
                        printf_debug("Send data to com error!\n");                   
                        break;
                        //exit(1);
                    }
                    //SendDataToCom(com_fd, "\r\n", 2);
                }
                break;
            case -2:
                printf("Recv data error!\r\n");
                exit(1);
            default: 
                break;
        }
    }

    return (void *)1;
}
