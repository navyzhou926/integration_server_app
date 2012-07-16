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

#include "global.h"
#include "net.h"
#include "entrance_guard.h"
#include "ck2316_alarm.h"
#include "matrix_control.h"

#define DEBUG

#ifdef DEBUG
#define printf_debug(fmt, arg...)  printf(fmt, ##arg)
#else       
#define printf_debug(fmt,arg...) do{ } while (0)
#endif 

#define PATH_HOME "/home"

unsigned char net_recv_verify_code[] = {0xAA,0x55,0xAA,0x55,0xAA,0x55,0xAA,0x55,0xAA,0x55,0xAA,0x55,0xAA,0x55,0xAA,0x55,0xAA,0x55,0xAA,0x55,0xAA,0x55,0xAA,0x55,0xAA,0x55,0xAA,0x55,0xAA,0x55};

int flag = 0;
int server_sock_tcp, client_sock_tcp;

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

int main(int argc, char *argv[])
{
    #ifdef DEBUG
    time_t tm;
    struct tm *t;
    #endif
    int ret = 0;
    //int i = 0;
    //char *receive_ip = NULL;
    //char *receive_mac = NULL;

    //socklen_t client_len;  
    struct sockaddr_in server_tcp,client_tcp;

    pthread_t tid_get_local_ip;
    pthread_t tid_entrance_guard;
    pthread_t tid_ck2316_alarm;
    pthread_t tid_matrix_control;
    pthread_t tid_handshake;

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

    //创建与门禁通信的线程
    if (pthread_create(&tid_entrance_guard, NULL, pthread_entrance_guard, NULL) != 0) 
    {
        printf("FUNC[%s] LINE[%d]\tCan't create entrance guard thread !\n",__FUNCTION__, __LINE__);
        exit(1);
    }

    //创建与报警器通信的线程
    if (pthread_create(&tid_ck2316_alarm, NULL, pthread_ck2316_alarm, NULL) != 0) 
    {
        printf("FUNC[%s] LINE[%d]\tCan't create ck2316 alarm thread !\n",__FUNCTION__, __LINE__);
        exit(1);
    }

    //创建与矩阵通信的线程
    if (pthread_create(&tid_matrix_control, NULL, pthread_matrix_control, NULL) != 0) 
    {
        printf("FUNC[%s] LINE[%d]\tCan't create maxtrix control thread !\n",__FUNCTION__, __LINE__);
        exit(1);
    }

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
    int on = 1;
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
            if (strncmp(net_recv_buffer, net_recv_verify_code, 30) == 0) 
            {
                switch(net_recv_buffer[31])
                {
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
                    default:   //无效指令
                        printf_debug("FUNC[%s] LINE[%d]\tInvalid net command!\n",__FUNCTION__, __LINE__);
                        entrance_guard_data.setup_command_set = ENTRANCE_GUARD_NO_VALID_COMMAND;
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

            printf_debug("length: %d   net_recv_data: 0x%x, 0x%x",ret, net_recv_buffer[30], net_recv_buffer[31]);
            #if 1
            //printf_debug("%s",net_recv_buffer);
            #else
            for (i = 0; i < ret; i++) 
            {
                printf_debug("0x%x ",net_recv_buffer[i]);
            }
            #endif
            printf("\n");
        }
    }

    //pthread_detach(tid_handshake);
    //pthread_join(tid_handshake, &tret);
    pthread_cancel(tid_handshake);
tcp_end:
    close(server_sock_tcp);
    goto start;

    exit(EXIT_SUCCESS);
}

