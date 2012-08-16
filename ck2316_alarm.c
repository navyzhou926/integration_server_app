#include <stdio.h>
#include <time.h>
#include "ck2316_alarm.h"
#include "global.h"
#include "uart.h"
#include "net.h"
#include "serial_common.h"

//#define MAIN_TEST

#define DEBUG
#ifdef DEBUG
#define printf_debug(fmt, arg...) printf(fmt, ##arg)
#else
#define printf_debug(fmt, arg...) do{}while(0)
#endif

enum CK2316_DEFENCN_AREA_ALARM_STATUS_ARG
{
    NO_ALARM = 0,
    NORMAL_ALARM = 1,
    EMERGENCY_ALARM = 2,
    NORMAL_AND_EMERGENCY_ALARM = 3,
}CK2316_DEFENCN_AREA_ALARM_STATUS_VALUE;

#define MAX_CHECK_COMMAND_COUNT            6

#define CK2316_KEYBOARD_ADDRESS            0x0F
//#define CK2316_ACTUAL_KEYBOARD_ADDRESS     (CK2316_KEYBOARD_ADDRESS+1)
#define CK2316_KEYBOARD_ADDRESS_MASK       0x03
#define CK2316_KEYBOARD_ADDRESS_SIZE       2
unsigned char ck2316_keyboard_address_code[] = {CK2316_KEYBOARD_ADDRESS, CK2316_KEYBOARD_ADDRESS_MASK};
//unsigned char ck2316_actual_keyboard_address_code[] = {CK2316_ACTUAL_KEYBOARD_ADDRESS, CK2316_KEYBOARD_ADDRESS_MASK};

#define CK2316_HANDSHAKE_CODE_1            0xFE
#define CK2316_HANDSHAKE_CODE_2            0x04            
#define CK2316_HANDSHAKE_CODE_3            0x03
#define CK2316_HANDSHAKE_SIZE              3
unsigned char ck2316_handshake_code[] = {CK2316_HANDSHAKE_CODE_1, CK2316_HANDSHAKE_CODE_2, CK2316_HANDSHAKE_CODE_3};

#define CK2316_HIGH_DEFENCE_AREA_HEAD           0x02
#define CK2316_LOW_DEFENCE_AREA_HEAD            0x09
#define CK2316_DEFENCE_AREA_HEAD_POSITION       4      //FE 04 03 FF 02/09

#define CK2316_IS_DEFENCED_OR_ABANDONED_MASK    0x80
#define CK2316_DEFENCE_AREA_ALARM_STATUS_MASK   0x03
#define CK2316_HIGH_BYPASS_DEFENCE_AREA_MASK    0xff
#define CK2316_LOW_BYPASS_DEFENCE_AREA_MASK     0xff00

#define CK2316_USER_PASSWORD_1              0x01
#define CK2316_USER_PASSWORD_2              0x02
#define CK2316_USER_PASSWORD_3              0x03
#define CK2316_USER_PASSWORD_4              0x04

ck2316_alarm_arg ck2316_alarm_data = {NO, IS_ABANDONED, NO_ALARM, {0x00, 0x00}, {0x00, 0x00}, {0x00, 0x00}, 0, CK2316_KEYBOARD_ADDRESS, {CK2316_USER_PASSWORD_1, CK2316_USER_PASSWORD_2, CK2316_USER_PASSWORD_3,CK2316_USER_PASSWORD_4}};

unsigned char ck2316_keyboard_code[][3] = {{0xFE,0x06,0x00},{0xFE,0x06,0x01},{0xFE,0x06,0x02},{0xFE,0x06,0x03},{0xFE,0x06,0x04},{0xFE,0x06,0x05},{0xFE,0x06,0x06},{0xFE,0x06,0x07},{0xFE,0x06,0x08},{0xFE,0x06,0x09},{0xFE,0x06,0x0A},{0xFE,0x06,0x0B},{0xFE,0x06,0x0C},{0xFE,0x06,0x0D},{0xFE,0x06,0x0E},{0xFE,0x06,0x0F}};

#define CK2316_SYSTEM_DEFENCE_SIZE          (7-1)
unsigned char system_defence_code[][3] = {{0xFE,0x06,CK2316_USER_PASSWORD_1}, {0xFE,0x06,CK2316_USER_PASSWORD_2},{0xFE,0x06,CK2316_USER_PASSWORD_3},{0xFE,0x06,CK2316_USER_PASSWORD_4},{0xFE,0x06,0x0B},{0xFE,0x04,0x02},{0xFE,0x04,0x06},{0xFE,0x04,0x03}};

#define CK2316_SYSTEM_ABANDON_SIZE          (5-0)
unsigned char system_abandon_code[][3] = {{0xFE,0x06,CK2316_USER_PASSWORD_1}, {0xFE,0x06,CK2316_USER_PASSWORD_2},{0xFE,0x06,CK2316_USER_PASSWORD_3},{0xFE,0x06,CK2316_USER_PASSWORD_4},{0xFE,0x06,0x0B},{0xFE,0x04,0x03}};

#define CK2316_ELIMINATE_ALARM_MEMORY_SIZE  (3-0)
unsigned char eliminate_alarm_memory_code[][3] = {{0xFE,0x06,0x0A},{0xFE,0x06,0x01},{0xFE,0x06,0x0B},{0xFE,0x04,0x03}};

#define CK2316_BYPASS_DEFENCE_AREA_SIZE     (8)
unsigned char bypass_defence_area_code[][3] = {{0xFE,0x06,CK2316_USER_PASSWORD_1}, {0xFE,0x06,CK2316_USER_PASSWORD_2},{0xFE,0x06,CK2316_USER_PASSWORD_3},{0xFE,0x06,CK2316_USER_PASSWORD_4},{0xFE,0x06,0x0C},{0xFE,0x06,0x00},{0xFE,0x06,0x00},{0xFE,0x06,0x0B},{0xFE,0x04,0x03}};

#define CK2316_ALARM_HOST_RESET_SIZE        (8-0)
unsigned char alarm_host_reset_code[][3] = {{0xFE,0x06,CK2316_USER_PASSWORD_1}, {0xFE,0x06,CK2316_USER_PASSWORD_2},{0xFE,0x06,CK2316_USER_PASSWORD_3},{0xFE,0x06,CK2316_USER_PASSWORD_4},{0xFE,0x06,0x0A},{0xFE,0x06,0x06},{0xFE,0x06,0x08},{0xFE,0x06,0x0B},{0xFE,0x04,0x03}};

FILE *fp_ck2316_config_file = NULL;

extern int server_sock_tcp, client_sock_tcp;
extern int if_have_net_client_connect;

extern void print_string(char *string, unsigned char *buffer, unsigned int len);
int ck2316_alarm_handshake_and_setup(int *com_fd);
int send_handshake_packet(int *com_fd);
int verify_ck2316_data(unsigned char *recv_buffer, unsigned int len);
int ck2316_user_password_setup(unsigned int user_password[4]);
int ck2316_alarm_init(void);

#ifndef MAIN_TEST
void *pthread_ck2316_alarm(void *arg)
#else
int if_have_net_client_connect = NO;
void print_string(char *string, unsigned char *buffer, unsigned int len)
{
    int i = 0;

    printf("%s",string);
    for (i = 0; i < len; i++) 
    {
        printf("%02X ",buffer[i]);
    }
    printf("\n");
}
int main(int argc, const char *argv[])
#endif
{
    int com_fd;
    int recv_ret = 0;
    unsigned char recv_buffer[72] = {0};

    while (1) 
    {
        recv_ret = ck2316_alarm_init();
        if (recv_ret == -1) 
        {
            printf("FUNC[%s] LINE[%d]\tCK2316 alarm init failed!\n",__FUNCTION__, __LINE__);
            sleep(10);
        }
        else
            break;
    }

    while (1) 
    {
#if 0
        com_fd = InitCom_ck2316(UART_DEVICE_ttyS0, CK2316_BOARD_RATE);
#else
        //navy1
        com_fd = OpenDev(UART_DEVICE_ttyS0);
        //fd nSpeed nBits nEvent nStop)
        //set_opt(com_fd, 2400, 3, 2, 0); //for ck2316
        set_opt(com_fd, CK2316_UART_BOARD, CK2316_UART_DATA_BIT, CK2316_UART_CHECK_BIT, CK2316_UART_STOP_BIT);
        //set_opt(com_fd, 2400, 3, 0, 0);
#endif
        if (com_fd == -1) 
        {
            printf("FUNC[%s] LINE[%d]\tInit uart failed!\n",__FUNCTION__, __LINE__);
            sleep(10);
        }
        ClrCom(com_fd);

#if 0
        while (if_have_net_client_connect == NO) 
        {
            printf("no link\n");
            sleep(2);
        }
#endif
        #if 0
        while (1) 
        {
        #if 1
            recv_ret = read_uart_data(com_fd, recv_buffer, CK2316_MAX_RECV_SIZE, CK2316_RECV_TIMEOUT); 
            //recv_ret = read(com_fd, recv_buffer, 10);
            printf_debug("recv_ret = %d\t",recv_ret);
            if (recv_ret > 0) 
            {
                #ifdef DEBUG
                print_string("\n ", recv_buffer, recv_ret);
                #endif
                write(com_fd, ck2316_handshake_code, CK2316_HANDSHAKE_SIZE);
                //ClrCom(com_fd);
            }
            else
            {
                printf_debug("read data error\n");
            }
        #else
            write(com_fd, ck2316_handshake_code, CK2316_HANDSHAKE_SIZE);
        #endif
            //ClrCom(com_fd);
            sleep(1);
        }
        #endif

         recv_ret = ck2316_alarm_handshake_and_setup(&com_fd);
        if (recv_ret == -1) 
        {
            close(com_fd);
            sleep(10);
        }
    }

#ifndef MAIN_TEST
    return (void *)1;
#else
    return 0;
#endif
}

int ck2316_alarm_handshake_and_setup(int *com_fd)
{
    int i = 0;
    int recv_ret = 0;
    unsigned char recv_buffer[72] = {0};
    unsigned char temp_test[] = {0xaa, 0x55, 0xa5};
    int check_command_count = 0;
    int system_count = 0;
    int timeout_count = 0;
    int capture_address_count = 0;
    unsigned int ck2316_check_command_setup_successful_flag = 0;
    unsigned int before_defence_area_bypass_value = 0;

    //ck2316_alarm_data.setup_command_set = CK2316_SYSTEM_DEFENCE;
    //ck2316_alarm_data.setup_command_set = CK2316_SYSTEM_ABANDON;
    //ck2316_alarm_data.setup_command_set = CK2316_ELIMINATE_ALARM_MEMORY;

    //ck2316_alarm_data.setup_command_set = CK2316_BYPASS_DEFENCE_AREA;
    //bypass_defence_area_code[5][2] = 1;
    //bypass_defence_area_code[6][2] = 2;
    //ck2316_alarm_data.ck2316_defence_area_bypass_value = CK2316_BYPASS_DEFENCE_AREA;
    //bypass_defence_area_code[5][2] = 0;
    //bypass_defence_area_code[6][2] = 2;
    
    //ck2316_alarm_data.setup_command_set = CK2316_ALARM_HOST_RESET;
    //ck2316_alarm_data.setup_command_set = CK2316_SIMULATE_KEYBOARD_ADDRESS_SETUP;
    //ck2316_alarm_data.setup_command_set = CK2316_SIMULATE_KEYBOARD_PASSWORD_SETUP;

    while (1) 
    {
        recv_ret = read_uart_data(*com_fd, recv_buffer, CK2316_MAX_RECV_SIZE, CK2316_RECV_TIMEOUT); 
        switch (recv_ret)
        {
            case 0:
                if (capture_address_count++ > 4) 
                {
                    capture_address_count = 0;
                    //navy 网络发送 地址设置错误或没有捕获报警主机的地址码
                    printf_debug("FUNC[%s] LINE[%d]\tKeyboard address set error or did not capture CK2316 address code!\n",__FUNCTION__, __LINE__);
                    ck2316_alarm_data.if_ck2316_alive = NO;
                    //ClrCom(*com_fd);
                }
                if (ck2316_alarm_data.setup_command_set != CK2316_NO_VALID_COMMAND) 
                {
                    //navy 网络发送 报警主机不在线
                    ck2316_alarm_data.setup_command_set = CK2316_NO_VALID_COMMAND;
                    printf_debug("FUNC[%s] LINE[%d]\tWarning ! Timeout, ck2316 is not alive !\n",__FUNCTION__, __LINE__);
                }
                break;
            case -1:
                if (ck2316_alarm_data.setup_command_set != CK2316_NO_VALID_COMMAND) 
                {
                    //navy 网络发送 报警主机不在线
                    ck2316_alarm_data.setup_command_set = CK2316_NO_VALID_COMMAND;
                    printf_debug("FUNC[%s] LINE[%d]\tWarning ! Timeout, ck2316 is not alive !\n",__FUNCTION__, __LINE__);
                }
                printf_debug("FUNC[%s] LINE[%d]\tTimeout, ck2316 is not alive ! recv_ret = %d\n",__FUNCTION__, __LINE__,recv_ret);
                if (timeout_count++ > 10000) 
                {
                    timeout_count = 0;
                }
                if (ck2316_check_command_setup_successful_flag == CK2316_ALARM_HOST_RESET) 
                {
                    if (timeout_count > 3) 
                    {
                        //navy 网络发送 报警主机复位失败
                        printf_debug("FUNC[%s] LINE[%d]\tCK2316 failed to reset!\n",__FUNCTION__, __LINE__);
                        ck2316_check_command_setup_successful_flag = CK2316_NO_VALID_COMMAND; 
                    }
                }
                ck2316_alarm_data.if_ck2316_alive = NO;
                break;
            case -2:
                printf("FUNC[%s] LINE[%d]\tRecv data error!\n",__FUNCTION__, __LINE__);
                return -1;
            default: //navy2
                if (verify_ck2316_data(recv_buffer, recv_ret) == 0)
                {
                    switch(ck2316_alarm_data.setup_command_set)
                    {
                        case CK2316_NO_VALID_COMMAND:  //没有合法的命令，则发送握手码   
                            write(*com_fd, ck2316_handshake_code, CK2316_HANDSHAKE_SIZE);
                            ck2316_alarm_data.if_ck2316_alive = YES;
                            if (ck2316_check_command_setup_successful_flag == CK2316_ALARM_HOST_RESET) 
                            {
                                if (timeout_count < 3) 
                                {
                                    //navy 网络发送 报警主机成功复位
                                    printf_debug("FUNC[%s] LINE[%d]\tCK2316 succeed to reset!\n",__FUNCTION__, __LINE__);
                                }
                                else
                                {
                                    //navy 网络发送 报警主机复位失败
                                    printf_debug("FUNC[%s] LINE[%d]\tCK2316 failed to reset!\n",__FUNCTION__, __LINE__);
                                }
                                ck2316_check_command_setup_successful_flag = CK2316_NO_VALID_COMMAND; 
                            }
                            break;
                        case CK2316_SYSTEM_DEFENCE: //系统布防
                            if (ck2316_alarm_data.ck2316_defence_status == IS_DEFENCED) 
                            {
                                write(*com_fd, ck2316_handshake_code, CK2316_HANDSHAKE_SIZE);
                                //navy 网络发送 系统已经布防，现在不用布防
                                printf_debug("FUNC[%s] LINE[%d]\tWarning ! CK2316 has been defence status !\n",__FUNCTION__, __LINE__);
                                ck2316_alarm_data.setup_command_set = CK2316_NO_VALID_COMMAND;
                            }
                            else
                            {
                                write(*com_fd, system_defence_code[system_count], CK2316_HANDSHAKE_SIZE);
                                system_count++;
                                if (system_count >= CK2316_SYSTEM_DEFENCE_SIZE) 
                                {
                                    system_count = 0;
                                    //ck2316_alarm_data.setup_command_set = CK2316_NO_VALID_COMMAND;
                                    ck2316_alarm_data.setup_command_set = CK2316_CHECK_COMMAND_IF_SETUP_SUCCESSFUL;
                                    ck2316_check_command_setup_successful_flag = CK2316_SYSTEM_DEFENCE;
                                }
                            }
                            break;
                        case CK2316_SYSTEM_ABANDON://系统撤防
                            if ((ck2316_alarm_data.ck2316_defence_status == IS_ABANDONED) && ((ck2316_alarm_data.ck2316_defence_area_alarm_status & EMERGENCY_ALARM) != EMERGENCY_ALARM)) 
                            {
                                write(*com_fd, ck2316_handshake_code, CK2316_HANDSHAKE_SIZE);
                                //navy 网络发送 系统已经撤防，现在不用撤防
                                printf_debug("FUNC[%s] LINE[%d]\tWarning ! CK2316 has been abandoned status !\n",__FUNCTION__, __LINE__);
                                ck2316_alarm_data.setup_command_set = CK2316_NO_VALID_COMMAND;
                            }
                            else
                            {
                                write(*com_fd, system_abandon_code[system_count], CK2316_HANDSHAKE_SIZE);
                                system_count++;
                                if (system_count >= CK2316_SYSTEM_ABANDON_SIZE) 
                                {
                                    system_count = 0;
                                    //ck2316_alarm_data.setup_command_set = CK2316_NO_VALID_COMMAND;
                                    ck2316_alarm_data.setup_command_set = CK2316_CHECK_COMMAND_IF_SETUP_SUCCESSFUL;
                                    ck2316_check_command_setup_successful_flag = CK2316_SYSTEM_ABANDON;
                                }
                            }
                            break;
                        case CK2316_ELIMINATE_ALARM_MEMORY://消除报警记忆
                            write(*com_fd, eliminate_alarm_memory_code[system_count], CK2316_HANDSHAKE_SIZE);
                            system_count++;
                            if (system_count >= CK2316_ELIMINATE_ALARM_MEMORY_SIZE) 
                            {
                                system_count = 0;
                                //ck2316_alarm_data.setup_command_set = CK2316_NO_VALID_COMMAND;
                                ck2316_alarm_data.setup_command_set = CK2316_CHECK_COMMAND_IF_SETUP_SUCCESSFUL;
                                ck2316_check_command_setup_successful_flag = CK2316_ELIMINATE_ALARM_MEMORY;
                            }
                            break;
                        case CK2316_BYPASS_DEFENCE_AREA: //系统旁路
                            if (ck2316_alarm_data.ck2316_defence_status == IS_DEFENCED) 
                            {
                                //navy 网络发送 请撤防后再旁路
                                printf_debug("FUNC[%s] LINE[%d]\tPlease abandon ,then bypass defence area!\n",__FUNCTION__, __LINE__);
                                ck2316_alarm_data.setup_command_set = CK2316_NO_VALID_COMMAND;
                            }
                            else
                            {
                                write(*com_fd, bypass_defence_area_code[system_count], CK2316_HANDSHAKE_SIZE);
                                if (system_count == 0) 
                                {
                                    before_defence_area_bypass_value = (ck2316_alarm_data.ck2316_defence_area_bypass_value[1]<<8) | ck2316_alarm_data.ck2316_defence_area_bypass_value[0];
                                }
                                system_count++;
                                if (system_count == 5 && bypass_defence_area_code[system_count][2] == 0) 
                                {
                                    system_count++;
                                }
                                if (system_count >= CK2316_BYPASS_DEFENCE_AREA_SIZE) 
                                {
                                    system_count = 0;
                                    //ck2316_alarm_data.setup_command_set = CK2316_NO_VALID_COMMAND;
                                    ck2316_alarm_data.setup_command_set = CK2316_CHECK_COMMAND_IF_SETUP_SUCCESSFUL;
                                    ck2316_check_command_setup_successful_flag = CK2316_BYPASS_DEFENCE_AREA;
                                }
                            }
                            break;
                        case CK2316_ALARM_HOST_RESET: //报警主机复位
                            if (ck2316_alarm_data.ck2316_defence_status == IS_DEFENCED) 
                            {
                                write(*com_fd, ck2316_handshake_code, CK2316_HANDSHAKE_SIZE);
                                ck2316_alarm_data.setup_command_set = CK2316_NO_VALID_COMMAND;
                                //navy 网络发送 报警主机复位失败
                                printf_debug("FUNC[%s] LINE[%d]\tCK2316 failed to reset, please abandon,then try again!\n",__FUNCTION__, __LINE__);
                            }
                            else
                            {
                                write(*com_fd, alarm_host_reset_code[system_count], CK2316_HANDSHAKE_SIZE);
                                system_count++;
                                if (system_count >= CK2316_ALARM_HOST_RESET_SIZE) 
                                {
                                    system_count = 0;
                                    ck2316_alarm_data.setup_command_set = CK2316_NO_VALID_COMMAND;
                                    //ck2316_alarm_data.setup_command_set = CK2316_CHECK_COMMAND_IF_SETUP_SUCCESSFUL;
                                    timeout_count = 0;
                                    ck2316_check_command_setup_successful_flag = CK2316_ALARM_HOST_RESET;
                                }
                            }
                            break;
                        /*
                        case CK2316_SIMULATE_KEYBOARD_ADDRESS_SETUP: //报警主机模拟键盘地址设置
                            write(*com_fd, ck2316_handshake_code, CK2316_HANDSHAKE_SIZE);
                            ck2316_alarm_data.setup_command_set = CK2316_NO_VALID_COMMAND;
                            //navy 网络发送 报警主机模拟键盘地址设置成功
                            printf_debug("\n\nCK2316 change keyboard address as: 0x%02x\n\n",ck2316_alarm_data.ck2316_simulate_keyboard_address);
                            break;
                        case CK2316_SIMULATE_KEYBOARD_PASSWORD_SETUP: //报警主机模密码设置
                            write(*com_fd, ck2316_handshake_code, CK2316_HANDSHAKE_SIZE);
                            ck2316_alarm_data.setup_command_set = CK2316_NO_VALID_COMMAND;
                            //navy 网络发送 报警主机模拟键盘用户密码设置成功
                            printf_debug("\n\nCK2316 change keyboard password as: %02d %02d %02d %02d\n\n",ck2316_alarm_data.ck2316_user_password[0], ck2316_alarm_data.ck2316_user_password[1], ck2316_alarm_data.ck2316_user_password[2], ck2316_alarm_data.ck2316_user_password[3]);
                            break;
                        */
                        case CK2316_CHECK_COMMAND_IF_SETUP_SUCCESSFUL:
                            write(*com_fd, ck2316_handshake_code, CK2316_HANDSHAKE_SIZE);
                            check_command_count++;
                            if (check_command_count > MAX_CHECK_COMMAND_COUNT) 
                            {
                                switch(ck2316_check_command_setup_successful_flag)
                                {
                                    case CK2316_SYSTEM_DEFENCE:   
                                        if (ck2316_alarm_data.ck2316_defence_status == IS_DEFENCED) 
                                        {
                                            //navy 网络发送 系统布防成功
                                            printf_debug("FUNC[%s] LINE[%d]\tCK2316 succeed to defence !\n",__FUNCTION__, __LINE__);
                                        }
                                        else
                                        {
                                            //navy 网络发送 系统布防失败
                                            printf_debug("FUNC[%s] LINE[%d]\tCK2316 failed to defence !\n",__FUNCTION__, __LINE__);
                                        }
                                        break;
                                    case CK2316_SYSTEM_ABANDON:   
                                        if (ck2316_alarm_data.ck2316_defence_status == IS_ABANDONED) 
                                        {
                                            //navy 网络发送 系统撤防成功
                                            printf_debug("FUNC[%s] LINE[%d]\tCK2316 succeed to abandon !\n",__FUNCTION__, __LINE__);
                                        }
                                        else
                                        {
                                            //navy 网络发送 系统撤防失败
                                            printf_debug("FUNC[%s] LINE[%d]\tCK2316 failed to abandon !\n",__FUNCTION__, __LINE__);
                                        }
                                        break;
                                    case CK2316_ELIMINATE_ALARM_MEMORY:   
                                        if (ck2316_alarm_data.ck2316_defence_area_alarm_memory_value[0] == 0x00 && ck2316_alarm_data.ck2316_defence_area_alarm_memory_value[1] == 0x00) 
                                        {
                                            //navy 网络发送 系统已经成功消除报警记忆
                                            printf_debug("FUNC[%s] LINE[%d]\tCK2316 succeed to eliminate alarm memory!\n",__FUNCTION__, __LINE__);
                                        }
                                        else
                                        {
                                            //navy 网络发送 系统消除报警记忆失败
                                            printf_debug("FUNC[%s] LINE[%d]\tCK2316 failed to eliminate alarm memory!\n",__FUNCTION__, __LINE__);
                                        }

                                        break;
                                    case CK2316_BYPASS_DEFENCE_AREA:   
                                        if (before_defence_area_bypass_value != ((ck2316_alarm_data.ck2316_defence_area_bypass_value[1]<<8) | ck2316_alarm_data.ck2316_defence_area_bypass_value[0])) 
                                        {
                                            if (before_defence_area_bypass_value & (1<<(bypass_defence_area_code[5][2]*10 + bypass_defence_area_code[6][2]-1))) 
                                            {
                                                //navy 网络发送 系统已经成功取消旁路防区
                                                printf_debug("FUNC[%s] LINE[%d]\tCK2316 succeed to cancel bypass defence area %d\n",__FUNCTION__, __LINE__, bypass_defence_area_code[5][2]*10 + bypass_defence_area_code[6][2]);
                                            }
                                            else
                                            {
                                                //navy 网络发送 系统已经成功旁路防区
                                                printf_debug("FUNC[%s] LINE[%d]\tCK2316 succeed to bypass defence area %d\n",__FUNCTION__, __LINE__, bypass_defence_area_code[5][2]*10 + bypass_defence_area_code[6][2]);
                                            }
                                        }
                                        else
                                        {
                                                //navy 网络发送 系统旁路或取消防区失败
                                                printf_debug("FUNC[%s] LINE[%d]\tCK2316 failed to bypass or cancel bypass defence area %d\n",__FUNCTION__, __LINE__, bypass_defence_area_code[5][2]*10 + bypass_defence_area_code[6][2]);
                                        }
                                        break;
                                    case CK2316_ALARM_HOST_RESET:
                                        break;
                                    default :
                                        break;
                                }
                                check_command_count = 0;
                                ck2316_alarm_data.setup_command_set = CK2316_NO_VALID_COMMAND;
                            }
                            break;
                        default:
                            ck2316_alarm_data.setup_command_set = CK2316_NO_VALID_COMMAND;
                            break;
                    }
                    /*
                    FE 04 03 FF 02 00 00 00 00 00 0F 03 
                    FE 04 03 A5 10 00 00 00 00 00 00 0F 03 
                    FE 04 03 A5 11 00 00 00 00 00 00 0F 03 
                    FE 04 03 A5 12 00 00 00 00 00 00 0F 03 
                    FE 04 03 FF 09 00 00 00 00 00 08 0F 03 */
                    if (recv_buffer[recv_ret-8] == CK2316_HIGH_DEFENCE_AREA_HEAD)    //高防区 
                    {
                        //获取高防区报警记忆状态
                        ck2316_alarm_data.ck2316_defence_area_alarm_memory_value[1] = recv_buffer[recv_ret-7];
                        //获取高防区实时报警状态
                        ck2316_alarm_data.ck2316_defence_area_real_time_alarm_value[1] = recv_buffer[recv_ret-6];
                        //获取高防区旁路状态
                        ck2316_alarm_data.ck2316_defence_area_bypass_value[1] = recv_buffer[recv_ret-4];
                    }
                    else if (recv_buffer[recv_ret-9] == CK2316_LOW_DEFENCE_AREA_HEAD) //低防区
                    {
                        //获取布撤防状态（布防，撤防）
                        if (recv_buffer[recv_ret-4] & CK2316_IS_DEFENCED_OR_ABANDONED_MASK) 
                            ck2316_alarm_data.ck2316_defence_status = IS_DEFENCED;
                        else
                            ck2316_alarm_data.ck2316_defence_status = IS_ABANDONED;
                        printf_debug("ck2316_defence_status = 0x%02x\t",ck2316_alarm_data.ck2316_defence_status);
                        printf_debug("keyboard_address = 0x%02X\n",ck2316_alarm_data.ck2316_simulate_keyboard_address);
                        //获取低防区报警记忆状态
                        ck2316_alarm_data.ck2316_defence_area_alarm_memory_value[0] = recv_buffer[recv_ret-8];
                        if (ck2316_alarm_data.ck2316_defence_area_alarm_status & EMERGENCY_ALARM)
                        {
                            ck2316_alarm_data.ck2316_defence_area_alarm_memory_value[0] = recv_buffer[recv_ret-8] | 0x80;
                        }
                        //获取低防区实时报警状态
                        ck2316_alarm_data.ck2316_defence_area_real_time_alarm_value[0] = recv_buffer[recv_ret-7] | (recv_buffer[recv_ret-6] & 0x80);
                        //获取低防区旁路状态
                        ck2316_alarm_data.ck2316_defence_area_bypass_value[0] = recv_buffer[recv_ret-5];
                        //CK2316防区报警状态(0:无报警 1:普通防区报警 2:紧急防区报警 3:普通和紧急防区都有报警)
                        ck2316_alarm_data.ck2316_defence_area_alarm_status = recv_buffer[recv_ret-3] & CK2316_DEFENCE_AREA_ALARM_STATUS_MASK;
                        switch(ck2316_alarm_data.ck2316_defence_area_alarm_status)
                        {
                            case NO_ALARM:
                                break;
                            case NORMAL_ALARM:
                            case EMERGENCY_ALARM:
                            case NORMAL_AND_EMERGENCY_ALARM:
                                if (if_have_net_client_connect == YES) 
                                //navy 报警上传
                                    printf_debug("Warning ! Defence area alarm\t0x%02X%02X 0x%02X%02X 0x%02X%02X 0x%02X 0x%02X\n",ck2316_alarm_data.ck2316_defence_area_alarm_memory_value[1], ck2316_alarm_data.ck2316_defence_area_alarm_memory_value[0], ck2316_alarm_data.ck2316_defence_area_real_time_alarm_value[1], ck2316_alarm_data.ck2316_defence_area_real_time_alarm_value[0], ck2316_alarm_data.ck2316_defence_area_bypass_value[1], ck2316_alarm_data.ck2316_defence_area_bypass_value[0], ck2316_alarm_data.ck2316_defence_status, ck2316_alarm_data.ck2316_defence_area_alarm_status);
                                else
                                    printf_debug("Warning no net connect! Defence area alarm\t0x%02X%02X 0x%02X%02X 0x%02X%02X 0x%02X 0x%02X\n",ck2316_alarm_data.ck2316_defence_area_alarm_memory_value[1], ck2316_alarm_data.ck2316_defence_area_alarm_memory_value[0], ck2316_alarm_data.ck2316_defence_area_real_time_alarm_value[1], ck2316_alarm_data.ck2316_defence_area_real_time_alarm_value[0], ck2316_alarm_data.ck2316_defence_area_bypass_value[1], ck2316_alarm_data.ck2316_defence_area_bypass_value[0], ck2316_alarm_data.ck2316_defence_status, ck2316_alarm_data.ck2316_defence_area_alarm_status);
                                break;
                        }
                    }
                }
                else
                {
                    if (ck2316_alarm_data.ck2316_simulate_keyboard_address != recv_buffer[recv_ret-2]) 
                    {
                        //navy 网络发送 地址设置错误
                        printf_debug("FUNC[%s] LINE[%d]\tKeyboard address set error ! simulate keyboard address = 0x%X\n",__FUNCTION__, __LINE__, ck2316_alarm_data.ck2316_simulate_keyboard_address);
        #ifdef DEBUG
                        //print_string("\n ", recv_buffer, recv_ret);
        #endif
                        ClrCom(*com_fd);
                        sleep(1);
                    }
                    printf_debug("FUNC[%s] LINE[%d]\tVerify data error,recv_ret = %d Adjusting... \tOK\n",__FUNCTION__, __LINE__, recv_ret);
                }
#ifdef DEBUG
                //printf_debug("recv_ret = %d\n",recv_ret);
                //print_string("\n ", recv_buffer, recv_ret);
#endif
                break;
        }
    }

    return 1;
}

int verify_ck2316_data(unsigned char *recv_buffer, unsigned int len)
{
    if (len < 2) 
    {
        return -1;
    }
    else if (len == 2) // 0F 03
    {
        if ((recv_buffer[0] == ck2316_keyboard_address_code[0]) && (recv_buffer[1] == ck2316_keyboard_address_code[1])) 
        {
            return 0;
        }
        else
        {
            return -1;
        }
    }
    else if (len == 5) //FE 04 03 0F 03
    {
        if ((recv_buffer[0] == CK2316_HANDSHAKE_CODE_1) && (recv_buffer[1] == CK2316_HANDSHAKE_CODE_1) && (recv_buffer[2] == CK2316_HANDSHAKE_CODE_3) && (recv_buffer[3] == ck2316_keyboard_address_code[0]) && (recv_buffer[4] == ck2316_keyboard_address_code[1])) 
        {
            return 0;
        }
        else
        {
            return -1;
        }
    }
    //FE 04 03 FF 02 *** 0F 03
    //FE 04 03 A5 10 *** 0F 03
    //FE 04 03 A5 11 *** 0F 03
    //FE 04 03 A5 12 *** 0F 03
    //FE 04 03 FF 09 *** 0F 03
    //FE 06 01/02/03/04/0B FF 02 *** 0F 03
    //FE 04 02/06 FF 02 *** 0F 03
    else 
    {
#if 0
        if ((recv_buffer[0] == CK2316_HANDSHAKE_CODE_1) && (recv_buffer[1] == CK2316_HANDSHAKE_CODE_1) && (recv_buffer[2] == CK2316_HANDSHAKE_CODE_3)) 
#else
            //if ((recv_buffer[len-2] == ck2316_keyboard_address_code[0]) && (recv_buffer[len-1] == ck2316_keyboard_address_code[1])) 
            if ((recv_buffer[0] == CK2316_HANDSHAKE_CODE_1) && (recv_buffer[len-2] == ck2316_keyboard_address_code[0]) && (recv_buffer[len-1] == ck2316_keyboard_address_code[1])) 
                //if ((recv_buffer[len-1] == ck2316_keyboard_address_code[0])) 
#endif
            {
                //printf_debug("FUNC[%s] LINE[%d]\tlen = %d\n",__FUNCTION__, __LINE__, len);
                return 0;
            }
            else
            {
                return -1;
            }
    }
}

int send_handshake_packet(int *com_fd)
{
    if (SendDataToCom(*com_fd, ck2316_handshake_code, CK2316_HANDSHAKE_SIZE) == -1)
    {
        printf("FUNC[%s] LINE[%d]\tSend data to com error!\n",__FUNCTION__, __LINE__);
        return -1;
    }
    return 0;
}

int ck2316_simulate_keyboard_address_setup(unsigned int address)
{
    ck2316_keyboard_address_code[0] = address;

    fseek(fp_ck2316_config_file, 0, SEEK_SET);
    fprintf(fp_ck2316_config_file, "%02d %02d %02d %02d %02d",ck2316_alarm_data.ck2316_simulate_keyboard_address, ck2316_alarm_data.ck2316_user_password[0], ck2316_alarm_data.ck2316_user_password[1], ck2316_alarm_data.ck2316_user_password[2], ck2316_alarm_data.ck2316_user_password[3]);
    fflush(fp_ck2316_config_file);

    return 0;
}


int ck2316_user_password_setup(unsigned int user_password[4])
{
    int i = 0;

    for (i = 0; i < 4; i++) 
    {
        system_defence_code[i][2] = (unsigned char)user_password[i];
        system_abandon_code[i][2] = (unsigned char)user_password[i];
        bypass_defence_area_code[i][2] = (unsigned char)user_password[i];
        alarm_host_reset_code[i][2] = (unsigned char)user_password[i];
    }
    fseek(fp_ck2316_config_file, 0, SEEK_SET);
    fprintf(fp_ck2316_config_file, "%02d %02d %02d %02d %02d",ck2316_alarm_data.ck2316_simulate_keyboard_address, ck2316_alarm_data.ck2316_user_password[0], ck2316_alarm_data.ck2316_user_password[1], ck2316_alarm_data.ck2316_user_password[2], ck2316_alarm_data.ck2316_user_password[3]);
    fflush(fp_ck2316_config_file);
    return 0;
}

int ck2316_alarm_init(void)
{
    int i = 0;
    //15 01 02 03 04
    //打开存储门禁控制器一些参数的文件
    //一共存储5个参数，从上到下顺序为：
    //door_lock_relay_status_setup (1:常开, 0:常闭)   
    //door_contact_detection_mode_setup (1:开路，0:短路)
    //door_status (1:开门，0:关门)
    //client_set_door_hold_time (0-255s)
    //button_set_door_hold_time (0-255s)
    if ((fp_ck2316_config_file = fopen(CK2316_CONFIG_FILE, "a+")) == NULL)
    {
        printf("FUNC[%s] LINE[%d]\tOpen %s error!\n",__FUNCTION__, __LINE__, CK2316_CONFIG_FILE);
        return -1;
    }
    fseek(fp_ck2316_config_file, 0, SEEK_SET);
    if ((fscanf(fp_ck2316_config_file, "%02d ",&ck2316_alarm_data.ck2316_simulate_keyboard_address)) != 1)
    {
        ck2316_keyboard_address_code[0] = CK2316_KEYBOARD_ADDRESS;
        ck2316_alarm_data.ck2316_simulate_keyboard_address = CK2316_KEYBOARD_ADDRESS;
    }
    else
    {
        ck2316_keyboard_address_code[0] = ck2316_alarm_data.ck2316_simulate_keyboard_address;
    }

    if ((fscanf(fp_ck2316_config_file, "%02d %02d %02d %02d",&ck2316_alarm_data.ck2316_user_password[0], &ck2316_alarm_data.ck2316_user_password[1], &ck2316_alarm_data.ck2316_user_password[2], &ck2316_alarm_data.ck2316_user_password[3])) != 4)
    {
        ck2316_alarm_data.ck2316_user_password[0] = CK2316_USER_PASSWORD_1;
        ck2316_alarm_data.ck2316_user_password[1] = CK2316_USER_PASSWORD_2;
        ck2316_alarm_data.ck2316_user_password[2] = CK2316_USER_PASSWORD_3;
        ck2316_alarm_data.ck2316_user_password[3] = CK2316_USER_PASSWORD_4;
    }
    else
    {
        //ck2316_user_password_setup(ck2316_alarm_data.ck2316_user_password);
        for (i = 0; i < 4; i++) 
        {
            system_defence_code[i][2] = (unsigned char)ck2316_alarm_data.ck2316_user_password[i];
            system_abandon_code[i][2] = (unsigned char)ck2316_alarm_data.ck2316_user_password[i];
            bypass_defence_area_code[i][2] = (unsigned char)ck2316_alarm_data.ck2316_user_password[i];
            alarm_host_reset_code[i][2] = (unsigned char)ck2316_alarm_data.ck2316_user_password[i];
        }
    }

    fclose(fp_ck2316_config_file);
    if ((fp_ck2316_config_file = fopen(CK2316_CONFIG_FILE, "w+")) == NULL)
    {
        printf("FUNC[%s] LINE[%d]\tOpen %s error!\n",__FUNCTION__, __LINE__, CK2316_CONFIG_FILE);
        return -1;
    }
    printf_debug("\n\nCK2316 initial status info: %02d %02d %02d %02d %02d\n\n",ck2316_alarm_data.ck2316_simulate_keyboard_address, ck2316_alarm_data.ck2316_user_password[0], ck2316_alarm_data.ck2316_user_password[1], ck2316_alarm_data.ck2316_user_password[2], ck2316_alarm_data.ck2316_user_password[3]);
    fprintf(fp_ck2316_config_file, "%02d %02d %02d %02d %02d",ck2316_alarm_data.ck2316_simulate_keyboard_address, ck2316_alarm_data.ck2316_user_password[0], ck2316_alarm_data.ck2316_user_password[1], ck2316_alarm_data.ck2316_user_password[2], ck2316_alarm_data.ck2316_user_password[3]);
    fflush(fp_ck2316_config_file);

    return 0;
}

