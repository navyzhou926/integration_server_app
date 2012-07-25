#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include "ck2316_alarm.h"
#include "global.h"
#include "uart.h"
#include "net.h"

#define DEBUG
#ifdef DEBUG
#define printf_debug(fmt, arg...) printf(fmt, ##arg)
#else
#define printf_debug(fmt, arg...) do{}while(0)
#endif

#define CK2316_KEYBOARD_ADDRESS            0x0D
#define CK2316_KEYBOARD_ADDRESS_MASK       0x03
#define CK2316_KEYBOARD_ADDRESS_SIZE       2
unsigned char ck2316_keyboard_address_code[] = {0x0D, 0x03};

#define CK2316_HANDSHAKE_CODE_1            0xFE
#define CK2316_HANDSHAKE_CODE_2            0x04            
#define CK2316_HANDSHAKE_CODE_3            0x03
#define CK2316_HANDSHAKE_SIZE              3
unsigned char ck2316_handshake_code[] = {0xFE, 0x04, 0x03};

extern int server_sock_tcp, client_sock_tcp;
extern int if_have_net_client_connect;

extern void print_string(char *string, unsigned char *buffer, unsigned int len);
int ck2316_alarm_handshake_and_setup(int *com_fd);
int send_handshake_packet(int *com_fd);
int verify_ck2316_data(unsigned char *recv_buffer, unsigned int len);

void *pthread_ck2316_alarm(void *arg)
{
    int i = 0;
    int com_fd;
    int recv_ret = 0;
    unsigned char recv_buffer[128] = {0};
    unsigned char ch = 0;

    while (1) 
    {
        com_fd = InitCom(UART_DEVICE_ttyS1, CK2316_BOARD_RATE);
        if (com_fd == -1) 
        {
            printf("FUNC[%s] LINE[%d]\tInit uart failed!\n",__FUNCTION__, __LINE__);
            sleep(10);
        }

        #if 1  //navy test
        #if 0
        while (if_have_net_client_connect == NO) 
        {
            printf("no link\n");
            sleep(2);
        }
        send(client_sock_tcp, "start\n", 6, 0);
        sleep(3);
        #endif
        while (1) 
        {
        #if 1
            #if 0
            if ((recv_ret = read(com_fd, recv_buffer, 1)) > 0) 
                    printf("%02X ",recv_buffer[0]);
            #else
            //if ((recv_ret = read(com_fd, &recv_buffer, 128)) > 0) 
            if ((recv_ret = read(com_fd, recv_buffer, 10)) > 0) 
            {
                #if 1
                for (i = 0; i < recv_ret; i++) 
                {
                    printf("%02X ",recv_buffer[i]);
                }
                printf("\n");
                #else
                if (if_have_net_client_connect == YES) 
                {
                    send(client_sock_tcp, recv_buffer, recv_ret, 0);
                }
                #endif
            }
            #endif
        #else
            recv_ret = read_uart_data(com_fd, recv_buffer, 10); 
            switch (recv_ret)
            {
                case 0:
                case -1:
                    printf_debug("FUNC[%s] LINE[%d]\tTimeout!\n",__FUNCTION__, __LINE__);
                    break;
                case -2:
                    printf("FUNC[%s] LINE[%d]\tRecv data error!\n",__FUNCTION__, __LINE__);
                    break;
                default: 
                    #if 0
                    if (if_have_net_client_connect == YES) 
                    {
                        send(client_sock_tcp, recv_buffer, 1, 0);
                    }
                    else
                    #endif
                    {
                        //printf("%02x ",recv_buffer[0]);
                        print_string(" ", recv_buffer, recv_ret);
                    }
                    break;
            }
        #endif
        }
        #endif

        recv_ret = ck2316_alarm_handshake_and_setup(&com_fd);
        if (recv_ret == -1) 
        {
            close(com_fd);
            sleep(10);
        }
        #if 0
        else if (recv_ret == 0) 
        {
            goto SEARCH_ENTRANCE_GUARD;
        }
        #endif
    }

    return (void *)1;
}

#if 1
int ck2316_alarm_handshake_and_setup(int *com_fd)
{
    int recv_ret = 0;
    unsigned char recv_buffer[128] = {0};

    ClrCom(*com_fd);
    while (1) 
    {
        //recv_size = CK2316_HANDSHAKE_RECV_SIZE;
        //recv_ret = RecvDataFromCom(*com_fd, recv_buffer, &recv_size, CK2316_RECV_TIMEOUT, CK2316_RECV_FINISH_TIMEOUT);
        //memset(recv_buffer, 0, CK2316_MAX_RECV_SIZE);
        recv_ret = read_uart_data(*com_fd, recv_buffer, CK2316_MAX_RECV_SIZE); 
        switch (recv_ret)
        {
            case 0:
            case -1:
                printf_debug("FUNC[%s] LINE[%d]\tTimeout, ck2316 is not alive !\n",__FUNCTION__, __LINE__);
                break;
            case -2:
                printf("FUNC[%s] LINE[%d]\tRecv data error!\n",__FUNCTION__, __LINE__);
                return -1;
            default: 
                if (verify_ck2316_data(recv_buffer, recv_ret) == 0)
                {
                    SendDataToCom(*com_fd, ck2316_handshake_code, CK2316_HANDSHAKE_SIZE);
                    printf("\nOK\n");
                }
                else
                {
                    printf_debug("FUNC[%s] LINE[%d]\tVerify data error, recv_ret = %d\n",__FUNCTION__, __LINE__, recv_ret);
                }
                #ifdef DEBUG
                print_string(" ", recv_buffer, recv_ret);
                #endif
                break;
        }
        //ClrCom(*com_fd);
    }

    return 1;
}

int verify_ck2316_data(unsigned char *recv_buffer, unsigned int len)
{
    if (len < 2) 
    {
        return -1;
    }
    else if (len == 2) // 0D 03
    {
        if ((recv_buffer[0] == CK2316_KEYBOARD_ADDRESS) && (recv_buffer[1] == CK2316_KEYBOARD_ADDRESS_MASK)) 
        {
            return 0;
        }
        else
        {
            return -1;
        }
    }
    else if (len == 5) //FE 04 03 0D 03
    {
        if ((recv_buffer[0] == CK2316_HANDSHAKE_CODE_1) && (recv_buffer[1] == CK2316_HANDSHAKE_CODE_1) && (recv_buffer[2] == CK2316_HANDSHAKE_CODE_3) && (recv_buffer[3] == CK2316_KEYBOARD_ADDRESS) && (recv_buffer[4] == CK2316_KEYBOARD_ADDRESS_MASK)) 
        {
            return 0;
        }
        else
        {
            return -1;
        }
    }
    else  //FE 04 03 FF 02/09 *** 0D 03
    {
    #if 0
        if ((recv_buffer[0] == CK2316_HANDSHAKE_CODE_1) && (recv_buffer[1] == CK2316_HANDSHAKE_CODE_1) && (recv_buffer[2] == CK2316_HANDSHAKE_CODE_3)) 
    #else
        if ((recv_buffer[len-2] == CK2316_KEYBOARD_ADDRESS) && (recv_buffer[len-1] == CK2316_KEYBOARD_ADDRESS_MASK)) 
    #endif
        {
            return 0;
        }
        else
        {
            return -1;
        }
    }
}
#else
int ck2316_alarm_handshake_and_setup(int *com_fd)
{
    int recv_ret = 0;
    unsigned int recv_size = 0;
    unsigned char recv_buffer[512] = {0};

    ClrCom(*com_fd);
    while (1) 
    {
        recv_size = CK2316_HANDSHAKE_RECV_SIZE;
        recv_ret = RecvDataFromCom(*com_fd, recv_buffer, &recv_size, CK2316_RECV_TIMEOUT, CK2316_RECV_FINISH_TIMEOUT);
        switch (recv_ret)
        {
            case 0:
            case -1:
                if (recv_size == 0) 
                {
                    printf_debug("FUNC[%s] LINE[%d]\tTimeout, ck2316 is not alive !\n",__FUNCTION__, __LINE__);
                }
                else
                {
                    if (strcmp(recv_buffer + recv_size - 2, ck2316_keyboard_address_code, CK2316_KEYBOARD_ADDRESS_SIZE) == 0)
                    {
                        send_handshake_packet(com_fd);
                        printf("\nOK\n");
                    }
                    ClrCom(*com_fd);
                    #ifdef DEBUG
                    print_string("", recv_buffer, recv_size);
                    #endif
                }
                break;
            case -2:
                printf("FUNC[%s] LINE[%d]\tRecv data error!\n",__FUNCTION__, __LINE__);
                return -1;
            default: 
                break;
        }
        ClrCom(*com_fd);
    }

    return 1;
}
#endif

int send_handshake_packet(int *com_fd)
{
    if (SendDataToCom(*com_fd, ck2316_handshake_code, CK2316_HANDSHAKE_SIZE) == -1)
    {
        printf("FUNC[%s] LINE[%d]\tSend data to com error!\n",__FUNCTION__, __LINE__);
        return -1;
    }
    return 0;
}

