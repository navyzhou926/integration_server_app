#include <unistd.h>
#include <stdio.h>
#include "matrix_control.h"
#include "net.h"
#include "serial_common.h"
#include "uart.h"
#include "global.h"
#include "cradle_head_control.h"
#include "net_server.h"

//#define DEBUG
#ifdef DEBUG
#define printf_debug(fmt, arg...) printf(fmt, ##arg)
#else
#define printf_debug(fmt, arg...) do{}while(0)
#endif

#define PELCO_P_UART_MAX_SNED_SIZE      8
#define PELCO_D_UART_MAX_SNED_SIZE      7
#define YAAN_UART_MAX_SNED_SIZE         6

#define CRADLE_HEAD_DEFAULT_ADDRESS             0x00  //1号设备
#define CRADLE_HEAD_DEFAULT_PROTOCOL_TYPE       Palco_P
#define CRADLE_HEAD_DEFAULT_MOVE_SPEED          0x0f
#define CRADLE_HEAD_DEFAULT_PRESET_POINT        0x01

cradle_head_control_arg cradle_head_control_data = {0, 
CRADLE_HEAD_CONTROL_NO_VALID_COMMAND,CRADLE_HEAD_DEFAULT_PROTOCOL_TYPE,CRADLE_HEAD_DEFAULT_ADDRESS,CRADLE_HEAD_DEFAULT_MOVE_SPEED,CRADLE_HEAD_DEFAULT_PRESET_POINT,{CRADLE_HEAD_SERIAL_PORT,{0,0,0},{LINK_DEV_CRADLE_HEAD,1,0},{CRADLE_HEAD_UART_BOARD,CRADLE_HEAD_UART_DATA_BIT,CRADLE_HEAD_UART_STOP_BIT,CRADLE_HEAD_UART_CHECK_BIT,0,{0,0}},{0}}};

unsigned char CRADLE_HEAD_UP[8] = {0xa0,0x00,0x00,0x08,0x00,0x0f,0xaf,0x08};   //上
unsigned char CRADLE_HEAD_DOWN[8] = {0xa0,0x00,0x00,0x10,0x00,0x0f,0xaf,0x10}; //下
unsigned char CRADLE_HEAD_LEFT[8] = {0xa0,0x00,0x00,0x04,0x0f,0x00,0xaf,0x04}; //左
unsigned char CRADLE_HEAD_RIGHT[8] = {0xa0,0x00,0x00,0x02,0x0f,0x00,0xaf,0x02};//右
unsigned char CRADLE_HEAD_STOP[8] = {0xa0,0x00,0x00,0x00,0x00,0x00,0xaf,0x00}; //停止

unsigned char CRADLE_HEAD_ZOOM_IN[8] = {0xa0,0x00,0x00,0x20,0x00,0x00,0xaf,0x2f}; //变倍长(特写, 放大)
unsigned char CRADLE_HEAD_ZOOM_OUT[8] = {0xa0,0x00,0x00,0x40,0x00,0x00,0xaf,0x4f};//变倍短(广角, 缩小)
unsigned char CRADLE_HEAD_FOCUS_IN[8] = {0xa0,0x00,0x02,0x00,0x00,0x00,0xaf,0x0d};//聚焦近
unsigned char CRADLE_HEAD_FOCUS_OUT[8] = {0xa0,0x00,0x01,0x00,0x00,0x00,0xaf,0x0e};//聚焦远
unsigned char CRADLE_HEAD_APERTURE_SMALL[8] = {0xa0,0x00,0x08,0x00,0x00,0x00,0xaf,0x07};//光圈小
unsigned char CRADLE_HEAD_APERTURE_BIG[8] = {0xa0,0x00,0x04,0x00,0x00,0x00,0xaf,0x0b};  //光圈大
unsigned char CRADLE_HEAD_SET_PRESET_POINT[8] = {0xa0,0x00,0x00,0x03,0x00,0x01,0xaf,0x0d};//设置预置点001
unsigned char CRADLE_HEAD_TURN_TO_PRESET_POINT[8] = {0xa0,0x00,0x00,0x07,0x00,0x01,0xaf,0x09};//转至预置点001

unsigned char PELCO_D_Table[20][7]={
    {0xff,0x01,0x00,0x08,0x00,0x2f,},       // 1 上
    {0xff,0x01,0x00,0x10,0x00,0x2f,},       // 2 下
    {0xff,0x01,0x00,0x04,0xff,0x00,},       // 3 左
    {0xff,0x01,0x00,0x02,0xff,0x00,},       // 4 右
    {0xff,0x01,0x04,0x00,0x00,0x00,},       // 5 光圈大
    {0xff,0x01,0x02,0x00,0x00,0x00,},       // 6 光圈小
    {0xff,0x01,0x00,0x80,0x00,0x00,},       // 7 聚焦近
    {0xff,0x01,0x01,0x00,0x00,0x00,},       // 8 聚焦远
    {0xff,0x01,0x00,0x40,0x00,0x00,},       // 9 变倍长
    {0xff,0x01,0x00,0x20,0x00,0x00,},       // 10 变倍短
    {0x00,0x01,0x00,0x00,0x00,0x00,},       // 11       
    {0x00,0x01,0x00,0x00,0x00,0x00,},       // 12
    {0x00,0x01,0x00,0x00,0x00,0x00,},       // 13              
    {0xff,0x01,0x00,0x03,0x00,0x01,},       // 14 设置预置点001
    {0xff,0x01,0x00,0x07,0x00,0x01,},       // 15 转至预置点001
    {0xff,0x01,0x00,0x00,0x00,0x00,},       // 16 停命令
    {0x00,0x01,0x00,0x00,0x00,0x00,},       // 17 
    {0x00,0x01,0x00,0x00,0x00,0x00,},       // 18 
    {0x00,0x01,0x00,0x00,0x00,0x00,},       // 19
    {0x00,0x01,0x00,0x00,0x00,0x00,},       // 20
};

unsigned char PELCO_P_Table[20][8]={
    {0xa0,0x00,0x00,0x08,0x00,0x30,0xaf,},  // 1 上
    {0xa0,0x00,0x00,0x10,0x00,0x30,0xaf,},  // 2 下
    {0xa0,0x00,0x00,0x04,0x10,0x00,0xaf,},  // 3 左
    {0xa0,0x00,0x00,0x02,0x10,0x00,0xaf,},  // 4 右
    {0xa0,0x00,0x04,0x00,0x00,0x00,0xaf,},  // 5 光圈大
    {0xa0,0x00,0x08,0x00,0x00,0x00,0xaf,},  // 6 光圈小
    {0xa0,0x00,0x02,0x00,0x00,0x00,0xaf,},  // 7 聚焦近
    {0xa0,0x00,0x01,0x00,0x00,0x00,0xaf,},  // 8 聚焦远
    {0xa0,0x00,0x00,0x20,0x00,0x00,0xaf,},  // 9 变倍长(特写, 放大)
    {0xa0,0x00,0x00,0x40,0x00,0x00,0xaf,},  // 10 变倍短(广角, 缩小)
    {0x00,0x00,0x00,0x00,0x00,0x00,0x00,},  // 11
    {0x00,0x00,0x00,0x00,0x00,0x00,0x00,},  // 12
    {0x00,0x00,0x00,0x00,0x00,0x00,0x00,},  // 13              
    {0xa0,0x00,0x00,0x03,0x00,0x01,0xaf,},  // 14 设置预置点001
    {0xa0,0x00,0x00,0x07,0x00,0x01,0xaf,},  // 15 转至预置点001
    {0xa0,0x00,0x00,0x00,0x00,0x00,0xaf,},  // 16 停命令
    {0x00,0x00,0x00,0x00,0x00,0x00,0x00,},  // 17 
    {0x00,0x00,0x00,0x00,0x00,0x00,0x00,},  // 18 
    {0x00,0x00,0x00,0x00,0x00,0x00,0x00,},  // 19
    {0x00,0x00,0x00,0x00,0x00,0x00,0x00,},  // 20
};

unsigned char YAAN_Table[20][12]={
    {0x02,0x01,0x01,0x04,0x00,0x09,0x02,0x01,0x0E,0x00,0x00,},  // 1 上
    {0x02,0x01,0x01,0x08,0x00,0x0D,0x02,0x01,0x0E,0x00,0x00,},  // 2 下
    {0x02,0x01,0x01,0x02,0x00,0x07,0x02,0x01,0x0E,0x00,0x00,},  // 3 左
    {0x02,0x01,0x01,0x01,0x00,0x06,0x02,0x01,0x0E,0x00,0x00,},  // 4 右
    {0x02,0x01,0x01,0x00,0x00,0x24,0x02,0x01,0x01,0x00,0x00,},  // 5 光圈大
    {0x02,0x01,0x01,0x00,0x00,0x14,0x02,0x01,0x01,0x00,0x00,},  // 6 光圈小
    {0x02,0x01,0x01,0x40,0x00,0x44,0x02,0x01,0x01,0x40,0x00,},  // 7 聚焦近
    {0x02,0x01,0x01,0x80,0x00,0x84,0x02,0x01,0x01,0x80,0x00,},  // 8 聚焦远
    {0x02,0x01,0x01,0x20,0x00,0x00,0x02,0x00,0x01,0x20,0x00,},  // 9 变倍长(特写, 放大)
    {0x02,0x01,0x01,0x10,0x00,0x00,0x02,0x00,0x01,0x10,0x00,},  // 10 变倍短(广角，缩小)
    {0x02,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,},  // 11
    {0x02,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,},  // 12
    {0x02,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,},  // 13              
    {0x02,0x01,0x05,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,},  // 14 设置预置点001
    {0x02,0x01,0x02,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,},  // 15 转至预置点001
    {0x02,0x01,0x01,0x00,0x00,0x04,0x02,0x01,0x0E,0x00,0x00,},  // 16 停命令
    {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,},  // 17 
    {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,},  // 18 
    {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,},  // 19
    {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,},  // 20
};  

extern void print_string(char *string, unsigned char *buffer, unsigned int len);
int cradle_head_handshake_and_contorl(int *com_fd);

void *pthread_cradle_head_control(void *arg)
{
    int com_fd;
    int recv_ret = 0;
    //unsigned char send_buffer[] = {0xBB, 0x00, 0x06, 0x02};

    while (1) 
    {
        com_fd = OpenDev(CRADLE_HEAD_UART_DEVICE);
        if (com_fd == -1) 
        {
            printf("FUNC[%s] LINE[%d]\tOpen uart failed!\n",__FUNCTION__, __LINE__);
            sleep(10);
            continue;
        }
        //fd nSpeed nBits nEvent nStop)
        //recv_ret = set_opt(com_fd, 9600, 3, 0, 0); //for cradle head  8N1
        recv_ret = set_opt(com_fd, CRADLE_HEAD_UART_BOARD, CRADLE_HEAD_UART_DATA_BIT, CRADLE_HEAD_UART_CHECK_BIT, CRADLE_HEAD_UART_STOP_BIT);
        if (recv_ret == -1) 
        {
            printf("FUNC[%s] LINE[%d]\tInit uart failed!\n",__FUNCTION__, __LINE__);
            sleep(10);
            continue;
        }
        ClrCom(com_fd);

        recv_ret = cradle_head_handshake_and_contorl(&com_fd);
        if (recv_ret == -1) 
        {
            close(com_fd);
            sleep(10);
        }
    }

    return (void *)1;
}

int cradle_head_handshake_and_contorl(int *com_fd)
{
    int i = 0;
    unsigned int temp = 0;
    char if_send_cradle_head_control_command = NO;

    //cradle_head_control_data.setup_command_set = CRADLE_HEAD_CONTROL_NO_VALID_COMMAND;
    //cradle_head_control_data.setup_command_set = PTZ_STOP_ALL;
    //cradle_head_control_data.setup_command_set = PTZ_UP;
    //cradle_head_control_data.setup_command_set = PTZ_DOWN;
    //cradle_head_control_data.setup_command_set = PTZ_LEFT;
    //cradle_head_control_data.setup_command_set = PTZ_RIGHT;
    //cradle_head_control_data.setup_command_set = PTZ_ZOOM_IN;
    //cradle_head_control_data.setup_command_set = PTZ_ZOOM_OUT;
    //cradle_head_control_data.setup_command_set = PTZ_FOCUS_IN;
    //cradle_head_control_data.setup_command_set = PTZ_FOCUS_OUT;
    //cradle_head_control_data.setup_command_set = PTZ_APERTURE_BIG;
    //cradle_head_control_data.setup_command_set = PTZ_APERTURE_SMALL;
    //cradle_head_control_data.setup_command_set = PTZ_SET_PTZBIT;
    //cradle_head_control_data.setup_command_set = PTZ_CLE_PTZBIT;
    
    while (1) 
    {
        //0xa0,0x01,0x00,0x08,0x00,0x30,0xaf 
        /*
        if (cradle_head_control_data.setup_command_set != CRADLE_HEAD_CONTROL_NO_VALID_COMMAND)
        	printf_debug("FUNC[%s] LINE[%d]\tcradle_head_control_data.setup_command_set = %d\n",__FUNCTION__, __LINE__, cradle_head_control_data.setup_command_set);
       */
        if (cradle_head_control_data.cradle_head_protocol_type == Palco_P) 
        {
            PELCO_P_Table[cradle_head_control_data.setup_command_set-1][1] = cradle_head_control_data.cradle_head_address;
            PELCO_P_Table[cradle_head_control_data.setup_command_set-1][7] = 0x00;

            switch(cradle_head_control_data.setup_command_set)
            {
		case PTZ_APERTURE_BIG: 
		case PTZ_APERTURE_SMALL: 
			cradle_head_control_data.setup_command_set = CRADLE_HEAD_CONTROL_NO_VALID_COMMAND;
			usleep(10000);
			break;
		case CRADLE_HEAD_CONTROL_NO_VALID_COMMAND: 
                    usleep(10000);
                    break;
                case PTZ_UP:            //上
                	printf("33333333\n");
                case PTZ_DOWN:          //下
                printf("44444444\n");
                    temp = (cradle_head_control_data.cradle_head_move_speed*0x3f)/0xff;
                    if (temp == 0) 
                        temp = 0x01;
                    else if (cradle_head_control_data.cradle_head_move_speed == 0xff) 
                        temp = 0x3f;

                    PELCO_P_Table[cradle_head_control_data.setup_command_set][5] = temp;
                    if_send_cradle_head_control_command = YES;
                    break;
                case PTZ_LEFT:          //左
                	printf("11111111\n");
                case PTZ_RIGHT:         //右
                	printf("22222222\n");
                    temp = (cradle_head_control_data.cradle_head_move_speed*0x3f)/0xff;
                    if (temp == 0) 
                        temp = 0x01;
                    else if (cradle_head_control_data.cradle_head_move_speed == 0xff) 
                        temp = 0x3f;

                    PELCO_P_Table[cradle_head_control_data.setup_command_set][4] = temp;
                    if_send_cradle_head_control_command = YES;
                    break;
                case PTZ_SET_PTZBIT:    //设置预置点
                case PTZ_CLE_PTZBIT:    //调用预置点
                    if (cradle_head_control_data.cradle_head_preset_point >= 1 && cradle_head_control_data.cradle_head_preset_point <= 32) 
                    {
                        //printf("FUNC[%s] LINE[%d]\tSet or call cradle head preset point successfully !\n",__FUNCTION__, __LINE__);
                        PELCO_P_Table[cradle_head_control_data.setup_command_set][5] = cradle_head_control_data.cradle_head_preset_point;
                    if_send_cradle_head_control_command = YES;
                    }
                    else
                    {
                        //网络发送 设置或调用云台预置点失败,预置点值错误
                        printf("FUNC[%s] LINE[%d]\tInvalid cradle head preset point !\n",__FUNCTION__, __LINE__);
                        cradle_head_control_data.setup_command_set = CRADLE_HEAD_CONTROL_NO_VALID_COMMAND;
                    }
                    break;
                default :
                    if_send_cradle_head_control_command = YES;
                    break;
            }
            //if (cradle_head_control_data.setup_command_set != CRADLE_HEAD_CONTROL_NO_VALID_COMMAND) 
            if (if_send_cradle_head_control_command == YES) 
            {
		printf_debug("FUNC[%s] LINE[%d]\tcradle_head_control_data.setup_command_set = %d\n",__FUNCTION__, __LINE__, cradle_head_control_data.setup_command_set);
		for (i = 0; i < 7; i++) 
                {
                    PELCO_P_Table[cradle_head_control_data.setup_command_set-1][7] ^= PELCO_P_Table[cradle_head_control_data.setup_command_set-1][i];
                }

                #ifdef DEBUG
                print_string(" ", PELCO_P_Table[cradle_head_control_data.setup_command_set-1], PELCO_P_UART_MAX_SNED_SIZE);
                #endif
                if (SendDataToCom(*com_fd, PELCO_P_Table[cradle_head_control_data.setup_command_set-1], PELCO_P_UART_MAX_SNED_SIZE) == -1)
                {
                    printf("FUNC[%s] LINE[%d]\tSend data to com error!\n",__FUNCTION__, __LINE__);
                    return -1;
                }
                //网络发送 设置或调用云台预置点失败,预置点值错误
                printf_debug("FUNC[%s] LINE[%d]\tSet cradle head command succeesfully!\n",__FUNCTION__, __LINE__);
                cradle_head_control_data.setup_command_set = CRADLE_HEAD_CONTROL_NO_VALID_COMMAND;
                if_send_cradle_head_control_command = NO;
            }
        }
        else if (cradle_head_control_data.cradle_head_protocol_type == Palco_D) 
        {
            PELCO_D_Table[cradle_head_control_data.setup_command_set-1][1] = cradle_head_control_data.cradle_head_address + 1;
            PELCO_D_Table[cradle_head_control_data.setup_command_set-1][6] = 0x00;
            switch(cradle_head_control_data.setup_command_set)
            {
                case CRADLE_HEAD_CONTROL_NO_VALID_COMMAND: 
                    usleep(10000);
                    break;
                case PTZ_UP:            //上
                case PTZ_DOWN:          //下
                    temp = (cradle_head_control_data.cradle_head_move_speed*0x3f)/0xff;
                    if (temp == 0) 
                        temp = 0x01;
                    else if (cradle_head_control_data.cradle_head_move_speed == 0xff) 
                        temp = 0x3f;

                    PELCO_D_Table[cradle_head_control_data.setup_command_set-1][5] = temp;
                    if_send_cradle_head_control_command = YES;
                    break;
                case PTZ_LEFT:          //左
                case PTZ_RIGHT:         //右
                    temp = (cradle_head_control_data.cradle_head_move_speed*0x3f)/0xff;
                    if (temp == 0) 
                        temp = 0x01;
                    else if (cradle_head_control_data.cradle_head_move_speed == 0xff) 
                        temp = 0x3f;

                    PELCO_D_Table[cradle_head_control_data.setup_command_set-1][4] = temp;
                    if_send_cradle_head_control_command = YES;
                    break;
                case PTZ_SET_PTZBIT:    //设置预置点
                case PTZ_CLE_PTZBIT:    //调用预置点
                    if (cradle_head_control_data.cradle_head_preset_point >= 1 && cradle_head_control_data.cradle_head_preset_point <= 32) 
                    {
                        PELCO_D_Table[cradle_head_control_data.setup_command_set-1][5] = cradle_head_control_data.cradle_head_preset_point;
                    }
                    else
                    {
                        //网络发送 设置或调用云台预置点失败,预置点值错误
                        printf("FUNC[%s] LINE[%d]\tInvalid cradle head preset point !\n",__FUNCTION__, __LINE__);
                        cradle_head_control_data.setup_command_set = CRADLE_HEAD_CONTROL_NO_VALID_COMMAND;
                    }
                    if_send_cradle_head_control_command = YES;
                    break;
                default :
                    if_send_cradle_head_control_command = YES;
                    break;
            }
            //if (cradle_head_control_data.setup_command_set != CRADLE_HEAD_CONTROL_NO_VALID_COMMAND) 
            if (if_send_cradle_head_control_command == YES) 
            {
               for (i = 0; i < 6; i++) 
                {
                    PELCO_D_Table[cradle_head_control_data.setup_command_set-1][6] += PELCO_D_Table[cradle_head_control_data.setup_command_set-1][i];
                }

                if (SendDataToCom(*com_fd, PELCO_D_Table[cradle_head_control_data.setup_command_set-1], PELCO_D_UART_MAX_SNED_SIZE) == -1)
                {
                    printf("FUNC[%s] LINE[%d]\tSend data to com error!\n",__FUNCTION__, __LINE__);
                    return -1;
                }
                //网络发送 设置或调用云台预置点失败,预置点值错误
                printf_debug("FUNC[%s] LINE[%d]\tSet cradle head command succeesfully!\n",__FUNCTION__, __LINE__);
                cradle_head_control_data.setup_command_set = CRADLE_HEAD_CONTROL_NO_VALID_COMMAND;
                if_send_cradle_head_control_command = NO;

		  //发送报警器控制失败状态回复-------------------------------------------------------------12.9.6  frank added
		//  send_act_cmd(NOSUPPORT, cradle_head_control_data.current_fd, SXC_ALARMCTL_DEV_CTL_CMD_V2, cradle_head_control_data.current_fd);
            }
        }
        else if (cradle_head_control_data.cradle_head_protocol_type == YAAN) 
        {
            YAAN_Table[cradle_head_control_data.setup_command_set-1][1] = cradle_head_control_data.cradle_head_address + 1;
            YAAN_Table[cradle_head_control_data.setup_command_set-1][7] = cradle_head_control_data.cradle_head_address + 1;
            YAAN_Table[cradle_head_control_data.setup_command_set-1][5] = 0x00;
            YAAN_Table[cradle_head_control_data.setup_command_set-1][11] = 0x00;
            switch(cradle_head_control_data.setup_command_set)
            {
                case CRADLE_HEAD_CONTROL_NO_VALID_COMMAND: 
                    usleep(10000);
                    break;
                case PTZ_UP:            //上
                case PTZ_DOWN:          //下
                    if (cradle_head_control_data.cradle_head_move_speed == 0x00) 
                    {
                        YAAN_Table[cradle_head_control_data.setup_command_set-1][10] = 0x01;
                    }
                    else
                    {
                        YAAN_Table[cradle_head_control_data.setup_command_set-1][9] = cradle_head_control_data.cradle_head_move_speed;
                    }
                    if_send_cradle_head_control_command = YES;
                    break;
                case PTZ_LEFT:          //左
                case PTZ_RIGHT:         //右
                    if (cradle_head_control_data.cradle_head_move_speed == 0x00) 
                    {
                        YAAN_Table[cradle_head_control_data.setup_command_set-1][10] = 0x01;
                    }
                    else
                    {
                        YAAN_Table[cradle_head_control_data.setup_command_set-1][10] = cradle_head_control_data.cradle_head_move_speed;
                    }
                    if_send_cradle_head_control_command = YES;
                    break;
                case PTZ_SET_PTZBIT:    //设置预置点
                case PTZ_CLE_PTZBIT:    //调用预置点
                    YAAN_Table[cradle_head_control_data.setup_command_set-1][3] = cradle_head_control_data.cradle_head_preset_point;
                    if_send_cradle_head_control_command = YES;
                    break;
                default :
                    if_send_cradle_head_control_command = YES;
                    break;
            }
            //if (cradle_head_control_data.setup_command_set != CRADLE_HEAD_CONTROL_NO_VALID_COMMAND) 
            if (if_send_cradle_head_control_command == YES) 
            {

                if((cradle_head_control_data.setup_command_set == PTZ_SET_PTZBIT) || (cradle_head_control_data.setup_command_set == PTZ_CLE_PTZBIT))
                {
                    YAAN_Table[cradle_head_control_data.setup_command_set-1][4] = 0;
                    for(i = 0; i < 4; i++)
                    {
                        YAAN_Table[cradle_head_control_data.setup_command_set-1][4] += YAAN_Table[cradle_head_control_data.setup_command_set-1][i];
                    }
                    if (SendDataToCom(*com_fd, YAAN_Table[cradle_head_control_data.setup_command_set-1], 5) == -1)
                    {
                        printf("FUNC[%s] LINE[%d]\tSend data to com error!\n",__FUNCTION__, __LINE__);
                        return -1;
                    }
                }
                else
                {
                    for(i = 6; i < 11; i++)
                    {
                        YAAN_Table[cradle_head_control_data.setup_command_set-1][11] += YAAN_Table[cradle_head_control_data.setup_command_set-1][i];
                    }         
                    if (SendDataToCom(*com_fd, YAAN_Table[cradle_head_control_data.setup_command_set-1], 5) == -1)
                    {
                        printf("FUNC[%s] LINE[%d]\tSend data to com error!\n",__FUNCTION__, __LINE__);
                        return -1;
                    }

                    for(i = 0; i < 5; i++)
                    {
                        YAAN_Table[cradle_head_control_data.setup_command_set-1][5] += YAAN_Table[cradle_head_control_data.setup_command_set-1][i];
                    }
                    if (SendDataToCom(*com_fd, YAAN_Table[cradle_head_control_data.setup_command_set-1], 5) == -1)
                    {
                        printf("FUNC[%s] LINE[%d]\tSend data to com error!\n",__FUNCTION__, __LINE__);
                        return -1;
                    }
                }
                //网络发送 设置或调用云台预置点失败,预置点值错误
                printf_debug("FUNC[%s] LINE[%d]\tSet cradle head command succeesfully!\n",__FUNCTION__, __LINE__);
                cradle_head_control_data.setup_command_set = CRADLE_HEAD_CONTROL_NO_VALID_COMMAND;
                if_send_cradle_head_control_command = YES;
            }
        }
        else
        {
            usleep(10000);
        }
    }
}

