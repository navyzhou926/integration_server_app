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

int flag = 0;
int server_sock_tcp, client_sock_tcp;

void *pthread_get_local_ip(void *arg)
{
    while (1) 
    {
        sleep(300);
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
    time_t timer;
    struct tm *tblock;
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
    timer = time(NULL);
    tblock = localtime(&timer);
    printf("FUNC[%s] LINE[%d]\tStartTime: %s\n",__FUNCTION__, __LINE__, asctime(tblock));
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

        if (ret != RECV_BUFFER_SIZE) 
        {
            printf_debug("FUNC[%s] LINE[%d]\tReceived %d bytes data, didn't receive enough data\t\n",__FUNCTION__, __LINE__,ret);
            break;
        }
        else 
        {
            #if 0
            if (strcmp(recv_buffer, "abcdefghi") == 0) 
            {
                pthread_mutex_lock(&counter_mutex);
                send(client_sock_tcp, "ABCDEFGHI", 9, 0);
                pthread_mutex_unlock(&counter_mutex);
            }
            else if(strcmp(recv_buffer, "I'm alive") == 0)
            {
                pthread_mutex_lock(&counter_mutex);
                send(client_sock_tcp, "AlsoAlive", 9, 0);
                pthread_mutex_unlock(&counter_mutex);
            }
            #else
                pthread_mutex_lock(&counter_mutex);
                send(client_sock_tcp, "ABCDEFGHIJKLMNOPQRSTUVWXYZ\n", 27, 0);
                pthread_mutex_unlock(&counter_mutex);
            #endif
            printf_debug("length: %d   recv_data: ",ret);
            //for (i = 0; i < RECV_BUFFER_SIZE; i++) 
            #if 1
            printf_debug("%s\n",recv_buffer);
            #else
            for (i = 0; i < ret; i++) 
            {
                printf_debug("%c ",recv_buffer[i]);
            }
            printf("\n");
            #endif
        }
    }

tcp_end:
    //pthread_detach(tid_handshake);
    //pthread_join(tid_handshake, &tret);
    pthread_cancel(tid_handshake);
    close(server_sock_tcp);
    goto start;

    exit(EXIT_SUCCESS);
}

