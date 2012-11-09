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
    /* unsigned int setup_command_set ����
    �ֽ�4������(���ֽ�)
    �ֽ�3������
    �ֽ�2������
    �ֽ�1��
    2-7������
    1������ȡ���л�
    0�������л�         */
    unsigned int setup_command_set;         //�洢��������(��16λ��ÿһλ��ʾһ������) 
    unsigned char matrix_current_switchover_in_value;  //�洢������Ƶ����ͨ����
    unsigned char matrix_current_switchover_out_value; //�洢������Ƶ���ͨ����
    SERIAL_PAMATER matrix_control_serial_pamater;
}matrix_control_arg;

matrix_control_arg matrix_control_data;

void *pthread_matrix_control(void *arg);

#endif
