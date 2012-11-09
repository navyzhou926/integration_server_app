#include <unistd.h>
#include <stdio.h>
#include "matrix_control.h"
#include "net.h"
#include "serial_common.h"
#include "uart.h"
#include "global.h"
#include "net_server.h"


#define DEBUG
#ifdef DEBUG
#define printf_debug(fmt, arg...) printf(fmt, ##arg)
#else
#define printf_debug(fmt, arg...) do{}while(0)
#endif

#define MATRIX_UART_MAX_SEND_SIZE   4
#define MATRIX_CANCEL_SWITCHOVER_IN_CHANNEL     0xFF

#define CURRENT_MATRIX_MAX_OUT_CHANNEL_NUM  4

unsigned char matrix_switchover_handshake_code[] = {0xBB, 0x00};

matrix_control_arg matrix_control_data = {0, MATRIX_CONTROL_NO_VALID_COMMAND, 0, 0, {MATRIX_SERIAL_PORT,{0,0,0},{LINK_DEV_MATRIX,1,MATRIX_PROTOCOL_PELCO},{MATRIX_UART_BOARD,MATRIX_UART_DATA_BIT,MATRIX_UART_STOP_BIT,MATRIX_UART_CHECK_BIT,0,{0,0}},{0}}};


extern void print_string(char *string, unsigned char *buffer, unsigned int len);
int matrix_handshake_and_setup(int *com_fd);

void *pthread_matrix_control(void *arg)
{
    int com_fd;
    int recv_ret = 0;
    //unsigned char send_buffer[] = {0xBB, 0x00, 0x06, 0x02};

    while (1) 
    {
        com_fd = OpenDev(MATRIX_UART_DEVICE);
        if (com_fd == -1) 
        {
            printf("FUNC[%s] LINE[%d]\tOpen uart failed!\n",__FUNCTION__, __LINE__);
            sleep(10);
            continue;
        }
        //fd nSpeed nBits nEvent nStop)
        //recv_ret = set_opt(com_fd, 9600, 3, 0, 0); //for matrix 
        recv_ret = set_opt(com_fd, MATRIX_UART_BOARD, MATRIX_UART_DATA_BIT, MATRIX_UART_CHECK_BIT, MATRIX_UART_STOP_BIT);
        if (recv_ret == -1) 
        {
            printf("FUNC[%s] LINE[%d]\tInit uart failed!\n",__FUNCTION__, __LINE__);
            sleep(10);
            continue;
        }
        ClrCom(com_fd);

        #if 0
        while (1) 
        {
            send_buffer[2] = 0x07;
            printf("1\n");
            write(com_fd, send_buffer, 4);
            sleep(2);
        }
        #endif 
        recv_ret = matrix_handshake_and_setup(&com_fd);
        if (recv_ret == -1) 
        {
            close(com_fd);
            sleep(10);
        }
    }

    return (void *)1;
}

int matrix_handshake_and_setup(int *com_fd)
{
    int i = 0;
    unsigned char *send_buffer = NULL;

    //matrix_control_data.setup_command_set = MATRIX_SWITCHOVER_COMMAND;
    //matrix_control_data.matrix_current_switchover_in_value = 0xff;
    //matrix_control_data.matrix_current_switchover_out_value = 0x00;

    while (1) 
    {
        switch(matrix_control_data.setup_command_set)
        {
            case MATRIX_CONTROL_NO_VALID_COMMAND:
                //printf_debug("FUNC[%s] LINE[%d]\tNo control command!\n",__FUNCTION__, __LINE__);
                usleep(500000);
                break;
            case MATRIX_SWITCHOVER_COMMAND:  //����ͨ���л�
                send_buffer = matrix_switchover_handshake_code;
                send_buffer[2] = matrix_control_data.matrix_current_switchover_in_value;
                send_buffer[3] = matrix_control_data.matrix_current_switchover_out_value;
                if (SendDataToCom(*com_fd, send_buffer, MATRIX_UART_MAX_SEND_SIZE) == -1)
                {
                    printf("FUNC[%s] LINE[%d]\tSend data to com error!\n",__FUNCTION__, __LINE__);
                    return -1;
                }
                matrix_control_data.setup_command_set = MATRIX_CONTROL_NO_VALID_COMMAND;
                #ifdef DEBUG
                print_string(" ", send_buffer, MATRIX_UART_MAX_SEND_SIZE);
                #endif
                //navy ���緢�;����л��ɹ�
                printf_debug("FUNC[%s] LINE[%d]\tMatrix succeed to switchover!\n",__FUNCTION__, __LINE__);

		  //���ͱ��������Ƴɹ�״̬�ظ�-------------------------------------------------------------12.9.6  frank added
		  send_act_cmd(QULIFIED,matrix_control_data.current_fd,SXC_EXCHAGE_MATRIX_INOUT,matrix_control_data.current_fd);
		  
                break;
            case MATRIX_CANCEL_SWITCHOVER_COMMAND:  //ȡ����ͨ���л�
                send_buffer = matrix_switchover_handshake_code;
                send_buffer[2] = MATRIX_CANCEL_SWITCHOVER_IN_CHANNEL;
                send_buffer[3] = matrix_control_data.matrix_current_switchover_out_value;
                if (SendDataToCom(*com_fd, send_buffer, MATRIX_UART_MAX_SEND_SIZE) == -1)
                {
                    printf("FUNC[%s] LINE[%d]\tSend data to com error!\n",__FUNCTION__, __LINE__);
                    return -1;
                }
                matrix_control_data.setup_command_set = MATRIX_CONTROL_NO_VALID_COMMAND;
                #ifdef DEBUG
                print_string(" ", send_buffer, MATRIX_UART_MAX_SEND_SIZE);
                #endif
                //navy ���緢�;���ȡ���л��ɹ�
                printf_debug("FUNC[%s] LINE[%d]\tMatrix succeed to cancel switchover!\n",__FUNCTION__, __LINE__);

		  //���ͱ��������Ƴɹ�״̬�ظ�-------------------------------------------------------------12.9.6  frank added
		  send_act_cmd(QULIFIED,matrix_control_data.current_fd,SXC_CANCEL_MATRIX_SINGLE_INOUT,matrix_control_data.current_fd);
		  
                break;
            case MATRIX_CANCEL_ALL_SWITCHOVER_COMMAND:  //ȡ������ͨ���л�
                send_buffer = matrix_switchover_handshake_code;
                send_buffer[2] = MATRIX_CANCEL_SWITCHOVER_IN_CHANNEL;
                for (i = 0; i < CURRENT_MATRIX_MAX_OUT_CHANNEL_NUM; i++) 
                {
                    send_buffer[3] = i;
                    if (SendDataToCom(*com_fd, send_buffer, MATRIX_UART_MAX_SEND_SIZE) == -1)
                    {
                        printf("FUNC[%s] LINE[%d]\tSend data to com error!\n",__FUNCTION__, __LINE__);
                        return -1;
                    }
                    #ifdef DEBUG
                    print_string(" ", send_buffer, MATRIX_UART_MAX_SEND_SIZE);
                    #endif
                    usleep(10000);
                }
                matrix_control_data.setup_command_set = MATRIX_CONTROL_NO_VALID_COMMAND;
                //navy ���緢�;���ȡ���л��ɹ�
                printf_debug("FUNC[%s] LINE[%d]\tMatrix succeed to cancel all switchover!\n",__FUNCTION__, __LINE__);

		  //���ͱ��������Ƴɹ�״̬�ظ�-------------------------------------------------------------12.9.6  frank added
		  send_act_cmd(QULIFIED,matrix_control_data.current_fd,SXC_CANCEL_MATRIX_ALL_INOUT,matrix_control_data.current_fd);
		  
                break;
            default :
                printf_debug("FUNC[%s] LINE[%d]\tInvalid matrix control command!\n",__FUNCTION__, __LINE__);
                matrix_control_data.setup_command_set = MATRIX_CONTROL_NO_VALID_COMMAND;
                break;
        }
    }
}

