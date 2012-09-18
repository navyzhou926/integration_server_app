#ifndef _MATRIX_CONTROL_H
#define _MATRIX_CONTROL_H

#include "net_server.h"

#define MATRIX_CONTROL_NO_VALID_COMMAND         0x00
#define MATRIX_SWITCHOVER_COMMAND                   0x01
#define MATRIX_CANCEL_SWITCHOVER_COMMAND            0x02
#define MATRIX_CANCEL_ALL_SWITCHOVER_COMMAND         0x03

#define MATRIX_SERIAL_PORT          1
#define MATRIX_UART_BOARD           9600
#define MATRIX_UART_DATA_BIT        3  //8bit
#define MATRIX_UART_CHECK_BIT       0
#define MATRIX_UART_STOP_BIT        0

typedef struct 
{
    int current_fd;
    /* unsigned int setup_command_set 介绍
    字节4：保留(高字节)
    字节3：保留
    字节2：保留
    字节1：
    2-7：保留
    1：矩阵取消切换
    0：矩阵切换         */
    unsigned int setup_command_set;         //存储设置命令(低16位，每一位表示一个命令) 
    unsigned char matrix_current_switchover_in_value;  //存储矩阵视频输入通道号
    unsigned char matrix_current_switchover_out_value; //存储矩阵视频输出通道号
    SERIAL_PAMATER matrix_control_serial_pamater;
}matrix_control_arg;

matrix_control_arg matrix_control_data;

void *pthread_matrix_control(void *arg);

#endif
