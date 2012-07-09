#include <string.h>
#include <stdio.h>
#include <pthread.h>

#include "net.h"
#include "global.h"

#define DEBUG

#ifdef DEBUG
#define printf_debug(fmt, arg...)  printf(fmt, ##arg)
#else       
#define printf_debug(fmt,arg...) do{ } while (0)
#endif 

char ip[36] = {0};
char mac[36] = {0};

char recv_buffer[RECV_BUFFER_SIZE + 1] = {0};

int if_disconnect_client = NO;

pthread_mutex_t counter_mutex = PTHREAD_MUTEX_INITIALIZER;

void *pthread_handshake(void *arg)
{
    int client_sock_tcp = *(int *)arg;

    while (1) 
    {
        sleep(HANDSHAKE_DELAY_TIME);
        pthread_mutex_lock(&counter_mutex);
        send(client_sock_tcp, "*********handshake@@@@@@@@\n", 27, 0);
        pthread_mutex_unlock(&counter_mutex);
        //printf("handshake\n");
    }

    //pthread_exit((void *)1);
    return (void *)1;
}

int get_local_ip(char *out_ip, char *out_mac)
{
    int i = 0;
    int sockfd;
    struct ifconf ifconf;
    char buf[512];
    struct ifreq *ifreq;
    char *ip;

    //struct ifreq ifr_mac;

    ifconf.ifc_len = 512;
    ifconf.ifc_buf = buf;


    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) 
        return -1;
    if (ioctl(sockfd, SIOCGIFCONF, &ifconf) < 0)
        return -1;

    ifreq = (struct ifreq *)buf;

    for (i = (ifconf.ifc_len/sizeof(struct ifreq)); i > 0; i--)  //get ip 
    {
        ip = inet_ntoa(((struct sockaddr_in *)&(ifreq->ifr_addr))->sin_addr);
#if 1
        if (strcmp(ip, "127.0.0.1") == 0 || strcmp(ip, "127.0.1.1") == 0) 
        {
            ifreq++;
            continue;
        }
#endif
        strcpy(out_ip, ip);
        break;
    }

    for (i = 0; i < 10; i++)  //get mac 
    {   
        memset(ifreq, 0, sizeof(ifreq));
        sprintf(ifreq->ifr_name,"eth%c",0x30+i);
        //strncpy(ifreq->ifr_name, "eth0", sizeof(ifreq->ifr_name)-1);
        if (ioctl(sockfd, SIOCGIFHWADDR, ifreq) < 0)
            continue;
        else
            break;
    }   
    if (i >= 10) 
    {
        close(sockfd);
        return -1;
    }

    sprintf(out_mac, "%02x:%02x:%02x:%02x:%02x:%02x",
            (unsigned char)ifreq->ifr_hwaddr.sa_data[0],
            (unsigned char)ifreq->ifr_hwaddr.sa_data[1],
            (unsigned char)ifreq->ifr_hwaddr.sa_data[2],
            (unsigned char)ifreq->ifr_hwaddr.sa_data[3],
            (unsigned char)ifreq->ifr_hwaddr.sa_data[4],
            (unsigned char)ifreq->ifr_hwaddr.sa_data[5]);

    close(sockfd);

    return 0;
}


int waiting_for_client_connect(int *server_sock_tcp, struct sockaddr_in *server_tcp, int *client_sock_tcp, struct sockaddr_in *client_tcp) 
{
    fd_set rset;
    socklen_t client_len = sizeof(*client_tcp);
    struct timeval tm;
#ifdef DEBUG
    struct in_addr in;
#endif

    tm.tv_sec = SERVER_ACCEPT_TIMEOUT_SECOND; 
    tm.tv_usec = SERVER_ACCEPT_TIMEOUT_MICROSECOND;

    FD_ZERO(&rset);
    FD_SET(*server_sock_tcp, &rset);
    if (select(*server_sock_tcp+1, &rset, NULL, NULL, &tm) <= 0)
    {
        printf("FUNC[%s] LINE[%d]\tTimeout, these is no client connect!\n",__FUNCTION__, __LINE__);
        return NO;
    }
    else
    {
        if (FD_ISSET(*server_sock_tcp, &rset)) 
        {
            if((*client_sock_tcp = accept(*server_sock_tcp, (struct sockaddr *)client_tcp, &client_len)) < 0)
            {
                printf("FUNC[%s] LINE[%d]\tClient connect error!\n",__FUNCTION__, __LINE__);
                return NO;
            }
            else
            {
                printf_debug("FUNC[%s] LINE[%d]\tAccept client ok !\n",__FUNCTION__, __LINE__);
            #ifdef DEBUG
                in.s_addr = client_tcp->sin_addr.s_addr;
                printf("FUNC[%s] LINE[%d]\tclient ip  : %s\n",__FUNCTION__, __LINE__, inet_ntoa(in));
                printf("FUNC[%s] LINE[%d]\tclient port: %d\n",__FUNCTION__, __LINE__, ntohs(client_tcp->sin_port));
            #endif
                return YES;
            }
        }
    }

    return NO;
}

int tcp_server_recv(int *server_sock_tcp, int *client_sock_tcp)
{
    int length = 0, sum_length = 0;
    fd_set rset;
    struct timeval tm;
    int ret = 0;

    while (sum_length < RECV_BUFFER_SIZE)
    {
        tm.tv_sec = SERVER_RECV_TIMEOUT_SECOND; 
        tm.tv_usec = SERVER_RECV_TIMEOUT_MICROSECOND;
        FD_ZERO(&rset);
        FD_SET(*client_sock_tcp, &rset);

        if ((ret = select((*client_sock_tcp)+1, &rset, NULL, NULL, &tm)) < 0)
        {
            printf("FUNC[%s] LINE[%d]\tRecv client data error !\n",__FUNCTION__, __LINE__);
            return NO;
        }
        else if(ret == 0)
        {
            printf_debug("FUNC[%s] LINE[%d]\tRecv client data time out!\n",__FUNCTION__, __LINE__);
            return sum_length;
        }
        else if(ret > 0)
        {
            if (FD_ISSET(*client_sock_tcp, &rset)) 
            {
                //if ((length = recv(*client_sock_tcp, recv_buffer, RECV_BUFFER_SIZE, 0)) > 0)
                if ((length = recv(*client_sock_tcp, recv_buffer + sum_length, RECV_BUFFER_SIZE-sum_length, 0)) > 0)
                {
                    sum_length += length;
                #if 0
                    while (length != RECV_BUFFER_SIZE) 
                    {
                        length += recv(*client_sock_tcp, recv_buffer + length, RECV_BUFFER_SIZE-length, 0);   
                    }
                #endif
                }
                else
                {
                    printf("FUNC[%s] LINE[%d]\tClient is disconnected !\n",__FUNCTION__, __LINE__);
                    return NO;
                }
            }
        }
    }

    return sum_length;
}

