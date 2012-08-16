#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <sys/select.h>
#include <sys/stat.h>
#include <dirent.h>
#include <netdb.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <pthread.h>

//#define DEBUG

#ifdef DEBUG
#define printf_debug(fmt, arg...)  printf(fmt, ##arg)
#else       
#define printf_debug(fmt,arg...) do{ } while (0)
#endif 

#define PATH_HOME "/home"

#define SERVER_PORT	    8000	
#define SERVER_PORT_UDP	   21203 
#define SERVER_PORT_TCP	    21203	
#define BUFFER_SIZE	    1024	

FILE * received_file(const char *buffer, int len, char *filename);
int get_local_ip(char *out_ip, char *out_mac);

FILE *fd_before = NULL;
FILE *fd_now = NULL;
int flag = 0;

char ip[36] = {0};
char mac[36] = {0};

void *pthread(void *arg)
{
    while (1) 
    {
        sleep(120);
        if (get_local_ip(ip, mac) == 0) 
        {
            printf("local ip: %s\n",ip);
            printf("local mac: %s\n",mac);
        }
    }

    return (void *)1;
}

int main(int argc, char *argv[])
{
    int len = 0;
    char buffer[4096];
    socklen_t client_len;  
    int server_sock;
    struct sockaddr_in server, client;
    //struct in_addr in;

    int server_sock_tcp;
    int client_sock_tcp; 
    struct sockaddr_in server_tcp,client_tcp;

    pthread_t tid;

    char *receive_ip = NULL;
    char *receive_mac = NULL;
    char *p = NULL;

    int optval = 1;
    int on = 1;
    fd_set rset;
    struct timeval tm;
    int res = 0;
    int length = 0;
    time_t timer;
    struct tm *tblock;

get_ip_mac:
    if (get_local_ip(ip, mac) == 0) 
    {
        printf("local ip: %s\n",ip);
        printf("local mac: %s\n",mac);
    }
    else
    {
        printf("Can not get loacl ip\n");
        sleep(3);
        goto get_ip_mac;
    }

    if (pthread_create(&tid, NULL, pthread, NULL) != 0) 
    {
        printf("can't create thread\n");
        exit(1);
    }

start:
    if ((server_sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0) 
    {
        fprintf(stderr, "%s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    else
    {
        printf_debug("UDP create socket ok !\n");
    }

    bzero(&server, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(SERVER_PORT);
    server.sin_addr.s_addr = htons(INADDR_ANY);
    //server.sin_addr.s_addr = inet_addr("255.255.255.255");
    setsockopt(server_sock, SOL_SOCKET, SO_BROADCAST, &optval, sizeof(optval)); 
    //setsockopt(server_sock, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)); 

    if (bind(server_sock, (struct sockaddr *)&server, sizeof(server)) < 0) 
    {
        fprintf(stderr, "%s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    else
    {
        printf_debug("UDP bind addr ok !\n\n");
    }
    printf("Wait to update ...\n");
again:
    client_len = sizeof(client);
    len = recvfrom(server_sock, buffer, BUFFER_SIZE, 0, (struct sockaddr *)&client, &client_len);
    printf_debug("--- %s ---\n", buffer);
    if (len < 0) 
    {
        close(server_sock);
        fprintf(stderr, "%s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    //if ((strcmp(buffer, "ready1") != 0) && (strcmp(buffer, "readyall") != 0)) 
    //goto again;
    if (strcmp(buffer, "readyall") != 0) 
    {
        if (strcmp(buffer, "heartbeat") == 0) 
        {
            printf("heartbeat\n");
            sprintf(buffer, "heartbeat@%s@%s",ip, mac);
            client_len = sizeof(client);
            if (sendto(server_sock, buffer, BUFFER_SIZE, 0,(struct sockaddr *)&client ,client_len) < 0)
            {
                printf("-----------------send heartbeat failed\n");
                close(server_sock);
                goto start;
            }
            goto again;
        }
#if 1
        else
        {
            p = buffer;
            while (*p != '\0')
            {   
                if (*p == '@') 
                {   
                    *p = '\0';
                    receive_ip = buffer; 
                    receive_mac = p + 1;
                    if ((strcmp(ip, receive_ip) == 0) && (strcmp(mac, receive_mac) == 0)) 
                    {
                        //printf_debug("ip: %s\n",ip);
                        //printf_debug("receive_ip: %s\n",receive_ip);
                        //printf_debug("mac: %s\n",mac);
                        //printf_debug("receive_mac: %s\n",receive_mac);
                        break;
                    }
                    goto again;
                }   
                p++;
            } 
        }
#endif
    }

    //TCP
#if 1
    if ((server_sock_tcp = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        fprintf(stderr, "%s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    else
    {
        printf_debug("TCP create socket ok!\n");
    }
    setsockopt(server_sock_tcp, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
    bzero(&server_tcp, sizeof(server_tcp));
    server_tcp.sin_family = AF_INET;
    server_tcp.sin_port = htons(SERVER_PORT_TCP);
    server_tcp.sin_addr.s_addr = htons(INADDR_ANY);
    if(bind(server_sock_tcp, (struct sockaddr *)&server_tcp, sizeof(server_tcp)) < 0)
    {
        fprintf(stderr, "%s\n", strerror(errno));
        printf("tcp bind error !\n");
        exit(EXIT_FAILURE);
    }
    else
    {
        printf_debug("TCP bind addr ok!\n");
    }
    if(listen(server_sock_tcp, 5) < 0)
    {
        fprintf(stderr, "%s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    else
    {
        printf_debug("TCP listen ok!\n");
    }
#endif

#if 0
    //delete files and ready to sync
    DIR *dir;
    struct dirent *ptr;
    int i = 0;
    int iret = 0;

    for (i = 0; i < 4; i++) 
    {
        sprintf(buffer, "%s/%s", PATH_HOME, *(filename + i));
        if ((dir = opendir(buffer)) != NULL) 
        {
            while ((ptr = readdir(dir)) != NULL)
            {
                if (ptr->d_type == DT_REG) 
                {
                    sprintf(buffer, "%s/%s/%s",PATH_HOME, *(filename + i), ptr->d_name);
                    printf_debug("path: %s\n",buffer);
                    if ((iret = remove(buffer)) < 0)
                    {
                        printf("fail to delete %s \n",ptr->d_name);
                    }
                    printf("delete one file\n");
                }
            }
        }
    }
#endif
    //UDP
    //sleep(3);
    printf_debug("UDP Connect client successfully !\n");
    client_len = sizeof(client);
    strcpy(buffer, "ready to sync");
    //if (sendto(server_sock, "ready to sync", strlen("ready to sync"), 0,(struct sockaddr *)&client ,client_len) < 0)
    //if (sendto(server_sock, "ready to sync", BUFFER_SIZE, 0,(struct sockaddr *)&client ,client_len) < 0)
    //if (sendto(server_sock, "ready to sync", 13, 0,(struct sockaddr *)&client ,client_len) < 0)
    if (sendto(server_sock, buffer, BUFFER_SIZE, 0,(struct sockaddr *)&client ,client_len) < 0)
    {
        printf("-----------------ready to sync failed\n");
        close(server_sock_tcp);
        close(server_sock);
        goto start;
    }


#if 0
    if (pthread_create(&tid, NULL, pthread, NULL) != 0)
    3
        printf("can't create thread");
        //close(server_sock);
        //close(server_sock_tcp);
        //system("./TS1500DC_update.bin");
        exit(1);
    }
#endif

#if 0
    int flag_block;
    int connected = 0;
    flag_block = fcntl(server_sock_tcp, F_GETFL, 0);
    flag_block |= O_NONBLOCK;
    fcntl(server_sock_tcp, F_SETFL, flag_block);//设置非阻塞
    client_len = sizeof(client_tcp);
    client_sock_tcp = accept(server_sock_tcp, (struct sockaddr *)&client_tcp, &client_len); 
    //if((client_sock_tcp = accept(server_sock_tcp, (struct sockaddr *)&client_tcp, &client_len)) < 0)
#if 0
    if (connected != 0) 
        if(errno != EINPROGRESS)
        {
            printf("<h2>Update failed !<br><h2>\n");
            goto tcp_end;
        }
        else //esle
#endif
        {
            struct timeval tm;
            tm.tv_sec = 3;
            tm.tv_usec = 0;
            fd_set set,rset;
            FD_ZERO(&set);
            FD_ZERO(&rset);
            FD_SET(server_sock_tcp, &set);
            FD_SET(server_sock_tcp, &rset);
            int res;
            res = select(server_sock_tcp+1, &rset,&set, NULL, &tm);
            if(res < 0)
            {
                printf("Network error in connect, update failed !\n");
                goto tcp_end;
            }
            else if(res == 0)
            {
                printf("Connect time out, update failed !\n");
                //printf_debug("client port: %d\n", ntohs(client_tcp.sin_port));
                goto tcp_end;
            }
            else if (res == 1)
            {
                if(FD_ISSET(server_sock_tcp, &rset))
                {
                    flag_block &= ~O_NONBLOCK;
                    fcntl(server_sock_tcp, F_SETFL, flag_block);//设置阻塞

                    fd_now = NULL;
                    flag = 0;
                    int length = 0;

                    memset(buffer, 0, sizeof(buffer));
                    while ((length = recv(client_sock_tcp, buffer, BUFFER_SIZE, 0)) > 0)
                    {
                        while (length != BUFFER_SIZE) 
                        {
                            length += recv(client_sock_tcp, buffer + length, BUFFER_SIZE-length, 0);   
                        }
                        len = ((buffer[2] << 8)&0xff00) + buffer[3];
                        //printf_debug("%s\n",buffer + 4);
#if 1
                        switch(buffer[0])
                        {
                            case '0':
                                received_file(buffer, len, PATH_HOME);
                                break;
                            case '1':
                                received_file(buffer, len, PATH_HOME);
                                break;
                            case '2':
                                received_file(buffer, len, PATH_HOME);
                                break;
                            case '3':
                                received_file(buffer, len, PATH_HOME);
#if 0
                                if (buffer[1] == '1') 
                                {
                                    //if (fd != NULL) 
                                    //fclose(fd);
                                    strcpy(path, "./upload-folder/OtherFile");
                                    iret = access(path, W_OK);
                                    if(iret != 0)
                                    {
                                        iret = mkdir(path, 0777);
                                        if(iret != 0)
                                        {
                                            printf("can not create dir");
                                            exit(1);
                                        }
                                    }
                                    strcat(path, "/");
                                    strcat(path, (buffer + 4));
                                    fd = fopen(path, "w+");
                                    if(fd == NULL)
                                    {
                                        printf("can not open file");
                                        exit(1);
                                    }
                                    printf("%s",(buffer + 4));
                                }
                                if (buffer[1] == '0') 
                                {
                                    len = fwrite(buffer + 4, 1, len-4, fd);
                                    printf("write len = %d\n",len);
                                }
#endif
                                break;
                            default : break;
                        }
#endif
                    }
                    if (fd_now != NULL) 
                    {
                        fclose(fd_now);
                    }
                    flag = 0;
                    fd_before = NULL;
                    printf("\nUpdate finished\n\n");
                }
                else
                {
                    fprintf(stderr, "%s\n", strerror(errno));
                    printf("Other error, update failed !\n");
                    goto tcp_end;
                }
            }
        }
#else
    //TCP
    #if 1
    tm.tv_sec = 90; 
    tm.tv_usec = 0;
    client_len = sizeof(client_tcp);
    while(1)
    {
        FD_ZERO(&rset);
        FD_SET(server_sock_tcp, &rset);
        res = select(server_sock_tcp+1, &rset, NULL, NULL, &tm);
        if (res < 0) 
        {
            goto tcp_end;
        }
        else if (res == 0)// time out 
        {
            printf("Accept time out, update failed !\n");
            goto tcp_end;
        }
        else
        {
            if (FD_ISSET(server_sock_tcp, &rset)) 
            {
                if((client_sock_tcp = accept(server_sock_tcp, (struct sockaddr *)&client_tcp, &client_len)) < 0)
                {
                    continue;
                }
                else
                {
                    printf_debug("accept client ok!\n");
                    break;
                }
            }
        }
    }
    #else
    client_len = sizeof(client_tcp);
    if((client_sock_tcp = accept(server_sock_tcp, (struct sockaddr *)&client_tcp, &client_len)) < 0)
    {
        fprintf(stderr, "%s\n", strerror(errno));
        goto tcp_end;
        //exit(EXIT_FAILURE);
    }
    else
    {
        printf_debug("accept client ok!\n");
        //in.s_addr = client_tcp.sin_addr.s_addr;
        //printf_debug("client ip  : %s\n", inet_ntoa(in));
        //printf_debug("client port: %d\n", ntohs(client_tcp.sin_port));
    }
    #endif
    fd_now = NULL;
    flag = 0;

    memset(buffer, 0, sizeof(buffer));
    while ((length = recv(client_sock_tcp, buffer, BUFFER_SIZE, 0)) > 0)
    {
        while (length != BUFFER_SIZE) 
        {
            length += recv(client_sock_tcp, buffer + length, BUFFER_SIZE-length, 0);   
        }
        len = ((buffer[2] << 8)&0xff00) + buffer[3];
        //printf_debug("%s\n",buffer + 4);
        switch(buffer[0])
        {
            case '0':
                received_file(buffer, len, PATH_HOME);
                break;
            case '1':
                received_file(buffer, len, PATH_HOME);
                break;
            case '2':
                received_file(buffer, len, PATH_HOME);
                break;
            case '3':
                received_file(buffer, len, PATH_HOME);
#if 0
                if (buffer[1] == '1') 
                {
                    //if (fd != NULL) 
                    //fclose(fd);
                    strcpy(path, "./upload-folder/OtherFile");
                    iret = access(path, W_OK);
                    if(iret != 0)
                    {
                        iret = mkdir(path, 0777);
                        if(iret != 0)
                        {
                            printf("can not create dir");
                            exit(1);
                        }
                    }
                    strcat(path, "/");
                    strcat(path, (buffer + 4));
                    fd = fopen(path, "w+");
                    if(fd == NULL)
                    {
                        printf("can not open file");
                        exit(1);
                    }
                    printf("%s",(buffer + 4));
                }
                if (buffer[1] == '0') 
                {
                    len = fwrite(buffer + 4, 1, len-4, fd);
                    printf("write len = %d\n",len);
                }
#endif
                break;
            default : break;
        }
    }
    if (fd_now != NULL) 
    {
        fclose(fd_now);
    }
    flag = 0;
    fd_before = NULL;
    timer = time(NULL);
    tblock = localtime(&timer);
    printf("\n%s",asctime(tblock));
    printf("Update finished\n\n");
#endif

tcp_end:
    close(server_sock_tcp);
    close(server_sock);
    goto start;

    exit(EXIT_SUCCESS);
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

FILE * received_file(const char *buffer, int len, char *filename)
{
    int iret = 0;
    char path[4096] = {'\0'};

    if (buffer[1] == '1')  //filename 
    {
        //if (fd != NULL) 
        //fclose(fd);
        strcpy(path, filename);
        iret = access(path, W_OK);
        if(iret != 0)
        {
            iret = mkdir(path, 0777);
            if(iret != 0)
            {
                printf("can not create dir");
                exit(1);
            }
        }
        strcat(path, "/");
        strcat(path, (buffer + 4));
        #if 0
        if (strcmp(buffer + 4, "TS1500DC_update_ARM.bin") == 0) 
        {
            if (remove(buffer + 4) < 0)
            {
                printf("fail to delete %s \n",buffer + 4);
            }
        }
        #endif
        fd_now = fopen(path, "w+");
        if(fd_now == NULL)
        {
            printf("%s\n",path);
            printf("can not open file\n");
            printf("Maybe have not enough space\n");
            exit(1);
        }
        if (flag == 0) 
        {
            flag = 1;
            fd_before = fd_now;
        }
        if (fd_before != fd_now) 
        {
            fclose(fd_before);
            fd_before = fd_now;
        }
        printf("\nReceiving: %s ...\n",(buffer + 4));
    }
    if (buffer[1] == '0')  //file data
    {
        printf_debug("Received byte = %d\n",len);
        len = fwrite(buffer + 4, 1, len-4, fd_now);
        //printf_debug("write byte = %d\n",len);
        printf_debug("write byte = %d\n",len + 4);
    }

    return fd_now;
}

