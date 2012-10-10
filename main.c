#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/select.h>
#include <sys/stat.h>
#include <dirent.h>
#include <netdb.h>
#include <sys/ioctl.h>
#include <pthread.h>

#include "net_server.h"
#include "global.h"
#include "net.h"
#include "entrance_guard.h"
#include "ck2316_alarm.h"
#include "matrix_control.h"
#include "cradle_head_control.h"
#include "alarm_input_output.h"

#define DEBUG

#ifdef DEBUG
#define printf_debug(fmt, arg...)  printf(fmt, ##arg)
#else       
#define printf_debug(fmt,arg...) do{ } while (0)
#endif 

#define PATH_HOME "/home"

unsigned char net_recv_verify_code[] = {0xAA,0x55,0xAA,0x55,0xAA,0x55,0xAA,0x55,0xAA,0x55,0xAA,0x55,0xAA,0x55,0xAA};

int flag = 0;
int server_sock_tcp, client_sock_tcp;
int if_have_net_client_connect = NO;

void *pthread_temp_test(void *arg)
{
    while (1) 
    {
    }

    return (void *)1;
}

void *pthread_get_local_ip(void *arg)
{
    while (1) 
    {
        sleep(500);
        if (get_local_ip(ip, mac) == 0) 
        {
            printf_debug("local ip: %s\n",ip);
            printf_debug("local mac: %s\n",mac);
        }
    }

    return (void *)1;
}

#if 0
void init_serial_pamater(void)
{
    int i = 0;
    int temp = 0;
    void *uart_device_data[MAX_UART_DEV_NUM];
    
#if 1
    FILE *fp_dev_config_file;
    uart_device_data[0] = &entrance_guard_data;
    uart_device_data[1] = &matrix_control_data;
    uart_device_data[2] = &cradle_head_control_data;
    uart_device_data[3] = NULL;
    uart_device_data[4] = NULL;
    uart_device_data[5] = &ck2316_alarm_data;
    uart_device_data[6] = NULL;

    if ((fp_dev_config_file = fopen(UART_DEV_CONFIG_FILE, "a+")) == NULL)
    {
        printf("FUNC[%s] LINE[%d]\tOpen %s error!\n",__FUNCTION__, __LINE__, UART_DEV_CONFIG_FILE);
        exit(1);
    }
    fseek(fp_dev_config_file, 0, SEEK_SET);

    for (i = 0; i < MAX_UART_DEV_NUM; i++) 
    {
        if (uart_device_data[i] != NULL) 
        {
            if ((fscanf(fp_dev_config_file, "%d ",&temp)) == 1)
            {
                (entrance_guard_arg)uart_device_data[i]->entrance_guard_serial_pamater.byPathNo = temp;
            }
            if ((fscanf(fp_dev_config_file, "%d ",&entrance_guard_data.entrance_guard_serial_pamater.serialAttr.dwBaudRate)) != 1)
            {
                entrance_guard_data.entrance_guard_serial_pamater.serialAttr.dwBaudRate = ENTRANCE_GUARD_BOARD;
            }
            if ((fscanf(fp_dev_config_file, "%d ",&entrance_guard_data.entrance_guard_serial_pamater.serialAttr.byDataBit)) != 1)
            {
                entrance_guard_data.entrance_guard_serial_pamater.serialAttr.byDataBit = ENTRANCE_GUARD_UART_DATA_BIT;
            }
            if ((fscanf(fp_dev_config_file, "%d ",&entrance_guard_data.entrance_guard_serial_pamater.serialAttr.byStopBit)) != 1)
            {
                entrance_guard_data.entrance_guard_serial_pamater.serialAttr.byStopBit = ENTRANCE_GUARD_UART_STOP_BIT;
            }
            if ((fscanf(fp_dev_config_file, "%d ",&entrance_guard_data.entrance_guard_serial_pamater.serialAttr.byParity)) != 1)
            {
                entrance_guard_data.entrance_guard_serial_pamater.serialAttr.byParity = ENTRANCE_GUARD_UART_CHECK_BIT;
            }
            if ((fscanf(fp_dev_config_file, "%d\n",&entrance_guard_data.entrance_guard_serial_pamater.serialAttr.byFlowcontrol)) != 1)
            {
                entrance_guard_data.entrance_guard_serial_pamater.serialAttr.byFlowcontrol = 0;
            }
        }
    }
    fclose(fp_dev_config_file);
    if ((fp_dev_config_file = fopen(UART_DEV_CONFIG_FILE, "w+")) == NULL)
    {
        printf("FUNC[%s] LINE[%d]\tOpen %s error!\n",__FUNCTION__, __LINE__, UART_DEV_CONFIG_FILE);
        exit(1);
    }
    printf_debug("\n\nUart devive attribution: %d %d %d %d %d %d\n\n",entrance_guard_data.entrance_guard_serial_pamater.byPathNo, entrance_guard_data.entrance_guard_serial_pamater.serialAttr.dwBaudRate, entrance_guard_data.entrance_guard_serial_pamater.serialAttr.byDataBit, entrance_guard_data.entrance_guard_serial_pamater.serialAttr.byStopBit, entrance_guard_data.entrance_guard_serial_pamater.serialAttr.byParity, entrance_guard_data.entrance_guard_serial_pamater.serialAttr.byFlowcontrol);
    fprintf(fp_dev_config_file, "%d %d %d %d %d %d\n",entrance_guard_data.entrance_guard_serial_pamater.byPathNo, entrance_guard_data.entrance_guard_serial_pamater.serialAttr.dwBaudRate, entrance_guard_data.entrance_guard_serial_pamater.serialAttr.byDataBit, entrance_guard_data.entrance_guard_serial_pamater.serialAttr.byStopBit, entrance_guard_data.entrance_guard_serial_pamater.serialAttr.byParity, entrance_guard_data.entrance_guard_serial_pamater.serialAttr.byFlowcontrol);
    fflush(fp_dev_config_file);
    #endif
}
#endif


int main(int argc, char *argv[])
{
    int on = 1;
    #ifdef DEBUG
    time_t tm;
    struct tm *t;
    #endif
    int ret = 0;
    int i = 0;
    //char *receive_ip = NULL;
    //char *receive_mac = NULL;

    //socklen_t client_len;  
    struct sockaddr_in server_tcp,client_tcp;

    pthread_t tid_get_local_ip;
    pthread_t tid_entrance_guard;
    pthread_t tid_ck2316_alarm;
    pthread_t tid_matrix_control;
    pthread_t tid_cradle_head_control;
    pthread_t tid_alarm_input_output;
    pthread_t tid_handshake;
    pthread_t tid_temp_test;

    while (get_local_ip(ip, mac) != 0) 
    {
        printf("FUNC[%s] LINE[%d]\tCan not get local ip !\n",__FUNCTION__, __LINE__);
        sleep(10);
    }
    printf("local ip: %s\n",ip);
    printf("local mac: %s\n",mac);

    #if 1
    //创建获取本地IP和MAC地址的线程
    if (pthread_create(&tid_get_local_ip, NULL, pthread_get_local_ip, NULL) != 0) 
    {
        printf("FUNC[%s] LINE[%d]\tCan't create get local ip thread !\n",__FUNCTION__, __LINE__);
        exit(1);
    }
    #endif

    init_serial_dev_parameter();

    #if 0
    //创建与门禁通信的线程
    if (pthread_create(&tid_entrance_guard, NULL, pthread_entrance_guard, NULL) != 0) 
    {
        printf("FUNC[%s] LINE[%d]\tCan't create entrance guard thread !\n",__FUNCTION__, __LINE__);
        exit(1);
    }
    #endif

    #if 1
    //创建与报警器通信的线程
    if (pthread_create(&tid_ck2316_alarm, NULL, pthread_ck2316_alarm, NULL) != 0) 
    {
        printf("FUNC[%s] LINE[%d]\tCan't create ck2316 alarm thread !\n",__FUNCTION__, __LINE__);
        exit(1);
    }
    #else
        #if 0
    if (pthread_create(&tid_temp_test, NULL, pthread_temp_test, NULL) != 0) 
    {
        printf("FUNC[%s] LINE[%d]\tCan't create temp test thread !\n",__FUNCTION__, __LINE__);
        exit(1);
    }
        #endif
    #endif

    #if 0
    //创建与矩阵通信的线程
    if (pthread_create(&tid_matrix_control, NULL, pthread_matrix_control, NULL) != 0) 
    {
        printf("FUNC[%s] LINE[%d]\tCan't create maxtrix control thread !\n",__FUNCTION__, __LINE__);
        exit(1);
    }
    #endif

    #if 0
    //创建与云台通信的线程
    if (pthread_create(&tid_cradle_head_control, NULL, pthread_cradle_head_control, NULL) != 0) 
    {
        printf("FUNC[%s] LINE[%d]\tCan't create cradle head control thread !\n",__FUNCTION__, __LINE__);
        exit(1);
    }
    #endif

    #if 0
    //创建与报警输入输出通信的线程
    if (pthread_create(&tid_alarm_input_output, NULL, pthread_alarm_input_output, NULL) != 0) 
    {
        printf("FUNC[%s] LINE[%d]\tCan't create alarm input and output thread !\n",__FUNCTION__, __LINE__);
        exit(1);
    }
    #endif

#if 1
start:

#ifdef DEBUG 
    tm = time(NULL);
    t = localtime(&tm);
    //printf("FUNC[%s] LINE[%d]\tStartTime: %s\n",__FUNCTION__, __LINE__, asctime(tblock));
    printf("FUNC[%s] LINE[%d]\tStartTime: %04d-%02d-%02d  %02d:%02d:%02d\n",__FUNCTION__, __LINE__,t->tm_year + 1900, t->tm_mon + 1, t->tm_mday, t->tm_hour, t->tm_min, t->tm_sec);
#endif

    if ((server_sock_tcp = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("FUNC[%s] LINE[%d]\t",__FUNCTION__, __LINE__);
        fprintf(stderr, "%s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    else
    {
        printf_debug("FUNC[%s] LINE[%d]\tTCP create socket ok !\n",__FUNCTION__, __LINE__);
    }
    setsockopt(server_sock_tcp, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
    bzero(&server_tcp, sizeof(server_tcp));
    server_tcp.sin_family = AF_INET;
    server_tcp.sin_port = htons(SERVER_PORT_TCP);
    server_tcp.sin_addr.s_addr = htons(INADDR_ANY);
    if(bind(server_sock_tcp, (struct sockaddr *)&server_tcp, sizeof(server_tcp)) < 0)
    {
        fprintf(stderr, "%s\n", strerror(errno));
        printf("FUNC[%s] LINE[%d]\tTcp bind error !\n",__FUNCTION__, __LINE__);
        exit(EXIT_FAILURE);
    }
    else
    {
        printf_debug("FUNC[%s] LINE[%d]\tTCP bind addr ok !\n",__FUNCTION__, __LINE__);
    }

    #if 0
    //set socket as non block
    int opts;
    if ((opts = fcntl(server_sock_tcp, F_GETFL)) < 0)
    {
        perror("fcntl");
        exit(errno);
    }
    opts |= O_NONBLOCK;
    //if (fcntl(server_sock_tcp, F_SETFL, O_NONBLOCK) == -1) 
    if (fcntl(server_sock_tcp, F_SETFL, opts) == -1) 
    {
        perror("fcntl");
        exit(errno);
    }
    #endif

    if(listen(server_sock_tcp, 3) < 0)
    {
        printf("FUNC[%s] LINE[%d]\t",__FUNCTION__, __LINE__);
        fprintf(stderr, "%s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    else
    {
        printf_debug("FUNC[%s] LINE[%d]\tTCP listen ok !\n",__FUNCTION__, __LINE__);
        printf_debug("FUNC[%s] LINE[%d]\tServer tcp port: %d\n",__FUNCTION__, __LINE__,SERVER_PORT_TCP);
    }

    //等待客户端连接
    printf_debug("FUNC[%s] LINE[%d]\tWaiting for client connect !\n",__FUNCTION__, __LINE__);
    if (waiting_for_client_connect(&server_sock_tcp, &server_tcp, &client_sock_tcp, &client_tcp) != YES) 
    {
        goto tcp_end;
    }

    if_have_net_client_connect = YES;

    //创建给客户端发送网络握手包的线程
    if (pthread_create(&tid_handshake, NULL, pthread_handshake, &client_sock_tcp) != 0) 
    {
        printf("FUNC[%s] LINE[%d]\tCan't create handshake thread !\n",__FUNCTION__, __LINE__);
        exit(1);
    }

    while(1)
    {
        //接收客户端发过来的数据
        ret = tcp_server_recv(&server_sock_tcp, &client_sock_tcp);

        if (ret != NET_RECV_BUFFER_SIZE) 
        {
            printf_debug("FUNC[%s] LINE[%d]\tReceived %d bytes data, didn't receive enough data\t\n",__FUNCTION__, __LINE__,ret);
            break;
        }
        else 
        {

            //printf_debug("length: %d   net_recv_data: 0x%x 0x%x, 0x%x 0x%x\n",ret, net_recv_buffer[28], net_recv_buffer[29], net_recv_buffer[30], net_recv_buffer[31]);
            #if 0
            //printf_debug("%s",net_recv_buffer);
            #else
            for (i = 9; i < ret; i++) 
            {
                printf_debug("0x%x ",net_recv_buffer[i]);
            }
            printf("\n");
            #endif
            if (strncmp(net_recv_buffer, net_recv_verify_code, 9) == 0) 
            {
                switch(net_recv_buffer[14])
                {
                    case 0x00: //没有指令,报警输入输出正常工作
                        alarm_input_output_data.setup_command_set = ALARM_INPUT_OUTPUT_NORMAL_OPERATION;
                        break;
                    case 0x01: //设置报警联动对应关系
                        if (alarm_input_output_data.real_time_alarm_output_objcet) 
                        {
                            //网络发送请取消所有报警输出，然后再设置报警联动对应关系
                            printf_debug("FUNC[%s] LINE[%d]\tPlease cancel alarm output, then try it again!\n",__FUNCTION__, __LINE__);
                        }
                        else
                        {
                            if (net_recv_buffer[12] < 1 || net_recv_buffer[12] > MAX_ALARM_LINKAGE_INPUT_OBJECT) 
                            {
                                //网络发送 无效参数
                                printf_debug("FUNC[%s] LINE[%d]\tInvalid arg alarm_linkage_input_object, it should be 1 to 9\n",__FUNCTION__, __LINE__);
                            }
                            else
                            {
                                alarm_input_output_data.alarm_linkage_input_object = net_recv_buffer[12];
                                alarm_input_output_data.alarm_input_linkage_output_object[alarm_input_output_data.alarm_linkage_input_object-1] = net_recv_buffer[13];
                                alarm_input_output_data.setup_command_set = ALARM_INPUT_OUTPUT_SET_LINKAGE_INFO;
                            }
                        }
                        break;
                    case 0x02: //获取报警联动对应关系
                        if (net_recv_buffer[12] < 1 || net_recv_buffer[12] > MAX_ALARM_LINKAGE_INPUT_OBJECT) 
                        {
                            //网络发送 无效参数
                            printf_debug("FUNC[%s] LINE[%d]\tInvalid arg alarm_linkage_input_object, it should be 1 to 9\n",__FUNCTION__, __LINE__);
                        }
                        else
                        {
                            alarm_input_output_data.alarm_linkage_input_object = net_recv_buffer[12];
                            //网络发送 获取报警联动对应关系
                            printf_debug("FUNC[%s] LINE[%d]\tChannel %d: 0x%X\n",__FUNCTION__, __LINE__, alarm_input_output_data.alarm_linkage_input_object, alarm_input_output_data.alarm_input_linkage_output_object[alarm_input_output_data.alarm_linkage_input_object-1]);
                        }
                        break;
                    case 0x03: //设置报警输出
                            alarm_input_output_data.set_and_cancel_linkage_alarm_channel = net_recv_buffer[13];
                            alarm_input_output_data.setup_command_set = ALARM_INPUT_OUTPUT_SET_AND_CANCEL_LINKAGE_ALARM;
                        break;
                    case 0x04: //获取报警输出
                            //网络发送 报警输出 
                            printf_debug("FUNC[%s] LINE[%d]\tAlarm output status: 0x%X\n",__FUNCTION__, __LINE__, alarm_input_output_data.real_time_alarm_output_objcet);
                        break;
                    case 0x05: //恢复默认参数
                        if (alarm_input_output_data.real_time_alarm_output_objcet) 
                        {
                            //网络发送请取消所有报警输出，然后再设置报警联动对应关系
                            printf_debug("FUNC[%s] LINE[%d]\tPlease cancel alarm output, then try it again!\n",__FUNCTION__, __LINE__);
                        }
                        else
                        {
                            alarm_input_output_data.setup_command_set = ALARM_INPUT_OUTPUT_RESTORE_TO_DEFAULT;
                        }
                        break;
                    case 0x06: //设置报警输出持续时间
                        if (alarm_input_output_data.real_time_alarm_output_objcet) 
                        {
                            //网络发送请取消所有报警输出，然后再设置报警联动对应关系
                            printf_debug("FUNC[%s] LINE[%d]\tPlease cancel alarm output, then try it again!\n",__FUNCTION__, __LINE__);
                        }
                        else
                        {
                                //if (net_recv_buffer[11] < 0 || net_recv_buffer[11] > MAX_ALARM_INPUT_LINKAGE_OUTPUT_OBJECT) 
                                if (net_recv_buffer[11] > MAX_ALARM_INPUT_LINKAGE_OUTPUT_OBJECT) 
                                {
                                    //网络发送 无效参数
                                    printf_debug("FUNC[%s] LINE[%d]\tInvalid arg alarm_linkage_output_object, it should be 0 to 8\n",__FUNCTION__, __LINE__);
                                
                                }
                                else
                                {
                                    alarm_input_output_data.alarm_linkage_output_object = net_recv_buffer[11];
                                    alarm_input_output_data.alarm_duration = 60*net_recv_buffer[9] + net_recv_buffer[10];
                                    if (alarm_input_output_data.alarm_duration > MAX_ALARM_DURATION) 
                                    {
                                        alarm_input_output_data.alarm_duration = MAX_ALARM_DURATION;
                                    }
                                    alarm_input_output_data.setup_command_set = ALARM_INPUT_OUTPUT_SET_ALARM_DURATION;
                                }
                        }
                        break;
                    case 0x07: //获取报警输出持续时间
                        if (net_recv_buffer[11] < 1 || net_recv_buffer[11] > MAX_ALARM_INPUT_LINKAGE_OUTPUT_OBJECT) 
                        {
                            //网络发送 无效参数
                            printf_debug("FUNC[%s] LINE[%d]\tInvalid arg alarm_linkage_output_object, it should be 1 to 8\n",__FUNCTION__, __LINE__);
                        
                        }
                        else
                        {
                            alarm_input_output_data.alarm_linkage_output_object = net_recv_buffer[11];
                            alarm_input_output_data.setup_command_set = ALARM_INPUT_OUTPUT_GET_ALARM_DURATION;
                        }
                        break;
                    default :
                        //网络发送无效命令
                        printf_debug("FUNC[%s] LINE[%d]\tAlarm input and output invalid net command!\n",__FUNCTION__, __LINE__);
                        alarm_input_output_data.setup_command_set = ALARM_INPUT_OUTPUT_NORMAL_OPERATION;
                        break;
                }
                switch(net_recv_buffer[19])
                {
                    case 0x00: //没有指令
                        cradle_head_control_data.setup_command_set = CRADLE_HEAD_CONTROL_NO_VALID_COMMAND;
                        break;
                    case 0x01: //以SS速度向上
                        cradle_head_control_data.cradle_head_protocol_type = net_recv_buffer[18];
                        cradle_head_control_data.cradle_head_address = net_recv_buffer[17];
                        cradle_head_control_data.cradle_head_move_speed = net_recv_buffer[16];
                        cradle_head_control_data.setup_command_set = PTZ_UP;
                        break;
                    case 0x02: //以SS速度向下
                        cradle_head_control_data.cradle_head_protocol_type = net_recv_buffer[18];
                        cradle_head_control_data.cradle_head_address = net_recv_buffer[17];
                        cradle_head_control_data.cradle_head_move_speed = net_recv_buffer[16];
                        cradle_head_control_data.setup_command_set = PTZ_DOWN;
                        break;
                    case 0x03: //以SS速度向左
                        cradle_head_control_data.cradle_head_protocol_type = net_recv_buffer[18];
                        cradle_head_control_data.cradle_head_address = net_recv_buffer[17];
                        cradle_head_control_data.cradle_head_move_speed = net_recv_buffer[16];
                        cradle_head_control_data.setup_command_set = PTZ_LEFT;
                        break;
                    case 0x04: //以SS速度向右
                        cradle_head_control_data.cradle_head_protocol_type = net_recv_buffer[18];
                        cradle_head_control_data.cradle_head_address = net_recv_buffer[17];
                        cradle_head_control_data.cradle_head_move_speed = net_recv_buffer[16];
                        cradle_head_control_data.setup_command_set = PTZ_RIGHT;
                        break;
                    case 0x05: //光圈调节，变大
                        cradle_head_control_data.cradle_head_protocol_type = net_recv_buffer[18];
                        cradle_head_control_data.cradle_head_address = net_recv_buffer[17];
                        cradle_head_control_data.setup_command_set = PTZ_APERTURE_BIG;
                        break;
                    case 0x06: //光圈调节，缩小
                        cradle_head_control_data.cradle_head_protocol_type = net_recv_buffer[18];
                        cradle_head_control_data.cradle_head_address = net_recv_buffer[17];
                        cradle_head_control_data.setup_command_set = PTZ_APERTURE_SMALL;
                        break;
                    case 0x07: //聚焦近
                        cradle_head_control_data.cradle_head_protocol_type = net_recv_buffer[18];
                        cradle_head_control_data.cradle_head_address = net_recv_buffer[17];
                        cradle_head_control_data.setup_command_set = PTZ_FOCUS_IN;
                        break;
                    case 0x08: //聚焦远
                        cradle_head_control_data.cradle_head_protocol_type = net_recv_buffer[18];
                        cradle_head_control_data.cradle_head_address = net_recv_buffer[17];
                        cradle_head_control_data.setup_command_set = PTZ_FOCUS_OUT;
                        break;
                    case 0x09: //变倍长(特写, 放大)
                        cradle_head_control_data.cradle_head_protocol_type = net_recv_buffer[18];
                        cradle_head_control_data.cradle_head_address = net_recv_buffer[17];
                        cradle_head_control_data.setup_command_set = PTZ_ZOOM_IN;
                        break;
                    case 0x0a: //变倍短(广角, 缩小)
                        cradle_head_control_data.cradle_head_protocol_type = net_recv_buffer[18];
                        cradle_head_control_data.cradle_head_address = net_recv_buffer[17];
                        cradle_head_control_data.setup_command_set = PTZ_ZOOM_OUT;
                        break;
                    case 0x0b: //开始自动焦距（自动倍率）
                        cradle_head_control_data.cradle_head_protocol_type = net_recv_buffer[18];
                        cradle_head_control_data.cradle_head_address = net_recv_buffer[17];
                        cradle_head_control_data.setup_command_set = PTZ_AUTO_ZOOM;
                        break;
                    case 0x0c: //开自动调焦
                        cradle_head_control_data.cradle_head_protocol_type = net_recv_buffer[18];
                        cradle_head_control_data.cradle_head_address = net_recv_buffer[17];
                        cradle_head_control_data.setup_command_set = PTZ_AUTO_FOCUS;
                        break;
                    case 0x0d: //开自动光圈
                        cradle_head_control_data.cradle_head_protocol_type = net_recv_buffer[18];
                        cradle_head_control_data.cradle_head_address = net_recv_buffer[17];
                        cradle_head_control_data.setup_command_set = PTZ_AUTO_APERTURE;
                        break;
                    case 0x0e: //设置预置点
                        cradle_head_control_data.cradle_head_protocol_type = net_recv_buffer[18];
                        cradle_head_control_data.cradle_head_address = net_recv_buffer[17];
                        cradle_head_control_data.cradle_head_preset_point = net_recv_buffer[15];
                        cradle_head_control_data.setup_command_set = PTZ_SET_PTZBIT;
                        break;
                    case 0x0f: //调用预置点
                        cradle_head_control_data.cradle_head_protocol_type = net_recv_buffer[18];
                        cradle_head_control_data.cradle_head_address = net_recv_buffer[17];
                        cradle_head_control_data.cradle_head_preset_point = net_recv_buffer[15];
                        cradle_head_control_data.setup_command_set = PTZ_CLE_PTZBIT;
                        break;
                    case 0x10: //停止所有连续量（镜头，云台）动作
                        cradle_head_control_data.cradle_head_protocol_type = net_recv_buffer[18];
                        cradle_head_control_data.cradle_head_address = net_recv_buffer[17];
                        cradle_head_control_data.setup_command_set = PTZ_STOP_ALL;
                        break;
                    case 0x11: //辅助功能1开
                        cradle_head_control_data.cradle_head_protocol_type = net_recv_buffer[18];
                        cradle_head_control_data.cradle_head_address = net_recv_buffer[17];
                        cradle_head_control_data.setup_command_set = AUX_PWRON1;
                        break;
                    case 0x12: //辅助功能1关
                        cradle_head_control_data.cradle_head_protocol_type = net_recv_buffer[18];
                        cradle_head_control_data.cradle_head_address = net_recv_buffer[17];
                        cradle_head_control_data.setup_command_set = AUX_PWROFF1;
                        break;
                    case 0x13: //辅助功能2开
                        cradle_head_control_data.cradle_head_protocol_type = net_recv_buffer[18];
                        cradle_head_control_data.cradle_head_address = net_recv_buffer[17];
                        cradle_head_control_data.setup_command_set = AUX_PWRON2;
                        break;
                    case 0x14: //辅助功能2关
                        cradle_head_control_data.cradle_head_protocol_type = net_recv_buffer[18];
                        cradle_head_control_data.cradle_head_address = net_recv_buffer[17];
                        cradle_head_control_data.setup_command_set = AUX_PWROFF2;
                        break;
                    default :
                        //网络发送无效命令
                        printf_debug("FUNC[%s] LINE[%d]\tCradle head invalid net command!\n",__FUNCTION__, __LINE__);
                        cradle_head_control_data.setup_command_set = CRADLE_HEAD_CONTROL_NO_VALID_COMMAND;
                        break;
                }
                switch(net_recv_buffer[22])
                {
                    case 0x00: //没有指令
                        matrix_control_data.setup_command_set = MATRIX_CONTROL_NO_VALID_COMMAND;
                        break;
                    case 0x01: //矩阵切换
                        matrix_control_data.setup_command_set = MATRIX_SWITCHOVER_COMMAND;
                        matrix_control_data.matrix_current_switchover_in_value = net_recv_buffer[20];
                        matrix_control_data.matrix_current_switchover_out_value = net_recv_buffer[21];
                        break;
                    case 0x02: //矩阵单通道取消切换
                        matrix_control_data.setup_command_set = MATRIX_CANCEL_SWITCHOVER_COMMAND;
                        matrix_control_data.matrix_current_switchover_in_value = net_recv_buffer[20];
                        matrix_control_data.matrix_current_switchover_out_value = net_recv_buffer[21];
                        break;
                    case 0x03: //矩阵所有通道取消切换
                        matrix_control_data.setup_command_set = MATRIX_CANCEL_ALL_SWITCHOVER_COMMAND;
                        break;
                    default :
                        //网络发送无效命令
                        printf_debug("FUNC[%s] LINE[%d]\tMatrix invalid net command!\n",__FUNCTION__, __LINE__);
                        ck2316_alarm_data.setup_command_set = CK2316_NO_VALID_COMMAND;
                        break;
                }
                switch(net_recv_buffer[29])
                {
                    case 0x00: //没有指令
                        ck2316_alarm_data.setup_command_set = CK2316_NO_VALID_COMMAND;
                        break;
                    case 0x01: //系统布防
                        ck2316_alarm_data.setup_command_set = CK2316_SYSTEM_DEFENCE;
                        break;
                    case 0x02: //系统撤防
                        ck2316_alarm_data.setup_command_set = CK2316_SYSTEM_ABANDON;
                        break;
                    case 0x03: //消除报警记忆
                        ck2316_alarm_data.setup_command_set = CK2316_ELIMINATE_ALARM_MEMORY;
                        break;
                    case 0x04: //旁路防区
                        if (net_recv_buffer[28] > 0 && net_recv_buffer[28] <= 16) //16个防区
                        {
                            bypass_defence_area_code[5][2] = net_recv_buffer[28]/10;
                            bypass_defence_area_code[6][2] = net_recv_buffer[28]%10;
                            ck2316_alarm_data.setup_command_set = CK2316_BYPASS_DEFENCE_AREA;
                        }
                        else
                        {
                            //网络发送 旁路防区失败，错误的防区号
                            printf_debug("\n\nWarning ! CK2316 bypass defence area failed,wrong Defence area number!\n\n");
                        }
                        break;
                    case 0x05: //CK2316报警主机复位
                        ck2316_alarm_data.setup_command_set = CK2316_ALARM_HOST_RESET;
                        break;
                    case 0x06: //设置模拟键盘地址
                        //if (net_recv_buffer[23] >= 0x00 && net_recv_buffer[23] <= 0x0F) 
                        if (net_recv_buffer[23] <= 0x0F) 
                        {
                            ck2316_alarm_data.ck2316_simulate_keyboard_address = net_recv_buffer[23];
                            ck2316_simulate_keyboard_address_setup(ck2316_alarm_data.ck2316_simulate_keyboard_address);
                            //网络发送 地址码设置成功
                            printf_debug("\n\nCK2316 change keyboard address as: 0x%02X\n\n",ck2316_alarm_data.ck2316_simulate_keyboard_address);
                            //ck2316_alarm_data.setup_command_set = CK2316_SIMULATE_KEYBOARD_ADDRESS_SETUP;
                        }
                        else
                        {
                            //网络发送 地址码设置失败，错误的地址码
                            printf_debug("\n\nWarning ! CK2316 change keyboard address failed,wrong address !\n\n");
                        }
                        break;
                    case 0x07: //设置模拟键盘用户密码
                        //if (net_recv_buffer[24] >= 0x00 && net_recv_buffer[24] <= 0x0F && net_recv_buffer[25] >= 0x00 && net_recv_buffer[25] <= 0x0F && net_recv_buffer[26] >= 0x00 && net_recv_buffer[26] <= 0x0F && net_recv_buffer[27] >= 0x00 && net_recv_buffer[27] <= 0x0F) 
                        if (net_recv_buffer[24] <= 0x0F && net_recv_buffer[25] <= 0x0F && net_recv_buffer[26] <= 0x0F && net_recv_buffer[27] <= 0x0F) 
                        {
                            ck2316_alarm_data.ck2316_user_password[0] = net_recv_buffer[24];
                            ck2316_alarm_data.ck2316_user_password[1] = net_recv_buffer[25];
                            ck2316_alarm_data.ck2316_user_password[2] = net_recv_buffer[26];
                            ck2316_alarm_data.ck2316_user_password[3] = net_recv_buffer[27];
                            ck2316_user_password_setup(ck2316_alarm_data.ck2316_user_password);
                            //网络发送 密码设置成功
                            printf_debug("\n\nCK2316 change keyboard password as: %02d %02d %02d %02d\n\n",ck2316_alarm_data.ck2316_user_password[0], ck2316_alarm_data.ck2316_user_password[1], ck2316_alarm_data.ck2316_user_password[2], ck2316_alarm_data.ck2316_user_password[3]);
                            //ck2316_alarm_data.setup_command_set = CK2316_SIMULATE_KEYBOARD_PASSWORD_SETUP;
                        }
                        else
                        {
                            //网络发送 密码设置失败，错误的密码值
                            printf_debug("\n\nWarning ! CK2316 change password failed,wrong password value!\n\n");
                        }
                        break;
                    case 0x08: //获取布撤防状态（布防，撤防）
                        if (ck2316_alarm_data.if_ck2316_alive == YES) 
                        {
                            //navy 网络发送
                            printf_debug("CK2316 Defence status\t0x%02X\n",ck2316_alarm_data.ck2316_defence_status);
                        }
                        else
                        {
                            //navy 网络发送 报警主机不在线
                            ck2316_alarm_data.setup_command_set = CK2316_NO_VALID_COMMAND;
                            printf_debug("FUNC[%s] LINE[%d]\tWarning ! Timeout, ck2316 is not alive !\n",__FUNCTION__, __LINE__);
                        }
                        break;
                    case 0x09: //获取全部信息
                        if (ck2316_alarm_data.if_ck2316_alive == YES) 
                        {
                            //navy 网络发送
                            printf_debug("CK2316 Defence area status\t0x%02X%02X 0x%02X%02X 0x%02X%02X 0x%02X 0x%02X 0x%02X\n",ck2316_alarm_data.ck2316_defence_area_alarm_memory_value[1], ck2316_alarm_data.ck2316_defence_area_alarm_memory_value[0], ck2316_alarm_data.ck2316_defence_area_real_time_alarm_value[1], ck2316_alarm_data.ck2316_defence_area_real_time_alarm_value[0], ck2316_alarm_data.ck2316_defence_area_bypass_value[1], ck2316_alarm_data.ck2316_defence_area_bypass_value[0], ck2316_alarm_data.ck2316_defence_status, ck2316_alarm_data.ck2316_defence_area_alarm_status, ck2316_alarm_data.ck2316_simulate_keyboard_address);
                        }
                        else
                        {
                            //navy 网络发送 报警主机不在线
                            ck2316_alarm_data.setup_command_set = CK2316_NO_VALID_COMMAND;
                            printf_debug("FUNC[%s] LINE[%d]\tWarning ! Timeout, ck2316 is not alive !\n",__FUNCTION__, __LINE__);
                        }
                        break;
                    default :
                        //网络发送无效命令
                        printf_debug("FUNC[%s] LINE[%d]\tCK2316 invalid net command!\n",__FUNCTION__, __LINE__);
                        ck2316_alarm_data.setup_command_set = CK2316_NO_VALID_COMMAND;
                        break;
                }
                switch(net_recv_buffer[31])
                {
                    case 0x00: //没有指令
                        entrance_guard_data.setup_command_set = ENTRANCE_GUARD_NO_VALID_COMMAND;
                        break;
                    case 0x01: //开门 
                        entrance_guard_data.setup_command_set = ENTRANCE_GUARD_OPEN_DOOR;
                        break;
                    case 0x02: //关门
                        entrance_guard_data.setup_command_set = ENTRANCE_GUARD_CLOSE_DOOR;
                        break;
                    case 0x03: //门锁继电器常开
                        entrance_guard_data.setup_command_set = ENTRANCE_GUARD_RELAY_OPEN_DOOR;
                        break;
                    case 0x04: //门锁继电器常闭
                        entrance_guard_data.setup_command_set = ENTRANCE_GUARD_RELAY_CLOSE_DOOR;
                        break;
                    case 0x05: //客户端设置门保持时间
                        entrance_guard_data.setup_command_set = ENTRANCE_GUARD_CLIENT_SET_DOOR_HOLD_TIME;
                        entrance_guard_data.client_set_door_hold_time = net_recv_buffer[30];
                        break;
                    case 0x06: //开门按钮设置门保持时间
                        entrance_guard_data.setup_command_set = ENTRANCE_GUARD_BUTTON_SET_DOOR_HOLD_TIME;
                        entrance_guard_data.button_set_door_hold_time = net_recv_buffer[30];
                        break;
                    case 0x07: //获取普通消息
                        //entrance_guard_data.setup_command_set = ENTRANCE_GUARD_GET_NORMAL_MESSAGE;
                        break;
                    case 0x08: //获取报警消息
                        //entrance_guard_data.setup_command_set = ENTRANCE_GUARD_GET_ALARM_MESSAGE;
                    case 0x09: //门磁状态设置(常开)
                        entrance_guard_data.setup_command_set = ENTRANCE_GUARD_DOOR_CONTACT_NORMALLY_OPEN;
                        break;
                    case 0x0a: //门磁状态设置(常闭)(默认)
                        entrance_guard_data.setup_command_set = ENTRANCE_GUARD_DOOR_CONTACT_NORMALLY_CLOSE;
                        break;
                    case 0x0b: //门磁取消报警状态
                        //entrance_guard_data.setup_command_set = ENTRANCE_GUARD_DOOR_CONTACT_NORMALLY_CLOSE;
                        entrance_guard_data.if_has_entrance_guard_alarm = NO;
                        printf_debug("FUNC[%s] LINE[%d]\tEntrance guard succeed to cancel alarm!\n",__FUNCTION__, __LINE__);
                        //navy 网络发送 门磁取消报警成功
                        entrance_guard_data.setup_command_set = ENTRANCE_GUARD_CANCEL_ALARM_STATUS;
                        break;
                    default:   //无效指令
                        //网络发送无效命令
                        printf_debug("FUNC[%s] LINE[%d]\tEntrance guard invalid net command!\n",__FUNCTION__, __LINE__);
                        break;
                }
                #if 0
                pthread_mutex_lock(&counter_mutex);
                send(client_sock_tcp, "opendoor\r\n", 10, 0);
                pthread_mutex_unlock(&counter_mutex);
                entrance_guard_data.if_open_or_close_door = OPEN_DOOR;
                entrance_guard_data.if_is_setup_command = YES;

                pthread_mutex_lock(&counter_mutex);
                send(client_sock_tcp, "closdoor\r\n", 10, 0);
                pthread_mutex_unlock(&counter_mutex);
                entrance_guard_data.if_open_or_close_door = CLOSE_DOOR;
                entrance_guard_data.if_is_setup_command = YES;
                #endif
            }
        }
    }

    //pthread_detach(tid_handshake);
    //pthread_join(tid_handshake, &tret);
    pthread_cancel(tid_handshake);
tcp_end:
    if_have_net_client_connect = NO;
    close(server_sock_tcp);
    goto start;
#else
while (1) 
{
    sleep(1000);
}
#endif

    exit(EXIT_SUCCESS);
}

