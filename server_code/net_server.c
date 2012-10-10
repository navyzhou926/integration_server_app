#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/select.h>
#include <time.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <net/if_arp.h>
#include <poll.h>
#include <malloc.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "ck2316_alarm.h"
#include "cradle_head_control.h"
#include "matrix_control.h"
#include "net_server.h"
#include "entrance_guard.h"
#include "global.h"
#include "uart.h"
#include "alarm_input_output.h"
#include "md5.h"

//#define DEBUG_DATA
//#define DEBUG
#define DEBUG_ERROR

#ifdef DEBUG
#define printf_debug(fmt, arg...)  printf(fmt, ##arg)
#else       
#define printf_debug(fmt,arg...) do{ } while (0)
#endif 

#ifdef DEBUG_ERROR
#define err_print(fmt,arg...)  printf(fmt, ##arg);
#else       
#define err_print(fmt,arg...) do{ } while (0)
#endif 
//-----------------ȫ�ֱ�����----------------------------

int g_listen_exit_flag;
struct sockaddr_in sockaddr_listen; //connect client socket
int channel_num_passive = 0,channe_num_active = 0;
pthread_mutex_t mutex_channel,mutex_link,user_outline;	//����ͳ��ͨ�����û��������������
char *file_path = "./net_para_file";                          //��������������ļ�
user_info *head ,*pf;//�����ͷ��β�ڵ�ָ��


extern unsigned char bypass_defence_area_code[][3];
SX_DEVICE_WORKING_STATUS dev_para = {DEV_ID_VERSION,{0},"1234567",DEV_SOFT_VERSION,DEV_HARD_VERSION,DEV_COMM_VERSION,DEV_COM_NUM,{0}};

int raudrate[] = {600,1200,2400,4800,9600,14400,19200,38400,57600,115200};

//----------------����ʵ����-------------------------------------

/*********************************************************************
  fun name    :��������ĳ�ʼ��
func        :    
para:   
ret         : 
date        : 2012-08-28
auto        : 
 *********************************************************************/
int  net_para_init(char *file_path,int len)
{
    FILE * fb;
    int gatewayip;
    char read_buf[200] = {0};
    int flag = 0;
    char ipbuf[4][16] = {{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}};
    char gateway_buf[16] = {0};
    char port[2][16] = {{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}};
    char gateway[50] =  "route add default gw  ";
    if((fb = fopen(file_path,"rt+")) == NULL)
    {
        fb = fopen(file_path,"wt+");
        printf_debug("open file fail\n");
    }
    //��ʼ��ΪĬ��ֵ
    memset(&para_net,0,sizeof(INTER_NETCFG));
    inet_pton(AF_INET,"192.168.11.250",&para_net.dwDeviceIP);//���޸�
    para_net.dwDeviceIP = ntohl(para_net.dwDeviceIP);            //ע��˴��ֽ����ת��
    //printf_debug("deviceip:%d\n",para_net.dwDeviceIP);

    inet_pton(AF_INET,"255.255.255.0",&para_net.dwDeviceIPMask);
    para_net.dwDeviceIPMask = ntohl(para_net.dwDeviceIPMask);

    para_net.wDevicePort = SERVER_PORT_TCP;

    inet_pton(AF_INET, "192.168.11.1",&para_net.dwGatewayIP);
    para_net.dwGatewayIP = ntohl(para_net.dwGatewayIP);

    inet_pton(AF_INET,"192.168.11.53",&para_net.dwManageHostIP);//���޸�
    para_net.dwManageHostIP = ntohl(para_net.dwManageHostIP);

    para_net.wManageHostPort = SERVER_PORT_TCP;

    memcpy(para_net.active_mode_name,"admin" ,16);

    memcpy(para_net.active_mode_pwd,"admin", 16);

    //��ȡ������MAC��ַ
    get_local_lan_mac(para_net.byMACAddr);
    //intf("local mac: %x:%x:%x:%x:%x:%x\n",para_net.byMACAddr[0],para_net.byMACAddr[1],para_net.byMACAddr[2],para_net.byMACAddr[3],para_net.byMACAddr[4],para_net.byMACAddr[5]);

    fread(read_buf,sizeof(read_buf),1,fb);
    sscanf(read_buf,"%d,%s",&flag,read_buf);
    if(1 == flag)
    {	
        printf("initialization the last net_setup.......\n");
        sscanf(read_buf,"%[^,],%[^,],%[^,],%[^,],%[^,],%[^,],%[^,],%s",ipbuf[0],ipbuf[1],ipbuf[2],ipbuf[3],port[0],port[1],para_net.active_mode_name,para_net.active_mode_pwd);

        inet_pton(AF_INET,ipbuf[0],&para_net.dwDeviceIP);

        inet_pton(AF_INET,ipbuf[1],&para_net.dwDeviceIPMask);

        inet_pton(AF_INET,ipbuf[2],&para_net.dwGatewayIP);

        inet_pton(AF_INET,ipbuf[3],&para_net.dwManageHostIP);

        para_net.dwDeviceIP = ntohl(para_net.dwDeviceIP);
        para_net.dwDeviceIPMask = ntohl(para_net.dwDeviceIPMask);
        para_net.dwGatewayIP = ntohl(para_net.dwGatewayIP);
        para_net.dwManageHostIP = ntohl(para_net.dwManageHostIP);

        para_net.wDevicePort = atoi(port[0]); 
        para_net.wManageHostPort = atoi(port[1]);

#ifdef DEBUG_DATA
	printf("recv_buf----:%s\n",read_buf);
        printf("ip:%x\n",para_net.dwDeviceIP);
        printf("ip:%x\n",para_net.dwDeviceIPMask);
        printf("ip:%x\n",para_net.dwGatewayIP);
        printf("ip:%x\n",para_net.dwManageHostIP);
	printf("%s----%s\n",para_net.active_mode_name,para_net.active_mode_pwd);
#endif
        
    }
    else
    {
        //��һ������������ļ���ʧ���ȡ���ݳ���ʱ��ʹ��Ĭ��ֵ********************************
        fclose(fb);
    }
    gatewayip = htonl(para_net.dwGatewayIP);
    inet_ntop(AF_INET,&gatewayip,gateway_buf,INET_ADDRSTRLEN); 
    sprintf(gateway,"%s%s",gateway,gateway_buf);

    //ʵ��ip �����ء� �������Ч**********************************************************

    if(set_lan_ip(para_net.dwDeviceIP) != 0)
    {
        err_print("set the ip fail\n");
    }
    if(set_local_lan_gateway(gateway) != 0)
    {
        printf_debug("she zhi wang guan shi bai\n");
    }
    if(set_lan_netmask(para_net.dwDeviceIPMask) != 0)
    {
        err_print("set the netmask fail\n");
    }
     printf("initialization the last net_setup  ok!!!!!!\n");
    return 1;
}

/*********************************************************************
  fun name    :����ģʽ��ͨ��
func        :    ����ģʽ���豸�����û���֮�����Ϣ����
para:   
ret         : 
date        : 2012-08-13
auto        : 
 *********************************************************************/
void *pthread_server_active(void *arg)
{
    int ret = 0;
    // p_active->mode = NET_ACTIVE_MODE;

    //struct timeval tstart = {0,0};   
    //struct timeval tend = {0,0};   
    //int diff;

    //err_print("  Net Connect PID is :%d...\n", getpid());

CYCLE_CONNECT:	  
    while (1)
    {	
        if (0 == para_net.dwManageHostIP)
        {
            sleep(10);
            continue;
        }
        server_connect_fd = api_connect_remote_client(para_net.dwManageHostIP, para_net.wManageHostPort, MAX_TIME_INTERNAL_FOR_CTL);
        if (-1 == server_connect_fd)
        {
            printf("no active sockfd\n");
            goto STEP0;    
        }
        //����tcp��¼�����        Ӧ�����ж�		 
        printf_debug("active sockfd:%d\n",server_connect_fd);
        ret = login_apply();

        if(ret  > 0)
        {
            printf_debug("login ok\n");
            channe_num_active++;
            link_insert(server_connect_fd);
            goto INTERACTIVE;
        }
        else 
        {
            printf_debug("fail\n");
            printf_debug("active_sockfd is :%d\n",server_connect_fd);
            link_delete(server_connect_fd);
            goto STEP0;
        }  	     
    }
INTERACTIVE:

    while(1)
    {	
        if(1 != main_service_ctl())//���ʧ�����˳�  ֱ�ӽ���STEP0
        {
            printf_debug("close the active_sockfd\n");
            goto STEP0;
        }
    }
STEP0:
    errno = 0;

    if (server_connect_fd==-1)
    {
        sleep(10);
        goto CYCLE_CONNECT;
    }
    while ( (-1 == close(server_connect_fd)) && (errno == EINTR) );

    sleep(10);
    goto CYCLE_CONNECT; 
}


/*********************************************************************
  fun name    :����ģʽ��ͨ��
func        :    ����ģʽ���豸�����û���֮�����Ϣ����
para:   
ret         : 
date        : 2012-08-13
auto        : 
 *********************************************************************/
void *pthread_server_passive(void *arg)
{
    int client_fd;
    int sin_size = sizeof(struct sockaddr_in); //this must be initilized otherwize the first accetp's ip will be zero

    pthread_t pth_deactive;
    LISTEN_ACQUIRED_PARA *client_tmp;
    struct timeval timeout={120, 0};     //120s
    fd_set fd_read;

    server_listen_fd = enable_net_listen();
    if (server_listen_fd<0)
    {
        goto DEAL_OVER;
    }

    //��ʼ��������
    pthread_mutex_init(&mutex_channel, NULL);
    pthread_mutex_init(&mutex_link, NULL);
    pthread_mutex_init(&user_outline, NULL);

    printf_debug("server_listen_fd = %d listen on %d port!\n",server_listen_fd,SERVER_PORT_TCP);
    while(!g_listen_exit_flag)
    {
        char ipbuf[16] = "";
        FD_ZERO(&fd_read);
        FD_SET(server_listen_fd, &fd_read);

        timeout.tv_sec = 120;//120s
        timeout.tv_usec = 0;

        switch (select(server_listen_fd + 1, &fd_read, 0, 0, &timeout))
        {
            case 0://timeout
                {
                    continue;
                }
            case -1://err
                {
                    break; //do-nothing
                }

            default:
                {
                    err_print("Something Come\n");
                    if (FD_ISSET(server_listen_fd, &fd_read))//has client connect
                    {
                        printf_debug("have clients connect........\n");
                        if (-1 == (client_fd = accept(server_listen_fd, (struct sockaddr *) &sockaddr_listen, (int*)&sin_size)))
                        {
                            continue; 
                        }

                        if ( !(client_tmp = (LISTEN_ACQUIRED_PARA *)calloc(1, sizeof(LISTEN_ACQUIRED_PARA))) )
                        {
                            // err_print("Net-Module--calloc %s\n", strerror(errno));
                            printf_debug("calloc fail\n");
                            continue;
                        }        

                        client_tmp->connect_fd = client_fd;
                        client_tmp->client_ip = ntohl(sockaddr_listen.sin_addr.s_addr);//�˴�Ӧ�����ֽ���ת��
                        inet_ntop(AF_INET,&sockaddr_listen.sin_addr,ipbuf,16);
                        printf_debug("client come from %s\n",ipbuf);

                        // memcpy(&client_tmp->client_ip, &sockaddr_listen, sizeof(struct sockaddr));
                        //   err_print("Coming-ClientIP:%x...\n", client_tmp->client_ip.sin_addr.s_addr);
                        printf_debug("clients connect sockfd = %d\n",client_tmp->connect_fd);
                        //ÿ��һ�����Ӵ���һ���߳�ȥ����
                        if (0 != pthread_create(&pth_deactive, NULL, pthread_visit_handel,  (void *)(client_tmp)))
                        {
                            //err_print("Net-Module--pthread_create-%s\n", strerror(errno));

                            errno = 0;
                            while ( (-1 == close((client_fd))) && (errno == EINTR) );
                            if (client_tmp) 
                            {
                                printf_debug("close the connect:%d\n",client_tmp->connect_fd);
                                free(client_tmp);
                                client_tmp = NULL;				   
                            }
                        }                  
                        break;
                    }
                }
        }
    }
DEAL_OVER:    
    errno = 0;
    err_print("Closeing\n");

    while ( (-1 == close((server_listen_fd))) && (errno == EINTR) );
    g_listen_exit_flag=1;
    err_print("Closeed socket\n");
    pthread_exit(NULL);      
    return NULL;
}

/*********************************************************************
  fun name    : ��ʼ���׽���
func        : 
para:   addr:�󶨵�IP��ַ����=-1ʱ����ʾANY
port:�󶨵Ķ˿�

ret         : 
date        : 2010-01-05
auto        : 
 *********************************************************************/
int enable_net_listen()
{    
    int fd = -1;

    while(!g_listen_exit_flag)
    {
        if (-1 == (fd = init_socket(INADDR_ANY,SERVER_PORT_TCP, 1,N_NET_LISTNING_PORT, TCP_SOCK)))
        {
            err_print("Err:init_socket,Try Again-errno:%d--%s...\n", errno, strerror(errno));
            sleep(3);
        }
        else
        {
            break;
        }
    }
    return fd;
}

/*********************************************************************
  fun name    : ��ʼ���׽���
func        : 
para:   addr:�󶨵�IP��ַ����=-1ʱ����ʾANY
port:�󶨵Ķ˿�

ret         : 
date        : 2010-01-05
auto        : 
 *********************************************************************/
int init_socket(int addr, int port, int listen_flag, int listen_num, int sock_type)
{
    int sock_id;
    int sin_size;
    sockaddr_listen.sin_family = AF_INET;
    sockaddr_listen.sin_port = htons(port);
    sockaddr_listen.sin_addr.s_addr = htonl(addr);
    memset (&(sockaddr_listen.sin_zero), 0, 8);
    sin_size = sizeof(sockaddr_listen);    

    if (sock_type==TCP_SOCK)
    {
        if (-1 == (sock_id = socket(AF_INET, SOCK_STREAM, 0)))
        {
            return -1;
        }   
    }
    else if (sock_type==UDP_SOCK)
    {
        if (-1 == (sock_id = socket(AF_INET, SOCK_DGRAM, 0)))
        {
            return -1;
        }  
    }
    else
    {
        return -1;
    }

    if (1)     
    {    		
        struct linger sopt = { 1, 0 }; 
        int reused = 1; //indicate that this opt is open    
        if (-1 == setsockopt(sock_id, SOL_SOCKET, SO_REUSEADDR, &reused, sizeof(int)))
        {
            return -1;
        }
        setsockopt(sock_id, SOL_SOCKET, SO_LINGER, (void *)&sopt, sizeof(sopt));
    }


    if (-1 == (bind(sock_id, (struct sockaddr *) &sockaddr_listen, sizeof (sockaddr_listen))))
    {
        while ( (-1 == close((sock_id))) && (errno == EINTR) );

        return -1;
    }

    if (1==listen_flag)
    {
        if (-1 == listen (sock_id, listen_num))
        {
            while ( (-1 == close((sock_id))) && (errno == EINTR) );
            return -1;
        }
    }

    return sock_id;
}

/********************************************************************
  fun name    : ���������߳�
func        : 
para:   addr
port:

ret         : 
date        : 
auto        : 
 *********************************************************************/

int main_service_ctl(void)
{
    INTER_SXC_HEAD recv_buf;
    printf_debug("wait to recv\n");
    return  recv_data_intime(server_connect_fd,60*1000,  (char *)(&recv_buf), sizeof(INTER_SXC_HEAD),0);
}

/*********************************************************************
  fun name    : ���������߳�
func        : 
para:   addr
port:

ret         : 
date        : 
auto        : 
 *********************************************************************/
void *pthread_visit_handel(void *arg)
{
    int recv_flag = 0;

    LISTEN_ACQUIRED_PARA *p_listen = (LISTEN_ACQUIRED_PARA *)arg;
    while(!g_listen_exit_flag)
    { 

        if (arg == NULL)
        {
            printf_debug("connect_fd = 0\n");
            //err_print("NULL-Ptr\n");
            pthread_exit(NULL);
        }

#if 0
        //set attr
        memset(&sock_att, 0, sizeof(sock_att));
        sock_att.noblocked = 1;
        if (set_sock_attr(p_listen->connect_fd, &sock_att))
        {
            goto DEAL_OVER;
        }
#endif

        //	printf_debug("start to recv msg\n");

        //60���ڽ�������
        recv_flag = recv_data_intime(p_listen->connect_fd, 60*1000,  (char *)(&p_listen->Inter_sxc_head), sizeof(INTER_SXC_HEAD),p_listen->client_ip);
        //	printf_debug("g_listen_exit_flag =%d-------recv_flag = %d\n",g_listen_exit_flag,recv_flag);
        if (recv_flag == -1)
        {
            goto DEAL_OVER;
        }
    }
DEAL_OVER:
    //err_print("Begin to Free Service Reource\n");
    //�ͷŴ���Ϣ�ײ�--Listen��������Ϣ
    //free-resource-------------
    printf_debug("client_fd :---->%d closed\n",p_listen->connect_fd);
    link_delete(p_listen->connect_fd);//��Ӧ��ͨ���ر�ʱά�������û�����
    channel_num_passive--;
#if 1
    link_print(head);
    printf_debug("channel NO.:%d\n",channel_num_passive);
#endif

    while ( (-1 == close((p_listen->connect_fd))) && (errno == EINTR) );
    //ע��˴��ı�Ҫ��
    free(p_listen);

    pthread_exit(NULL);

    return NULL;
}


/******************************************************
Fun:  �ڹ涨��ʱ���ڽ��ճ���Ϊsrc_len�����ݵ�recv_buf��ȥ
para:   client_fd ���յ�socket
timeout:��ʱʱ�䣬��λΪ����[ms]
recv_buf,���ջ����� 
src_len�����յĳ���
 *********************************************************/
int recv_data_intime(int client_fd, int timeout, char *recv_buf, int src_len,int client_ip)
{

    int res;
    int poll_timeout;
    struct pollfd poll_set[1];



    if (recv_buf == NULL || src_len <= 0 )
    {
        //  err_print("Recv data intime para invalid...\n");
        printf_debug("recv_buf = NULL || src_len = 0\n");
        return -1;
    } 

    poll_set[0].fd = client_fd;
    poll_set[0].events = POLLIN;// ���Խ���
    poll_timeout = timeout; //

    errno = 0;


    //probe
POLL_AGAIN:
    res = poll(poll_set, 1, poll_timeout);
    if (res == -1 )
    {
        if (errno == EINTR) goto POLL_AGAIN;
        else return -1;
    }

    else if (res == 0)//timeout
    {
        return -1;
    }

    //recv
    return recv_data(client_fd,  recv_buf,  src_len,client_ip);
}

/******************************************************
Fun: ѭ����������,����
para:   
timeout:
recv_buf, 
src_len��
 *********************************************************/
int recv_data(int client_fd, char *dst, int num,int client_ip)
{
    int recv_len, recv_bytes;
    char *recv_buf,*host_recv_buf;
    int try_times = 0;

    // INTER_SXC_HEAD *user_info = (INTER_SXC_HEAD *)calloc(1,sizeof(LISTEN_ACQUIRED_PARA));//�˴�������ʱ��Ӧ����Ϊ��ȥ�ͷ�

    if (NULL == dst || num < 0)
    {
        //err_print("invalide argument, client:%d, num:%d, dst:%x", client_fd, num, (U32)dst);
        return -1;
    }

    recv_len = num;
    recv_buf = dst;
    host_recv_buf = dst;
    while(recv_len>0 && (NULL != recv_buf))//��Ϣ
    {
        INTER_SXC_HEAD *str;
        recv_bytes = recv(client_fd,  recv_buf,  recv_len,  MSG_NOSIGNAL);                
        if (0 ==  recv_bytes)//�Զ˹ر�
        {   
            printf_debug("client_fd :%d------------recv_data 0.\n", client_fd);
            return -1;
        }

        else if (-1 ==  recv_bytes)
        {
            if ((errno == EAGAIN) || (errno == EINTR))
            {
                recv_bytes = 0;
                try_times++;
                if (try_times>SOCKET_BUSY_TRY_TIMES)
                {
                    err_print("Clent:%d Recv Try More than %d times-Poll Passed but Recv busy\n", client_fd, SOCKET_BUSY_TRY_TIMES);     
                    return -1;
                }

                errno = 0;                
                usleep(40000); //40ms,����,Ҳ���ڽ���,ֻ�ǽ�cpuʱ���ó�
                continue;
            }
            else
            {
                err_print("recv faital err:%d:%s\n", errno, strerror(errno));      
                errno = 0;                
                return -1;
            }
        }
        //�������յ����ݺ�recv_bytes>0

        str = databyte_net_to_host(recv_buf);
        // printf("version=%d, length =%d, checksum =%d,cmdword =%x, status = %d,userid =%d\n",str->version,str->length,str->checkSum,str->cmdWord,str->status,str->usrID);
        recv_buf += recv_bytes;
        recv_len -= recv_bytes;

        //���ݿͻ���������л�Ӧ
        cmd_data_deal(str,client_fd,str->length - recv_bytes,client_ip);
    }
    return 1;
}
/******************************************************
Fun: �ͻ���ͷ�����ֽ���ת��Ϊ������
para:   
timeout:
recv_buf, 
src_len��
 *********************************************************/
INTER_SXC_HEAD *databyte_net_to_host(char *recv_buffer)
{
    INTER_SXC_HEAD *str = (INTER_SXC_HEAD *)recv_buffer;
    str->version = ntohl(str->version);
    str->length= ntohl(str->length);
    str->checkSum = ntohl(str->checkSum);
    str->cmdWord = ntohl(str->cmdWord);
    str->status = ntohl(str->status);	
    str->usrID = ntohl(str->usrID);

    return str;
}

/******************************************************
Fun: �����û���������д���
para:   
timeout:
recv_buf, 
src_len��
 *********************************************************/
int   cmd_data_deal(INTER_SXC_HEAD *data_head,int client_fd,int para_len,int client_ip)
{
    char *recv_para;
    int para_length = 0;
    recv_para = (char *)calloc(1,para_len);

    if(para_len > 0)
    {	
        para_length = recv(client_fd,  recv_para, para_len ,  MSG_NOSIGNAL);
    }
#ifdef DEBUG_DATA
    if(data_head->cmdWord != SXC_KEEPLIVE_B)
        printf("The data length is***** %d****cmd is %02x\n",para_length,data_head->cmdWord);
#endif

    switch(data_head->cmdWord)
    {
        //---------------------------------------------------------------------------------�����������
        case SXC_SET_NETCFG:
            {
                if( !net_para_set(recv_para,data_head,client_fd,client_ip))
                {
                    printf_debug("send net msg fail\n");
                    return -1;
                }
                break;
            }
            //---------------------------------------------------------------------------------��������Ļ�ȡ
        case SXC_GET_NETCFG:
            {
                if(!net_para_send(recv_para,data_head,client_fd))
                {
                    printf_debug("get net_para fail\n");
                    return -1;
                }
                printf_debug("get the net_para successful\n");
                break;
            }

            //---------------------------------------------------------------------------------�ͻ��˵�½ȷ�����¼�˳�
        case SXC_LOG_IN:                                     
            {
                return log_in_reply(recv_para,data_head,client_fd);
                break;
            }
            //--------------------------------------------------------------------------------- ˫�����ֻظ�
        case SXC_KEEPLIVE_B:                             
            {
                return (send_act_cmd(QULIFIED,client_fd,SXC_KEEPLIVE_B,client_fd));
                break;
            }
            //----------------------------------------------------------------------------------�豸�˵�¼�ظ�����
        case SXD_LOG_IN:
            {
                if( data_head->status != QULIFIED)
                {
                    printf_debug("active load fail\n");
                    return -1;
                }
                break;
            }
            //----------------------------------------------------------------------------------��̨����
        case SXC_PTZ:
            {
                return cradle_ctrl_reply(recv_para,data_head,client_fd);
                break;
            }
            //----------------------------------------------------------------------------------����ͨ��ȡ��
        case SXC_CANCEL_MATRIX_SINGLE_INOUT:
            {
                return MATRIX_ctrl_cancel_singal(recv_para,data_head,client_fd);
                break;
            }
            //----------------------------------------------------------------------------------����ȫͨ��ȡ��
        case SXC_CANCEL_MATRIX_ALL_INOUT:
            {
                return MATRIX_ctrl_cancel_all(recv_para,data_head,client_fd);
                break;
            }
            //-----------------------------------------------------------------------------------������������л�
        case SXC_EXCHAGE_MATRIX_INOUT:
            {
                return MATRIX_ctrl_change(recv_para,data_head,client_fd);
                break;
            }	     //-----------------------------------------------------------------------------------�Ž����ؿ���
        case SXC_DOOR_CTL_CMD:
            {

                entrance_open_close_ctrl(recv_para,data_head,client_fd);
                break;
            }
            //-----------------------------------------------------------------------------------��ȡ�Ž���������
        case SXC_GET_DOORCFG:
            {
                if(!entrance_para_get(recv_para,data_head,client_fd))
                {
                    printf_debug("send entrance para msg fail\n");
                    return -1;
                }
                break;	
            }
            //-----------------------------------------------------------------------------------�����Ž�����
        case SXC_SET_DOORCFG:
            {
                entrance_para_set(recv_para,data_head,client_fd);
                break;
            }
            //-----------------------------------------------------------------------------------��ȡ��ǰ�Ž�״̬
        case SXC_GET_DOOR_STATUS:
            {
                if(!entrance_status_send(recv_para,data_head,client_fd))
                {
                    printf_debug("send entrance status fail\n");
                    return -1;
                }
                break;
            }
            //-----------------------------------------------------------------------------------����״̬��ȡ
        case  SXC_ALARMCTL_DEV_GET_STATE_V2:
            {
                if(!alarm_status_get(recv_para,client_fd))
                {
                    printf_debug("send alarm_para fail\n");
                    return -1;
                }
                break;
            }
            //-------------------------------------------------------------------------------------��������
        case SXC_ALARMCTL_DEV_CTL_CMD_V2:
            {
                alarm_ctrl(recv_para,client_fd);
                break;
            }
            //-------------------------------------------------------------------------------------�ֶ������ϴ�
        case SXC_ALARM_UPLOAD_MANU:
            {
                alarm_upload_get(recv_para,client_fd);
                break;
            }
            //-------------------------------------------------------------------------------------���ñ�������
        case SXC_ALARMCTL_DEV_SET_PARAM:
            {
                if(!alarm_para_set(recv_para,client_fd))
                {
                    printf_debug("set alarm_para fail\n");
                    return -1;
                }
                break;
            }
            //-------------------------------------------------------------------------------------���ñ�����������
        case SXC_ALARMCTL_DEV_SET_PWD_PARAM:
            {
                if(!alarm_user_pwd_setup(recv_para,client_fd))
                {
                    printf_debug("set the password fail\n");
                    return -1;
                }			
                break;
            }
            //-------------------------------------------------------------------------------------���ñ���������ַ
        case SXC_ALARMCTL_DEV_SET_ADDR_PARAM:
            {
                if(!alarm_user_addr_setup(recv_para,client_fd))
                {
                    printf_debug("set the addr fail\n");
                    return -1;
                }
                break;
            }
            //-------------------------------------------------------------------------------------��ȡ��������
        case SXC_GET_ALARM_LINKAGE_CFG_V2:
            {
                if(!get_alarm_linkage(recv_para,client_fd))
                {
                    printf_debug("set the addr fail\n");
                    return -1;
                }
                break;
            }
            //-------------------------------------------------------------------------------------���ñ�������
        case SXC_SET_ALARM_LINKAGE_CFG_V2:
            {
                if(!set_alarm_linkage(recv_para,client_fd))
                {
                    printf_debug("set the addr fail\n");
                    return -1;
                }
                break;
            }
            //-------------------------------------------------------------------------------------��ȡ�����������ʱ��
        case SXC_GET_ALARM_OUTPUT_DURATION_TIME:
            {
                if(!alarmlinkage_output_time_get(recv_para,client_fd))
                {
                    printf_debug("get the alarmlinkage_time fail\n");
                    return -1;
                }
                break;			
            }
            //-------------------------------------------------------------------------------------���ñ����������ʱ��
        case SXC_SET_ALARM_OUTPUT_DURATION_TIME:
            {
                alarmlinkage_output_time_set(recv_para,client_fd);
                break;
            }
            //-------------------------------------------------------------------------------------ȡ������ʱ��Ĭ�ϲ�������
        case SXC_RESET_ALARM_LINKAGE_AND_TIMEPARA:
            {
                alarmlinkage_output_time_cancle(recv_para,client_fd);
                break;
            }
            //-------------------------------------------------------------------------------------��ȡ�������״̬
        case SXC_GETALAOUT_STATE:
            {
                if(!alarm_out_state(recv_para,client_fd))
                {
                    printf_debug("set out_state fail\n");
                    return -1;
                }
                break;
            }
            //-------------------------------------------------------------------------------------�������������
        case SXC_SETALAOUT:
            {
                if(!alarm_out_ctrl(recv_para,client_fd))
                {
                    printf_debug("set out_ctrl fail\n");
                    return -1;
                }	
                break;
            }
            //------------------------------------------------------------------------------------���������������ȡ
        case SXC_GET_ALARMOUTCFG:
            {
                if(!switch_para_send(recv_para,client_fd))
                {
                    printf_debug("set switch_para send fail\n");
                    return -1;
                }
                break;
            }
            //------------------------------------------------------------------------------------�����������������
        case SXC_SET_ALARMOUTCFG:
            {
                if(!switch_para_set(recv_para,client_fd))
                {
                    printf_debug("set switch_para send fail\n");
                    return -1;
                }
                break;
            }
            //------------------------------------------------------------------------------------�����������������
        case SXC_SET_ALARMINCFG:
            {
                if(!switch_in_para_set(recv_para,client_fd))
                {
                    printf_debug("set switch_in_para set fail\n");
                    return -1;
                }
                break;
            }
            //------------------------------------------------------------------------------------���������������ȡ
        case SXC_GET_ALARMINCFG:
            {

                break;
            }
            //------------------------------------------------------------------------------------��ȡ���������豸����
        case SXC_GET_CONNECT_PAMA:
            {
                if(!com_para_send(recv_para,client_fd))
                {
                    printf_debug("set com_para send fail\n");
                    return -1;
                }
                break;
            }
            //-----------------------------------------------------------------------------------���ô��������豸����
        case SXC_SET_CONNECT_PAMA:
            {
                if(!com_para_set(recv_para,client_fd))
                {
                    printf_debug("set com_para set fail\n");
                    return -1;
                }
                break;			
            }
            //------------------------------------------------------------------------------------��ȡ�豸����
        case SXC_GET_ICC_DEV_PARAM:
            {			
                if(!dev_para_get(recv_para,client_fd))
                {
                    printf_debug("get dev_para set fail\n");
                    return -1;
                }
                break;
            }
            //-------------------------------------------------------------------------------------�����豸
        case SXC_REBOOT:
            {
                if(!dev_restart(client_fd))
                {
                    printf_debug("restart system fail\n");
                    return -1;
                }
                break;
            }
            //-------------------------------------------------------------------------------------ϵͳУʱ
        case SXC_SET_TIMECFG:
            {
                if(!system_time_set(recv_para,client_fd))
                {
                    printf_debug("system time set fail\n");
                    return -1;
                }
                break;
            }
            //------------------------------------------------------------------------------------�ָ�Ĭ�ϲ���
        case SXC_PARA_RESTORE:
            {
                acquiescent_para_recovery(client_fd);
                break;
            }
        default:
            {
                printf_debug("nothing cmd\n");
                return send_act_cmd(ERRORDATA,client_fd,data_head->cmdWord,client_fd);//�������ݴ���״̬���ͻ���
                break;
            }
    }
    free(recv_para);//ע��˴��ı�Ҫ��

    return 1;
}

/******************************************************
Fun: ����������ip
para:   
timeout:
recv_buf, 
src_len��
 *********************************************************/
int set_lan_ip(unsigned int ip)
{ 
    int fd;
    struct ifreq ifr;
    struct sockaddr_in *sin;


    if ((fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        err_print("%s-%d--Creat socket Fail:%d:%s....\n", __FILE__, __LINE__, errno, strerror(errno));
        return -1;

    }

    memset(&ifr, 0, sizeof(ifr));
    memcpy(ifr.ifr_name, "eth0",  strlen("eth0") + 1);
    sin = (struct sockaddr_in *)&ifr.ifr_addr;
    sin->sin_family = AF_INET;
    sin->sin_addr.s_addr = (htonl)(ip);

    if (ioctl(fd, SIOCSIFADDR, &ifr) < 0)
    {
        err_print("%s-%d--ioctl Fail:%d:%s....\n", __FILE__, __LINE__, errno, strerror(errno));
    }

    errno = 0;
    cycle_close_socket(fd);

    return 0;

}

/******************************************************
Fun: �ͻ��˵�½����
para:   
timeout:
recv_buf, 
src_len��
 *********************************************************/
int log_in_reply(char *recv_para,INTER_SXC_HEAD *data_head,int client_fd)
{
    SX_INTER_SXC_LOGIN *setup_para = (SX_INTER_SXC_LOGIN *)recv_para;
    char usrPwd[32] = {0};

    printf_debug("%s come to login pwd:%s\n",setup_para->usrName,setup_para->usrPwd);
    channel_num_track();
    printf_debug("channel_num is :%d\n",channel_num_passive);
    if(channel_num_passive > 127)
    {
        printf("channel::%d��over 127\n",channel_num_passive);
        // channel_num_passive--;
        return -1;
    }
    else
    {
        if(data_head->version == SDK_VERSION)
        {	
             MD5String2(para_net.active_mode_pwd,usrPwd,strlen(para_net.active_mode_pwd));
            if((strcmp(setup_para->usrName,para_net.active_mode_name) == 0)&&(strncmp(setup_para->usrPwd,usrPwd,32) == 0))
            //if(strcmp(setup_para->usrName,para_net.active_mode_name) == 0)	
            {
                printf_debug("username is right\n");
                link_insert(client_fd);
                return send_act_cmd(QULIFIED,client_fd,SXC_LOG_IN,client_fd);
            }
            else
            {
                err_print("login_info wrong!\n");
                return send_act_cmd(ERRORPASSWD,client_fd,SXC_LOG_IN,client_fd);
            }
        }
        else
        {
            printf_debug("the SDK Version is wrong\n");
            return send_act_cmd(VERSIONNOMATCH,client_fd,SXC_LOG_IN,client_fd);

        }		 
    }
    return 1;
}

/******************************************************
Fun: ����ģʽ�µ�½����
para:   
timeout:
recv_buf, 
src_len��
 *********************************************************/
int login_apply(void)
{

    SX_INTER_SXD_LOGIN_SEND_V1 login_apply_data;
    int len = sizeof(SX_INTER_SXD_LOGIN_SEND_V1);
    char pwd_locked[32] = {0};
    int ret =1;

    memset(&login_apply_data,0,len);
    login_apply_data.device_send_data.UserId = htonl(server_connect_fd);

    MD5String2(para_net.active_mode_pwd,pwd_locked,strlen(para_net.active_mode_pwd));
    memcpy(login_apply_data.device_send_data.usrName, para_net.active_mode_name,16);
    memcpy(login_apply_data.device_send_data.usrPwd, pwd_locked, sizeof(pwd_locked));
    memcpy(login_apply_data.device_send_data.macAddr, para_net.byMACAddr, 6);
    //login_apply_data.device_send_data.byAlarmOutPortNum = 
    //login_apply_data.device_send_data.byAlarmInPortNum = 

#ifdef DEBUG_DATA
    printf("test login apply\n");
    printf("*****************************************************\n");
    printf("user name :%s\n",login_apply_data.device_send_data.usrName);
    printf("user password: %s\n",login_apply_data.device_send_data.usrPwd);
    printf("local mac: %x:%x:%x:%x:%x:%x\n",login_apply_data.device_send_data.macAddr[0],login_apply_data.device_send_data.macAddr[1],login_apply_data.device_send_data.macAddr[2],login_apply_data.device_send_data.macAddr[3],login_apply_data.device_send_data.macAddr[4],login_apply_data.device_send_data.macAddr[5]);
    printf("local mac: %x:%x:%x:%x:%x:%x\n",para_net.byMACAddr[0],para_net.byMACAddr[1],para_net.byMACAddr[2],para_net.byMACAddr[3],para_net.byMACAddr[4],para_net.byMACAddr[5]);

    printf("*****************************************************\n");
#endif

    login_apply_data.device_send_head.version = htonl(SDK_VERSION);
    login_apply_data.device_send_head.status = htonl(1);
    login_apply_data.device_send_head.requestID = htonl(SXD_LOG_IN);
    login_apply_data.device_send_head.length = htonl(len);
    memcpy(login_apply_data.device_send_head.clientMAC,para_net.byMACAddr, 6);	
    //memcpy(login_apply_data.device_send_data.macAddr, para_net.byMACAddr, 6);


    login_apply_data.device_send_head.checkSum = htonl(ret);

    return send_data_intime_over(server_connect_fd, MAX_TIME_OUT_MS, (char *)&login_apply_data, len);
    //turn send(server_connect_fd, (char *)&login_apply_data, len, MSG_NOSIGNAL);
}
/******************************************************
Fun: �������ʵ����Ӧ��ͻ���
para:   
timeout:
recv_buf, 
src_len��
 *********************************************************/
int cradle_ctrl_reply(char *recv_para,INTER_SXC_HEAD *data_head,int fd)
{
    //int i = 0;
    SX_INTER_PTZ_REQ *set_para = (SX_INTER_PTZ_REQ *)recv_para;

    set_para->ptzID = ntohl(set_para->ptzID);
    set_para->ptzCommand = ntohl(set_para->ptzCommand);
    set_para->ptzBit = ntohl(set_para->ptzBit);

#ifdef DEBUG_DATA   
    printf("Matrix_ctrl-->ptzID=%ld   ;ptzCommand=%ld   ;ptzBit=%ld\n",set_para->ptzID,set_para->ptzCommand,set_para->ptzBit);
#endif

    cradle_head_control_data.cradle_head_address = (char)(set_para->ptzID);
    if((set_para->ptzCommand == PTZ_UP)||
            (set_para->ptzCommand == PTZ_DOWN)||
            (set_para->ptzCommand == PTZ_LEFT)||
            (set_para->ptzCommand == PTZ_RIGHT))
    {
        cradle_head_control_data.cradle_head_move_speed = (char)(set_para->ptzBit);
    }
    if((set_para->ptzCommand == PTZ_SET_PTZBIT)||
            (set_para->ptzCommand == PTZ_CLE_PTZBIT))
    {
        cradle_head_control_data.cradle_head_preset_point = (char)(set_para->ptzBit);
    }

    cradle_head_control_data.setup_command_set = set_para->ptzCommand;
    //printf("FUNC[%s] LINE[%d]\tcradle_head_control_data.setup_command_set____ = %d\n",__FUNCTION__, __LINE__, cradle_head_control_data.setup_command_set);

#if 0     
    switch(set_para->ptzCommand)
    {
        case 0:     
            cradle_head_control_data.setup_command_set = PTZ_UP; 
            cradle_head_control_data.cradle_head_move_speed = (char)(set_para->ptzBit);
            break;
        case 1:     
            cradle_head_control_data.setup_command_set = PTZ_DOWN;
            cradle_head_control_data.cradle_head_move_speed = (char)(set_para->ptzBit);
            break;
        case 2:     
            cradle_head_control_data.setup_command_set = PTZ_LEFT; 
            cradle_head_control_data.cradle_head_move_speed = (char)(set_para->ptzBit);
            break;
        case 3:    
            cradle_head_control_data.setup_command_set = PTZ_RIGHT;
            cradle_head_control_data.cradle_head_move_speed = (char)(set_para->ptzBit);
            break;
        case 4 :     
            cradle_head_control_data.setup_command_set = PTZ_APERTURE_BIG; 
            break;
        case 5:
            cradle_head_control_data.setup_command_set = PTZ_APERTURE_SMALL;
            break;
        case 6:
            cradle_head_control_data.setup_command_set = PTZ_FOCUS_IN;
            break;
        case 7:
            cradle_head_control_data.setup_command_set = PTZ_FOCUS_OUT; 
            break;
        case 8:
            cradle_head_control_data.setup_command_set = PTZ_ZOOM_IN;
            break;
        case 9:
            cradle_head_control_data.setup_command_set = PTZ_ZOOM_OUT;
            break;
        case 10:
            cradle_head_control_data.setup_command_set = PTZ_AUTO_ZOOM;
            break;
        case 11:
            cradle_head_control_data.setup_command_set = PTZ_AUTO_FOCUS;
            break;
        case 12:
            cradle_head_control_data.setup_command_set = PTZ_AUTO_APERTURE;
            break;
        case 13:    
            cradle_head_control_data.setup_command_set = PTZ_SET_PTZBIT; 
            cradle_head_control_data.cradle_head_preset_point = (char)(set_para->ptzBit);
            break;
        case 14:    
            cradle_head_control_data.setup_command_set = PTZ_CLE_PTZBIT;
            cradle_head_control_data.cradle_head_preset_point = (char)(set_para->ptzBit);
            break;
        case 15:
            cradle_head_control_data.setup_command_set = PTZ_STOP_ALL;
            break;
        case 16:
            cradle_head_control_data.setup_command_set = AUX_PWRON1;
            break;
        case 17:
            cradle_head_control_data.setup_command_set = AUX_PWROFF1;
            break;
        case 18:
            cradle_head_control_data.setup_command_set = AUX_PWRON2; 
            break;
        case 19:
            cradle_head_control_data.setup_command_set = AUX_PWROFF2; 
            break;
        case 20:
            cradle_head_control_data.setup_command_set = PTZ_EX_LEFT_UP;
            break;
        case 21:
            cradle_head_control_data.setup_command_set = PTZ_EX_LEFT_DOWN;
            break;
        case 22:
            cradle_head_control_data.setup_command_set = PTZ_EX_RIGHT_UP; 
            break;
        case 23:
            cradle_head_control_data.setup_command_set = PTZ_EX_RIGHT_DOWN;
            break;
        default:
            return send_act_cmd(ERRORDATA,fd,SXC_PTZ,fd);//�������ݴ���״̬���ͻ���
            break;
    }
#endif	 


    return send_act_cmd(QULIFIED,fd,SXC_PTZ,fd);//�������ݴ���״̬���ͻ���
}

/******************************************************
Fun:Զ�����������������
para:   
timeout:
recv_buf, 
src_len��
 *********************************************************/
int net_para_set(char *recv_para,INTER_SXC_HEAD *data_head,int fd,int client_ip)
{
    INTER_NETCFG *net_para_client_set = (INTER_NETCFG *)recv_para;
    FILE *fp;
    char ipbuf[4][16] = {{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}};
    char write_buf[200] = {0};
	
    printf_debug("start to set net_para\n");
    //�������������ε�ַ
    para_net.dwDeviceIPMask = ntohl(net_para_client_set->dwDeviceIPMask);
    //���ip�б����������Ч������Ӧ�ı������
    if(para_net.dwDeviceIP != ntohl(net_para_client_set->dwDeviceIP))
    {	
        para_net.dwDeviceIP = ntohl(net_para_client_set->dwDeviceIP);
        if(ip_judge(para_net.dwDeviceIPMask,para_net.dwDeviceIP,client_ip))
        {	
            set_lan_ip(para_net.dwDeviceIP);
            //�˴�Ӧ�����ݱ��浽�ļ��з�ֹ����ʱ���ݶ�ʧ(������)
        }
        else
        {	
            //���ip���ڸ���������ip��ƥ�����ظ�
            return send_act_cmd(NOSUPPORT ,fd,SXC_SET_NETCFG,fd);
        }	
    }
    //�豸�˿� �����ַ     Զ�����ĵ�ip�Ͷ˿� 
    if(strcmp(para_net.byMACAddr,net_para_client_set->byMACAddr) == 0)
    {
        set_local_lan_mac(net_para_client_set->byMACAddr);
    }
    para_net.wDevicePort = ntohs(net_para_client_set->wDevicePort);
   // para_net.dwGatewayIP = ntohl(net_para_client_set->dwGatewayIP);
    para_net.dwManageHostIP = ntohl(net_para_client_set->dwManageHostIP);
    para_net.wManageHostPort = ntohs(net_para_client_set->wManageHostPort);
    //����ģʽ�û���֤���û���������
    memcpy(para_net.active_mode_name,net_para_client_set->active_mode_name, 16);
    memcpy(para_net.active_mode_pwd,net_para_client_set->active_mode_pwd, 32);

#ifdef DEBUG_DATA
    printf("wdeviceip:%x\n",para_net.dwDeviceIP);
    printf("wDevicePort:%d\n",para_net.wDevicePort);
    printf("active_mode_name:%s\n",para_net.active_mode_name);
    printf("active_mode_pwd:%s\n",para_net.active_mode_pwd);
    //printf("byMACAddr:%x\n",para_net.byMACAddr);
    printf("wManageHostPort:%d\n",para_net.wManageHostPort);
    printf("dwSize:%d\n",para_net.dwSize);
    printf("dwGatewayIP:%x\n",para_net.dwGatewayIP);
    printf("dwManageHostIP:%x\n",para_net.dwManageHostIP);
#endif

    if(!(fp = fopen(net_para_file_path,"wt+")))
    {
        err_print("open file fail\n");
        return send_act_cmd(NOSUPPORT,fd,SXC_SET_NETCFG,fd);
    }

    //����������õı�־λ


    inet_ntop(AF_INET,&net_para_client_set->dwDeviceIP,ipbuf[0],INET_ADDRSTRLEN);


    inet_ntop(AF_INET,&net_para_client_set->dwDeviceIPMask,ipbuf[1],INET_ADDRSTRLEN);


    inet_ntop(AF_INET,&net_para_client_set->dwGatewayIP,ipbuf[2],INET_ADDRSTRLEN);


    inet_ntop(AF_INET,&net_para_client_set->dwManageHostIP,ipbuf[3],INET_ADDRSTRLEN);

    // MD5String2(para_net.active_mode_pwd,pwd_locked,strlen(para_net.active_mode_pwd));

    sprintf(write_buf,"%d,%s,%s,%s,%s,%d,%d,%s,%s%c",1,ipbuf[0],ipbuf[1],ipbuf[2],ipbuf[3],para_net.wDevicePort,para_net.wManageHostPort,para_net.active_mode_name,para_net.active_mode_pwd,0);

	
    fprintf(fp,"%s",write_buf);

#ifdef DEBUG_DATA
	printf("net_para_file_content1:%s\n",ipbuf[0]);
	printf("net_para_file_content:%s\n",ipbuf[1]);
	printf("net_para_file_content:%s\n",ipbuf[2]);
	printf("net_para_file_content:%s\n",ipbuf[3]);
	printf("net_para_file_content:%d\n",para_net.wDevicePort);
	printf("net_para_file_content:%d\n",para_net.wManageHostPort);
	printf("net_para_file_content:%s\n",para_net.active_mode_name);
	printf("net_para_file_content8:%s\n",para_net.active_mode_pwd);
#endif


    fclose(fp);
    return send_act_cmd(QULIFIED ,fd,SXC_SET_NETCFG,fd);

}

/******************************************************
Fun:��ȡԶ���������õ��������
para:   
timeout:
recv_buf, 
src_len��
 *********************************************************/
int net_para_send(char *recv_para,INTER_SXC_HEAD *data_head,int fd)
{
    INTER_NETCFG_SEND send_buf;
    int check_ret,len;

    memset(&send_buf, 0, sizeof(INTER_NETCFG_SEND));
    send_buf.para_net_send.dwDeviceIPMask = htonl(para_net.dwDeviceIPMask);
    send_buf.para_net_send.dwDeviceIP =  htonl(para_net.dwDeviceIP);
    send_buf.para_net_send.wDevicePort = htons(para_net.wDevicePort);
    send_buf.para_net_send.dwGatewayIP = htonl(para_net.dwGatewayIP);
    memcpy(send_buf.para_net_send.byMACAddr,para_net.byMACAddr, 6);
    send_buf.para_net_send.dwManageHostIP =htonl( para_net.dwManageHostIP);
    send_buf.para_net_send.wManageHostPort = htons(para_net.wManageHostPort);
    memcpy(send_buf.para_net_send.active_mode_name,para_net.active_mode_name,16);
    memcpy(send_buf.para_net_send.active_mode_pwd,para_net.active_mode_pwd,16);

    printf_debug("name:%s\npwd:%s\n",para_net.active_mode_name,para_net.active_mode_pwd);
    check_ret=1; //checksum_send(&ret_cmd.sxdHeader,0);
    send_buf.sxdHeader.version = htonl(SDK_VERSION);
    send_buf.sxdHeader.checkSum=htonl(check_ret);
    send_buf.sxdHeader.requestID = htonl(SXC_GET_NETCFG);
    send_buf.sxdHeader.status = htonl(QULIFIED); 						 //**************�˴��Ƿ�Ϊȷ������?
    send_buf.sxdHeader.length = htonl(sizeof(INTER_SXD_DOOR_PARA_SEND));
    len = sizeof(INTER_NETCFG_SEND);

    printf_debug("net_para_package length is %d\n",len);
    return send_data_intime_over(fd, SXC_GET_NETCFG, (char *)&send_buf, len);
}

/******************************************************
Fun:����ͨ��ȡ�����
para:   
timeout:
recv_buf, 
src_len��
 *********************************************************/
int MATRIX_ctrl_cancel_singal(char *recv_para,INTER_SXC_HEAD *data_head,int fd)
{
    SX_INTER_MATRIX_REQ *set_para = (SX_INTER_MATRIX_REQ *)recv_para;

    set_para->matrixID = ntohl(set_para->matrixID);
    set_para->nValueIn = ntohl(set_para->nValueIn);
    set_para->nValueOut = ntohl(set_para->nValueOut);

    matrix_control_data.setup_command_set = MATRIX_CANCEL_SWITCHOVER_COMMAND;
    matrix_control_data.matrix_current_switchover_in_value = set_para->nValueIn;
    matrix_control_data.matrix_current_switchover_out_value = set_para->nValueOut;
    matrix_control_data.current_fd = fd;

    return 1;
}

/******************************************************
Fun:����ȫͨ��ȡ�����
para:   
timeout:
recv_buf, 
src_len��
 *********************************************************/
int MATRIX_ctrl_cancel_all(char *recv_para,INTER_SXC_HEAD *data_head,int fd)
{
    SX_INTER_MATRIX_REQ *set_para = (SX_INTER_MATRIX_REQ *)recv_para;

    set_para->matrixID = ntohl(set_para->matrixID);
    set_para->nValueIn = ntohl(set_para->nValueIn);
    set_para->nValueOut = ntohl(set_para->nValueOut);
    matrix_control_data.current_fd = fd;
    matrix_control_data.setup_command_set = MATRIX_CANCEL_ALL_SWITCHOVER_COMMAND;

    return 1;
}
/******************************************************
Fun:�����л�
para:   
timeout:
recv_buf, 
src_len��
 *********************************************************/
int MATRIX_ctrl_change(char *recv_para,INTER_SXC_HEAD *data_head,int fd)
{
    SX_INTER_MATRIX_REQ *set_para = (SX_INTER_MATRIX_REQ *)recv_para;

    set_para->matrixID = ntohl(set_para->matrixID);
    set_para->nValueIn = ntohl(set_para->nValueIn);
    set_para->nValueOut = ntohl(set_para->nValueOut);
    printf_debug("matrixid= %ld--in= %ld--out= %ld\n",set_para->matrixID,set_para->nValueIn,set_para->nValueOut);

    //matrix_control_data.matrix_id = set_para->matrixID;
    if((set_para->nValueIn <= MATRIX_MAX_CHANNEL_IN)&&(set_para->nValueOut <= MATRIX_MAX_CHANNEL_OUT)&&(set_para->nValueIn >= 0)&&(set_para->nValueOut >= 0))
    {
        matrix_control_data.matrix_current_switchover_in_value = set_para->nValueIn;
        matrix_control_data.matrix_current_switchover_out_value = set_para->nValueOut;
    }
    else
    {
        return send_act_cmd(IPMISMATCH ,fd,SXC_EXCHAGE_MATRIX_INOUT,fd);//�豸��֧�ָò���
    }

    matrix_control_data.setup_command_set = MATRIX_SWITCHOVER_COMMAND;
    matrix_control_data.current_fd = fd;
#ifdef DEBUG_DATA
    printf("matrix_value_input:%d\n",matrix_control_data.matrix_current_switchover_in_value);
    printf("matrix_value_output:%d\n",matrix_control_data.matrix_current_switchover_out_value);
#endif

    return 1;
}


/******************************************************
Fun: �����ſ���
para:   
timeout:
recv_buf, 
src_len��
 *********************************************************/
int entrance_open_close_ctrl(char *recv_para,INTER_SXC_HEAD *data_head,int fd)
{
    SXC_INTER_DOOR_CTL_CMD *set_para = (SXC_INTER_DOOR_CTL_CMD *)recv_para;
    entrance_guard_data.current_fd = fd;

    if(set_para->byCmdType == 0x01)
    {
        entrance_guard_data.setup_command_set = ENTRANCE_GUARD_OPEN_DOOR;
    }
    else if(set_para->byCmdType == 0x00)
    {
        entrance_guard_data.setup_command_set = ENTRANCE_GUARD_CLOSE_DOOR;
    }

#if 0
    printf("DoorID:%x\n",set_para->byDoorNo);
    printf("DOORCMD:%x\n",set_para->byCmdType);
#endif

    return 1;
}

/******************************************************
Fun:�Ž���������
para:   
timeout:
recv_buf, 
src_len��
 *********************************************************/
int entrance_para_get(char *recv_para,INTER_SXC_HEAD *data_head,int fd)
{
    INTER_SXD_DOOR_PARA_SEND door_para;
    // SX_INTER_DOOR_PARA_REQ *recv_buf = (SX_INTER_DOOR_PARA_REQ *)recv_para;
    int check_ret,len;

    memset(&door_para, 0, sizeof(INTER_SXD_DOOR_PARA_SEND));
    door_para.sxddoorpara.byDelayCloseDoor1 =entrance_guard_data.client_set_door_hold_time;
    door_para.sxddoorpara.byDelayCloseDoor2 = entrance_guard_data.button_set_door_hold_time;
    door_para.sxddoorpara.byDevNo = 1;
    door_para.sxddoorpara.byDoorContMode = entrance_guard_data.door_contact_detection_mode_setup;
    door_para.sxddoorpara.byDoorLock =entrance_guard_data.door_lock_relay_status_setup;
    door_para.sxddoorpara.byDoorNo = 1;

#ifdef DEBUG_DATA
    printf("door_lock_relay_status_setup:%d\n",door_para.sxddoorpara.byDoorLock);
    printf("door_contact_detection_mode_setup:%d\n",door_para.sxddoorpara.byDoorContMode);
    printf("client_set_door_hold_time:%d\n",door_para.sxddoorpara.byDelayCloseDoor2);
    printf("button_set_door_hold_time:%d\n",door_para.sxddoorpara.byDelayCloseDoor1);
    printf("doorID:%d,devID:%d\n",door_para.sxddoorpara.byDoorNo,door_para.sxddoorpara.byDevNo);
#endif

    check_ret=1; //checksum_send(&ret_cmd.sxdHeader,0);
    door_para.sxdHeader.checkSum=htonl(check_ret);
    door_para.sxdHeader.requestID = htonl(SXC_GET_DOORCFG);
    door_para.sxdHeader.status = htonl(QULIFIED); 						 //**************�˴��Ƿ�Ϊȷ������?
    door_para.sxdHeader.length = htonl(sizeof(INTER_SXD_DOOR_PARA_SEND));
    len = sizeof(INTER_SXD_DOOR_PARA_SEND);
    door_para.sxdHeader.version = htonl(SDK_VERSION);

    return send_data_intime_over(fd, MAX_TIME_OUT_MS, (char *)&door_para, len);
}

/******************************************************
Fun:�Ž���������
para:   
timeout:
recv_buf, 
src_len��
 *********************************************************/
int entrance_para_set(char *recv_para,INTER_SXC_HEAD *data_head,int fd)
{
    SX_INTER_DOOR_PARA_REQ *entrance_para = (SX_INTER_DOOR_PARA_REQ *)recv_para;

    entrance_guard_data.current_fd = fd;
    switch(entrance_para->byReserved[0])
    {
        case 0x00: //����
            if(entrance_para->byDoorLock == 0)
                entrance_guard_data.setup_command_set = ENTRANCE_GUARD_RELAY_OPEN_DOOR;
            else
                entrance_guard_data.setup_command_set = ENTRANCE_GUARD_RELAY_CLOSE_DOOR;
            break;
        case 0x01: //�ͻ��������ű���ʱ��
            entrance_guard_data.setup_command_set = ENTRANCE_GUARD_CLIENT_SET_DOOR_HOLD_TIME;
            entrance_guard_data.client_set_door_hold_time = entrance_para->byDelayCloseDoor1;
            break;
        case 0x02: //���Ű�ť�����ű���ʱ��
            entrance_guard_data.setup_command_set = ENTRANCE_GUARD_BUTTON_SET_DOOR_HOLD_TIME;
            entrance_guard_data.button_set_door_hold_time = entrance_para->byDelayCloseDoor2;
            break;
            /*
               case 0x07: //��ȡ��ͨ��Ϣ
            //entrance_guard_data.setup_command_set = ENTRANCE_GUARD_GET_NORMAL_MESSAGE;
            break;
            case 0x08: //��ȡ������Ϣ
            //entrance_guard_data.setup_command_set = ENTRANCE_GUARD_GET_ALARM_MESSAGE;
            case 0x0b: //�Ŵ�ȡ������״̬
            //entrance_guard_data.setup_command_set = ENTRANCE_GUARD_DOOR_CONTACT_NORMALLY_CLOSE;
            entrance_guard_data.if_has_entrance_guard_alarm = NO;
            printf_debug("FUNC[%s] LINE[%d]\tEntrance guard succeed to cancel alarm!\n",__FUNCTION__, __LINE__);
            //navy ���緢�� �Ŵ�ȡ�������ɹ�
            entrance_guard_data.setup_command_set = ENTRANCE_GUARD_CANCEL_ALARM_STATUS;
            break;

*/
        case 0x03: //�Ŵ�״̬����
            if(entrance_para->byDoorContMode == 0)
                entrance_guard_data.setup_command_set = ENTRANCE_GUARD_DOOR_CONTACT_NORMALLY_OPEN;
            else
                entrance_guard_data.setup_command_set = ENTRANCE_GUARD_DOOR_CONTACT_NORMALLY_CLOSE;
            break;
        default:   //��Чָ��
            //���緢����Ч����
            printf_debug("FUNC[%s] LINE[%d]\tEntrance guard invalid net command!\n",__FUNCTION__, __LINE__);

            send_act_cmd(NOSUPPORT  ,fd,SXC_SET_DOORCFG,fd);		
            break;
    }

    return 1;

}

/******************************************************
Fun:�Ž�״̬��ȡ
para:   
timeout:
recv_buf, 
src_len��
 *********************************************************/
int entrance_status_send(char *recv_para,INTER_SXC_HEAD *data_head,int fd)
{
    INTER_SXD_DOOR_STATUS_SEND door_status;
    int check_ret,len;
    INTER_DOOR_STATUS *recv_buf = (INTER_DOOR_STATUS *)recv_para;

    memset(&door_status, 0, sizeof(INTER_SXD_DOOR_STATUS_SEND));
    door_status.sxddoorstate.byDevNo = recv_buf->byDevNo;
    door_status.sxddoorstate.byDoorStatus = entrance_guard_data.door_status;
    door_status.sxddoorstate.byDoorNo = recv_buf->byDoorNo;

#ifdef DEBUG_DATA
    printf("byDoorContactDetectionMode:%d\n",door_status.sxddoorstate.byDoorContactDetectionMode);
    printf("byDoorLockRelayStatus:%d\n",door_status.sxddoorstate.byDoorLockRelayStatus);
    printf("byDoorStatus:%d\n",door_status.sxddoorstate.byDoorStatus);
#endif

    check_ret=1; //checksum_send(&ret_cmd.sxdHeader,0);
    door_status.sxdHeader.checkSum=htonl(check_ret);
    door_status.sxdHeader.requestID = htonl(SXC_GET_DOOR_STATUS);
    door_status.sxdHeader.status = htonl(QULIFIED); 						 //**************�˴��Ƿ�Ϊȷ������?
    door_status.sxdHeader.length = htonl(sizeof(INTER_SXD_DOOR_STATUS_SEND));
    len = sizeof(INTER_SXD_DOOR_STATUS_SEND);
    door_status.sxdHeader.version = htonl(SDK_VERSION);

    return send_data_intime_over(fd, MAX_TIME_OUT_MS, (char *)&door_status, len);
}

/******************************************************
Fun:����״̬��ȡ
para:   
timeout:
recv_buf, 
src_len��
 *********************************************************/
int alarm_status_get(char *recv_buf,int fd)
{
    INTER_SXD_ALARM_STATUS_SEND alarm_status;
    int check_ret,len;
    SXC_INTER_ALARMCTL_DEV_GET_STATE *recv_status = (SXC_INTER_ALARMCTL_DEV_GET_STATE *)recv_buf;

    memset(&alarm_status, 0, sizeof(INTER_SXD_ALARM_STATUS_SEND));

    //��������ID
    alarm_status.sxdalarmstate.byDevNo = 1;
    //���ε�Ϊ��������
    if(recv_status->byCmdType  == 0)
    {	  
        alarm_status.sxdalarmstate.byDevState = ck2316_alarm_data.if_ck2316_alive;
    }
    else if(recv_status->byCmdType == 1) //�˴�Ӧ���뵱�豸δ������ʱ���ѯʧ�ܵĲ���
    {
        alarm_status.sxdalarmstate.byDevState = ck2316_alarm_data.if_ck2316_alive;
        alarm_status.sxdalarmstate.byDevDefenceState = ck2316_alarm_data.ck2316_defence_status;

        //ͨ�������ݵ�ת��
        alarm_status.sxdalarmstate.byAlarmState.byBypassState = (( ck2316_alarm_data.ck2316_defence_area_bypass_value[1]<<8)|(ck2316_alarm_data.ck2316_defence_area_bypass_value[0]));
        alarm_status.sxdalarmstate.byAlarmState.byAlarmState = ((ck2316_alarm_data.ck2316_defence_area_alarm_memory_value[1]<<8)|(ck2316_alarm_data.ck2316_defence_area_alarm_memory_value[0]));
        alarm_status.sxdalarmstate.byAlarmState.byDefenceAlarmType = ck2316_alarm_data.ck2316_defence_area_alarm_status;

        alarm_status.sxdalarmstate.byAlarmState.byBypassState = htonl(alarm_status.sxdalarmstate.byAlarmState.byBypassState);
        alarm_status.sxdalarmstate.byAlarmState.byAlarmState = htonl(alarm_status.sxdalarmstate.byAlarmState.byAlarmState);
        alarm_status.sxdalarmstate.byAlarmState.byDefenceAlarmType = htonl(alarm_status.sxdalarmstate.byAlarmState.byDefenceAlarmType);
    }
    else if(recv_status->byCmdType == 2) 
    {
        alarm_status.sxdalarmstate.byDevDefenceState = ck2316_alarm_data.ck2316_defence_status;
    }
    else
    {
        //���ʹ���(�豸��֧�ָò���)
        return send_act_cmd(NOSUPPORT  ,fd,SXC_ALARMCTL_DEV_GET_STATE_V2,fd);
    }

#ifdef DEBUG_DATA
    printf("byDevState:%d\n",alarm_status.sxdalarmstate.byDevState);
    printf("byDevshe fang zhuang tai:%d\n",alarm_status.sxdalarmstate.byDevDefenceState);
    printf("byAlarmState2222:%d\n",alarm_status.sxdalarmstate.byAlarmState.byAlarmState);
    printf("byBypassState:%d\n",alarm_status.sxdalarmstate.byAlarmState.byBypassState);
#endif

    alarm_status.sxdalarmstate.byCmdType = recv_status->byCmdType;
    check_ret=1; //checksum_send(&ret_cmd.sxdHeader,0);
    alarm_status.sxdHeader.checkSum=htonl(check_ret);
    alarm_status.sxdHeader.requestID = htonl(SXC_ALARMCTL_DEV_GET_STATE_V2);
    alarm_status.sxdHeader.status = htonl(QULIFIED); 						 //**************�˴��Ƿ�Ϊȷ������?
    alarm_status.sxdHeader.length = htonl(sizeof(INTER_SXD_ALARM_STATUS_SEND));
    len = sizeof(INTER_SXD_ALARM_STATUS_SEND);
    alarm_status.sxdHeader.version = htonl(SDK_VERSION);

    return send_data_intime_over(fd, MAX_TIME_OUT_MS, (char *)&alarm_status, len); 
}

/******************************************************
Fun:  ����������
para:   
timeout:
recv_buf, 
src_len��
 *********************************************************/
int alarm_ctrl(char *recv_buf,int fd)
{
    SXC_INTER_ALARMCTL_DEV_CTL_CMD *alarm_ctrl_cmd = (SXC_INTER_ALARMCTL_DEV_CTL_CMD *)recv_buf;

    ck2316_alarm_data.current_fd = fd;

#ifdef DEBUG_DATA
    printf("setup_command_set:%d\n",alarm_ctrl_cmd->byCmdType);
#endif
    /*
       6�����������û���������
       5����������ģ����̵�ַ����
       4������������λ
       3��ϵͳ��·
       2��������������
       1��ϵͳ����
       0��ϵͳ��?
       */ 
#if 1
    switch(alarm_ctrl_cmd->byCmdType)
    {
        case 2:
            {
                ck2316_alarm_data.setup_command_set = CK2316_SYSTEM_DEFENCE;
                break;
            }
        case 3:
            {
                ck2316_alarm_data.setup_command_set = CK2316_SYSTEM_ABANDON;
                break;
            }
        case 4:
            {
                ck2316_alarm_data.setup_command_set = CK2316_ELIMINATE_ALARM_MEMORY;
                break;
            }
        case 6:
            {
                //�ͻ��˲��Գ�������   Ϊ�����޸�  �������޸�
                alarm_ctrl_cmd->wGuardNo = ntohs(alarm_ctrl_cmd->wGuardNo);
                printf_debug("fang qu ID %d\n",alarm_ctrl_cmd->wGuardNo);
                if (alarm_ctrl_cmd->wGuardNo > 0 && alarm_ctrl_cmd->wGuardNo <= 16) //16������
                {         		
                    bypass_defence_area_code[5][2] = (alarm_ctrl_cmd->wGuardNo)/10;
                    bypass_defence_area_code[6][2] = (alarm_ctrl_cmd->wGuardNo)%10;
                    ck2316_alarm_data.setup_command_set = CK2316_BYPASS_DEFENCE_AREA;
                }
                else
                {
                    return send_act_cmd(ERRORDATA ,fd,SXC_SET_DOORCFG,fd);  
                }
                break;
            }
        case 5:
            {
                break;
            }
        default:
            {
                break;
            }
    }
#endif 
    return 1;
}

/******************************************************
Fun: ���ñ�����������
para:   
timeout:
recv_buf, 
src_len��
 *********************************************************/
int alarm_user_pwd_setup(char *recv_buf,int fd)
{
    SXC_ALARMCTL_PWD_PARA *pwd_alarm = (SXC_ALARMCTL_PWD_PARA *)recv_buf;
    if (pwd_alarm->byPassword[0] <= 0x0F && pwd_alarm->byPassword[1] <= 0x0F && pwd_alarm->byPassword[2] <= 0x0F && pwd_alarm->byPassword[3] <= 0x0F) 
    {
        ck2316_alarm_data.ck2316_user_password[0] = pwd_alarm->byPassword[0];
        ck2316_alarm_data.ck2316_user_password[1] = pwd_alarm->byPassword[1];
        ck2316_alarm_data.ck2316_user_password[2] = pwd_alarm->byPassword[2];
        ck2316_alarm_data.ck2316_user_password[3] = pwd_alarm->byPassword[3];
        ck2316_user_password_setup(ck2316_alarm_data.ck2316_user_password);

        //���緢�� �������óɹ�
        send_act_cmd(QULIFIED,fd,SXC_ALARMCTL_DEV_SET_PWD_PARAM,fd);
#ifdef DEBUG_DATA
        printf("\n\nCK2316 change keyboard password as: %02d %02d %02d %02d\n\n",ck2316_alarm_data.ck2316_user_password[0], ck2316_alarm_data.ck2316_user_password[1], ck2316_alarm_data.ck2316_user_password[2], ck2316_alarm_data.ck2316_user_password[3]);
#endif
    }
    else
    {
        //���緢�� ��������ʧ�ܣ����������ֵ
        return send_act_cmd(ERRORDATA ,fd,SXC_ALARMCTL_DEV_SET_PWD_PARAM,fd);
#ifdef DEBUG_DATA
        printf("\n\nWarning ! CK2316 change password failed,wrong password value!\n\n");
#endif
    }
    return 1;
}

/******************************************************
Fun: ���ñ���������ַ
para:   
timeout:
recv_buf, 
src_len��
 *********************************************************/
int alarm_user_addr_setup(char *recv_buf,int fd)
{
    SXC_ALARMCTL_ADDR_PARA *addr_alarm = (SXC_ALARMCTL_ADDR_PARA *)recv_buf;
    if (addr_alarm->byAddress <= 0x0F) 
    {
        ck2316_alarm_data.ck2316_simulate_keyboard_address = addr_alarm->byAddress;
        ck2316_simulate_keyboard_address_setup(ck2316_alarm_data.ck2316_simulate_keyboard_address);

        //���緢�� ��ַ�����óɹ�
        send_act_cmd(QULIFIED,fd,SXC_ALARMCTL_DEV_SET_ADDR_PARAM,fd);
#ifdef DEBUG_DATA
        printf("\n\nCK2316 change keyboard address as: 0x%02X\n\n",ck2316_alarm_data.ck2316_simulate_keyboard_address);
#endif
        //ck2316_alarm_data.setup_command_set = CK2316_SIMULATE_KEYBOARD_ADDRESS_SETUP;
    }
    else
    {
        //���緢�� ��ַ������ʧ�ܣ�����ĵ�ַ��
        return send_act_cmd(ERRORDATA ,fd,SXC_ALARMCTL_DEV_SET_ADDR_PARAM,fd);
#ifdef DEBUG_DATA
        printf("\n\nWarning ! CK2316 change keyboard address failed,wrong address !\n\n");
#endif
    }
    return 1;
}

/******************************************************
Fun: ���ñ�����������
para:   
timeout:
recv_buf, 
src_len��
 *********************************************************/
int set_alarm_linkage(char *recv_buf,int fd)
{
    ALARM_LINKAGE_PARA *alarm_linkage_para = (ALARM_LINKAGE_PARA *)recv_buf;
    alarm_input_output_data.current_fd = fd;

    if (alarm_input_output_data.real_time_alarm_output_objcet) 
    {
        //���緢����ȡ�����б��������Ȼ�������ñ���������Ӧ��ϵ
        printf_debug("FUNC[%s] LINE[%d]\tPlease cancel alarm output, then try it again!\n",__FUNCTION__, __LINE__);
        send_act_cmd(NOSUPPORT ,fd,SXC_GET_ALARM_LINKAGE_CFG_V2,fd);
    }
    else
    {
        if( (alarm_linkage_para->byAlarmInType < 1 )|| (alarm_linkage_para->byAlarmInType > MAX_ALARM_LINKAGE_INPUT_OBJECT)) 
        {
            //���緢�� ��Ч����
            printf_debug("FUNC[%s] LINE[%d]\tInvalid arg alarm_linkage_input_object, it should be 1 to 9\n",__FUNCTION__, __LINE__);
            send_act_cmd(ERRORDATA ,fd,SXC_GET_ALARM_LINKAGE_CFG_V2,fd);
        }
        else
        {
            alarm_input_output_data.alarm_linkage_input_object = alarm_linkage_para->byAlarmInType;
            alarm_input_output_data.alarm_input_linkage_output_object[alarm_input_output_data.alarm_linkage_input_object-1] = ntohl(alarm_linkage_para->byAlarmLinkOutChan);
            alarm_input_output_data.setup_command_set = ALARM_INPUT_OUTPUT_SET_LINKAGE_INFO;
        }
    }

    return 1;
}

/******************************************************
Fun: ���ñ����������ʱ������
para:   
timeout:
recv_buf, 
src_len��
 *********************************************************/
int alarmlinkage_output_time_set(char *recv_buf,int fd)
{
    ALARM_OUTPUT_DURATION_CFG_V1 *time_para_set = (ALARM_OUTPUT_DURATION_CFG_V1 *)recv_buf;
    alarm_input_output_data.current_fd = fd;
    if (alarm_input_output_data.real_time_alarm_output_objcet) 
    {
        //���緢����ȡ�����б��������Ȼ�������ñ���������Ӧ��ϵ
        send_act_cmd(NOSUPPORT ,fd,SXC_SET_ALARM_OUTPUT_DURATION_TIME,fd);
        printf_debug("FUNC[%s] LINE[%d]\tPlease cancel alarm output, then try it again!\n",__FUNCTION__, __LINE__);
    }
    else
    {
        //  if( (time_para_set->byAlarmOutChan < 0 )||(time_para_set->byAlarmOutChan > MAX_ALARM_INPUT_LINKAGE_OUTPUT_OBJECT)) 
        if( time_para_set->byAlarmOutChan > MAX_ALARM_INPUT_LINKAGE_OUTPUT_OBJECT) 
        {
            //���緢�� ��Ч����
            send_act_cmd(ERRORDATA ,fd,SXC_SET_ALARM_OUTPUT_DURATION_TIME,fd);
            printf_debug("FUNC[%s] LINE[%d]\tInvalid arg alarm_linkage_output_object, it should be 1 to 8\n",__FUNCTION__, __LINE__);                                
        }
        else
        {
            alarm_input_output_data.alarm_linkage_output_object = time_para_set->byAlarmOutChan;
            alarm_input_output_data.alarm_duration = 60*time_para_set->byHour + time_para_set->byMinute;
            if (alarm_input_output_data.alarm_duration > MAX_ALARM_DURATION) 
            {
                alarm_input_output_data.alarm_duration = MAX_ALARM_DURATION;
            }
            alarm_input_output_data.setup_command_set = ALARM_INPUT_OUTPUT_SET_ALARM_DURATION;
        }
    }				

    return 1;
}

/******************************************************
Fun: ��ȡ�����������ʱ���ȡ
para:   
timeout:
recv_buf, 
src_len��
 *********************************************************/
int alarmlinkage_output_time_get(char *recv_buf,int fd)
{

    ALARM_OUTPUT_DURATION_CFG_V1 *para_recv = (ALARM_OUTPUT_DURATION_CFG_V1 *)recv_buf;

    if (para_recv->byAlarmOutChan < 1 || para_recv->byAlarmOutChan> MAX_ALARM_INPUT_LINKAGE_OUTPUT_OBJECT) 
    {
        //���緢�� ��Ч����
        printf_debug("FUNC[%s] LINE[%d]\tInvalid arg alarm_linkage_output_object, it should be 1 to 8\n",__FUNCTION__, __LINE__);

    }
    else
    {
        alarm_input_output_data.alarm_linkage_output_object = para_recv->byAlarmOutChan;
        alarm_input_output_data.setup_command_set = ALARM_INPUT_OUTPUT_GET_ALARM_DURATION;
    }


    return 1;

}

/******************************************************
Fun: ��ȡ������������
para:   
timeout:
recv_buf, 
src_len��
 *********************************************************/
int get_alarm_linkage(char *recv_buf,int fd)
{
    ALARM_LINKAGE_PARA_SEND alarm_linkage_para_send;
    ALARM_LINKAGE_PARA *para_recv = (ALARM_LINKAGE_PARA *)recv_buf;
    int check_ret,len;

    memset(&alarm_linkage_para_send,0,sizeof(alarm_linkage_para_send));

    alarm_input_output_data.alarm_linkage_input_object = para_recv->byAlarmInType;
    alarm_linkage_para_send.alarm_linkage_para.byAlarmInType = alarm_input_output_data.alarm_linkage_input_object;
    alarm_linkage_para_send.alarm_linkage_para.byAlarmLinkOutChan = htonl(alarm_input_output_data.alarm_input_linkage_output_object[alarm_input_output_data.alarm_linkage_input_object-1]);

    check_ret=1; //checksum_send(&ret_cmd.sxdHeader,0);
    alarm_linkage_para_send.sxdHeader.checkSum=htonl(check_ret);
    alarm_linkage_para_send.sxdHeader.requestID = htonl(SXC_GET_ALARM_LINKAGE_CFG_V2);
    alarm_linkage_para_send.sxdHeader.status = htonl(QULIFIED); 						 //**************�˴��Ƿ�Ϊȷ������?
    alarm_linkage_para_send.sxdHeader.length = htonl(sizeof(ALARM_LINKAGE_PARA_SEND));
    len = sizeof(ALARM_LINKAGE_PARA_SEND);
    alarm_linkage_para_send.sxdHeader.version = htonl(SDK_VERSION);
    printf_debug("get the alarmlinkage successfully\n");

    return send_data_intime_over(fd, MAX_TIME_OUT_MS, (char *)&alarm_linkage_para_send, len);
}

/******************************************************
Fun: ����ʱ���������
para:   
timeout:
recv_buf, 
src_len��
 *********************************************************/
int alarm_para_set(char *recv_buf,int fd)
{
#ifdef DEBUG_DATA
    SXC_INTER_ALARMCTL_SET_PARAM_REQ *alarm_para = (SXC_INTER_ALARMCTL_SET_PARAM_REQ *)recv_buf;


    printf("byAlarmDelay:%d\n",alarm_para->byAlarmDelay);
    printf("byAlarmHostID:%d\n",alarm_para->byAlarmHostID);
    printf("byEnterDelay:%d\n",alarm_para->byEnterDelay);
    printf("byOutDelay:%d\n",alarm_para->byOutDelay);
#endif

    //return 1;
    return send_act_cmd(QULIFIED ,fd,SXC_ALARMCTL_DEV_SET_PARAM,fd);
}

/******************************************************
Fun:�ָ�����ʱ���������
para:   
timeout:
recv_buf, 
src_len��
 *********************************************************/
int alarmlinkage_output_time_cancle(char *recv_buf,int fd)
{
    if (alarm_input_output_data.real_time_alarm_output_objcet) 
    {
        //���緢����ȡ�����б��������Ȼ�������ñ���������Ӧ��ϵ
        send_act_cmd(NOSUPPORT,fd,SXC_RESET_ALARM_LINKAGE_AND_TIMEPARA,fd);
        printf_debug("FUNC[%s] LINE[%d]\tPlease cancel alarm output, then try it again!\n",__FUNCTION__, __LINE__);
    }
    else
    {
        alarm_input_output_data.setup_command_set = ALARM_INPUT_OUTPUT_RESTORE_TO_DEFAULT;
        alarm_input_output_data.current_fd = fd;
    }

    return 1;
}

/******************************************************
Fun:�������״̬��ѯ
para:   
timeout:
recv_buf, 
src_len��
 *********************************************************/
int alarm_out_state(char *recv_buf,int fd)
{
    SXC_INTER_ALARM_OUT_STATE send_buf;
    int check_ret;
    int len = sizeof(SXC_INTER_ALARM_OUT_STATE);

    memset(&send_buf,0,len);
    send_buf.alarm_state = htonl(alarm_input_output_data.real_time_alarm_output_objcet);
    printf("FUNC[%s] LINE[%d]\tAlarm output status: 0x%X\n",__FUNCTION__, __LINE__, alarm_input_output_data.real_time_alarm_output_objcet);
    check_ret=1; //checksum_send(&ret_cmd.sxdHeader,0);
    send_buf.sxdHeader.checkSum=htonl(check_ret);
    send_buf.sxdHeader.requestID = htonl(SXC_GETALAOUT_STATE);
    send_buf.sxdHeader.status = htonl(QULIFIED); 
    send_buf.sxdHeader.length = htonl(sizeof(SXC_INTER_ALARM_OUT_STATE));
    send_buf.sxdHeader.version = htonl(SDK_VERSION);

    return send_data_intime_over(fd, MAX_TIME_OUT_MS, (char *)&send_buf, len);
}

/******************************************************
Fun:�����������
para:   
timeout:
recv_buf, 
src_len��
 *********************************************************/
int alarm_out_ctrl(char *recv_buf,int fd)
{
    SXC_INTER_ALARM_OUT_CTRL *cmd = (SXC_INTER_ALARM_OUT_CTRL *)recv_buf;
    alarm_input_output_data.current_fd = fd;
    alarm_input_output_data.set_and_cancel_linkage_alarm_channel = ntohl(cmd->channel);
    alarm_input_output_data.setup_command_set = ALARM_INPUT_OUTPUT_SET_AND_CANCEL_LINKAGE_ALARM;

#ifdef DEBUG_DATA
    printf("alarm_out_ctrl:%d\n",alarm_input_output_data.real_time_alarm_output_objcet);
#endif

    return 1;
}

/******************************************************
Fun:���������������ȡ
para:   
timeout:
recv_buf, 
src_len��
 *********************************************************/
int switch_para_send(char *recv_buf,int fd)
{



    //ע���ֽ���
    return 1;
}

/******************************************************
Fun: �����������������
para:   
timeout:
recv_buf, 
src_len��
 *********************************************************/
int switch_para_set(char *recv_buf,int fd)
{
    //INTER_ALARMOUTCFG *switch_para = (INTER_ALARMOUTCFG *)recv_buf;


    //ע���ֽ���

    return send_act_cmd(QULIFIED ,fd,SXC_SET_ALARMOUTCFG,fd);
}

/******************************************************
Fun: �����������������
para:   
timeout:
recv_buf, 
src_len��
 *********************************************************/
int switch_in_para_set(char *recv_buf,int fd)
{


    return 1;

}

/******************************************************
Fun:���ڲ����ķ���
para:   
timeout:
recv_buf, 
src_len��
 *********************************************************/
int com_para_send(char *recv_buf,int fd)
{
    SERIAL_PAMATER_SEND send_buf;
    int check_ret,len;
    SERIAL_PAMATER *recv_para = (SERIAL_PAMATER *)recv_buf;
    //-------------------------------------------------------�˴������豸��  �豸���� �ʹ��ںŵ�ƥ��
    len = sizeof(SERIAL_PAMATER_SEND);
    memset(&send_buf,0,len);

    send_buf.com_para.byPathNo = recv_para->byPathNo;

    switch(recv_para->byPathNo)
    {
        case 0:
            {
                send_buf.com_para.serialAttr.byDataBit = entrance_guard_data.entrance_guard_serial_pamater.serialAttr.byDataBit - 5;
                send_buf.com_para.serialAttr.byFlowcontrol =entrance_guard_data.entrance_guard_serial_pamater.serialAttr.byFlowcontrol ;
                send_buf.com_para.serialAttr.byParity = entrance_guard_data.entrance_guard_serial_pamater.serialAttr.byParity;
                send_buf.com_para.serialAttr.byStopBit = entrance_guard_data.entrance_guard_serial_pamater.serialAttr.byStopBit - 1;

                //�˴�Ӧת��  ���ͻ���δת��   �˴�Ҳ��ת
                send_buf.com_para.serialAttr.dwBaudRate = com_dwBaudRate_get(entrance_guard_data.entrance_guard_serial_pamater.serialAttr.dwBaudRate);
                printf("dwBaudRate :%x\n",send_buf.com_para.serialAttr.dwBaudRate);
                send_buf.com_para.serialLink.byDevNo = 1;
                send_buf.com_para.serialLink.byDevProtocol = 1;
                send_buf.com_para.serialLink.byDevType = LINK_DEV_DOOR_CLT;
                //send_act_cmd(QULIFIED,fd,SXC_GET_CONNECT_PAMA,fd);

                break;
            }
        case 1:
            {
                send_buf.com_para.serialAttr.byDataBit =  matrix_control_data.matrix_control_serial_pamater.serialAttr.byDataBit - 5;
                send_buf.com_para.serialAttr.byFlowcontrol = matrix_control_data.matrix_control_serial_pamater.serialAttr.byFlowcontrol;
                send_buf.com_para.serialAttr.byParity = matrix_control_data.matrix_control_serial_pamater.serialAttr.byParity;
                send_buf.com_para.serialAttr.byStopBit = matrix_control_data.matrix_control_serial_pamater.serialAttr.byStopBit - 1;
                send_buf.com_para.serialAttr.dwBaudRate = com_dwBaudRate_get(matrix_control_data.matrix_control_serial_pamater.serialAttr.dwBaudRate);

                send_buf.com_para.serialLink.byDevNo = 1;
                send_buf.com_para.serialLink.byDevProtocol = 1;
                send_buf.com_para.serialLink.byDevType = LINK_DEV_MATRIX;
                //send_act_cmd(QULIFIED,fd,SXC_GET_CONNECT_PAMA,fd);
                break;
            }
        case 2:
            {
                send_buf.com_para.serialAttr.byDataBit =  cradle_head_control_data.cradle_head_control_serial_pamater.serialAttr.byDataBit - 5;
                send_buf.com_para.serialAttr.byFlowcontrol = cradle_head_control_data.cradle_head_control_serial_pamater.serialAttr.byFlowcontrol;
                send_buf.com_para.serialAttr.byParity = cradle_head_control_data.cradle_head_control_serial_pamater.serialAttr.byParity;
                send_buf.com_para.serialAttr.byStopBit = cradle_head_control_data.cradle_head_control_serial_pamater.serialAttr.byStopBit - 1;
                send_buf.com_para.serialAttr.dwBaudRate = com_dwBaudRate_get(cradle_head_control_data.cradle_head_control_serial_pamater.serialAttr.dwBaudRate);

                send_buf.com_para.serialLink.byDevNo = 1;
                send_buf.com_para.serialLink.byDevProtocol = 1;
                send_buf.com_para.serialLink.byDevType = LINK_DEV_CRADLE_HEAD;
                //send_act_cmd(QULIFIED,fd,SXC_GET_CONNECT_PAMA,fd);
                break;
            }
        case 5:
            {
                send_buf.com_para.serialAttr.byDataBit =  ck2316_alarm_data.ck2316_alarm_serial_pamater.serialAttr.byDataBit - 5;
                send_buf.com_para.serialAttr.byFlowcontrol = ck2316_alarm_data.ck2316_alarm_serial_pamater.serialAttr.byFlowcontrol;
                send_buf.com_para.serialAttr.byParity = ck2316_alarm_data.ck2316_alarm_serial_pamater.serialAttr.byParity;
                send_buf.com_para.serialAttr.byStopBit = ck2316_alarm_data.ck2316_alarm_serial_pamater.serialAttr.byStopBit - 1;
                send_buf.com_para.serialAttr.dwBaudRate = com_dwBaudRate_get(ck2316_alarm_data.ck2316_alarm_serial_pamater.serialAttr.dwBaudRate);

                send_buf.com_para.serialLink.byDevNo = 1;
                send_buf.com_para.serialLink.byDevProtocol = 1;
                send_buf.com_para.serialLink.byDevType = LINK_DEV_ALARM_CTL;
                //send_act_cmd(QULIFIED,fd,SXC_GET_CONNECT_PAMA,fd);
                break;
            }
        default:
            {
                send_act_cmd(NOSUPPORT,fd,SXC_GET_CONNECT_PAMA,fd);
                break;
            }
    }
    //send_buf.com_para.serialLink.byDevNo = recv_para->serialLink.byDevNo;
    //send_buf.com_para.serialLink.byDevProtocol = recv_para->serialLink.byDevProtocol;
    //send_buf.com_para.serialLink.byDevType = recv_para->serialLink.byDevType;

    check_ret=1; //checksum_send(&ret_cmd.sxdHeader,0);
    send_buf.sxdHeader.version = htonl(SDK_VERSION);
    send_buf.sxdHeader.checkSum=htonl(check_ret);
    send_buf.sxdHeader.requestID = htonl(SXC_GET_CONNECT_PAMA);
    send_buf.sxdHeader.status = htonl(QULIFIED); 
    send_buf.sxdHeader.length = htonl(sizeof(SERIAL_PAMATER_SEND));

#ifdef DEBUG_DATA
    printf("pathNO:%d---devNO:%d---devtype:%d\n",send_buf.com_para.byPathNo,send_buf.com_para.serialLink.byDevNo,send_buf.com_para.serialLink.byDevType);
#endif

    return send_data_intime_over(fd, MAX_TIME_OUT_MS, (char *)&send_buf, len);

}

/******************************************************
Fun:���ڲ���������
para:   
timeout:
recv_buf, 
src_len��
 *********************************************************/
int com_para_set(char *recv_buf,int fd)
{
    SERIAL_PAMATER *recv_para = (SERIAL_PAMATER *)recv_buf;
    //-------------------------------------------------------�˴�Ҫ�����豸��  �豸���� �ʹ��ںŵ�ƥ��

    //�����ӿڵĸ�ֵ
#ifdef DEBUG_DATA
    printf("byPathNo:%d\n",recv_para->byPathNo);
    printf("dwBaudRate:%d\n",ntohl(recv_para->serialAttr.dwBaudRate));
    printf("byStopBit:%d\n",recv_para->serialAttr.byStopBit);
    printf("byParity:%d\n",recv_para->serialAttr.byParity);
    printf("byFlowcontrol:%d\n",recv_para->serialAttr.byFlowcontrol);
    printf("byDataBit:%d\n",recv_para->serialAttr.byDataBit);
    printf("byDevNo:%d\n",recv_para->serialLink.byDevNo);
    printf("byDevProtocol:%d\n",recv_para->serialLink.byDevProtocol);
    printf("byDevType:%d\n",recv_para->serialLink.byDevType);
#endif

    //У�鴮�������Ƿ�Ϸ�
    //����һ�������Ž�������
    switch(recv_para->byPathNo)
    {
        case 0:
            {

                if(LINK_DEV_DOOR_CLT == recv_para->serialLink.byDevType)
                {
                    entrance_guard_data.entrance_guard_serial_pamater.byPathNo = recv_para->byPathNo;
                    entrance_guard_data.entrance_guard_serial_pamater.serialLink.byDevNo = recv_para->serialLink.byDevNo;
                    entrance_guard_data.entrance_guard_serial_pamater.serialLink.byDevProtocol = recv_para->serialLink.byDevProtocol;
                    entrance_guard_data.entrance_guard_serial_pamater.serialLink.byDevType = recv_para->serialLink.byDevType;
                    entrance_guard_data.entrance_guard_serial_pamater.serialAttr.byDataBit = recv_para->serialAttr.byDataBit +5;
                    entrance_guard_data.entrance_guard_serial_pamater.serialAttr.byFlowcontrol = recv_para->serialAttr.byFlowcontrol;
                    entrance_guard_data.entrance_guard_serial_pamater.serialAttr.byParity = recv_para->serialAttr.byParity;
                    entrance_guard_data.entrance_guard_serial_pamater.serialAttr.byStopBit = recv_para->serialAttr.byStopBit +1;

                    entrance_guard_data.entrance_guard_serial_pamater.serialAttr.dwBaudRate = com_baudrate_set(ntohl(recv_para->serialAttr.dwBaudRate));

                    set_serial_dev_parameter(fp_dev_config_file);
                    send_act_cmd(QULIFIED,fd,SXC_SET_CONNECT_PAMA,fd);

                }
                else
                {
                    send_act_cmd(NOSUPPORT,fd,SXC_SET_CONNECT_PAMA,fd);
                }
                break;
            }
        case 1:
            {
                if(LINK_DEV_MATRIX == recv_para->serialLink.byDevType)
                {
                    matrix_control_data.matrix_control_serial_pamater.byPathNo = recv_para->byPathNo;
                    matrix_control_data.matrix_control_serial_pamater.serialLink.byDevNo = recv_para->serialLink.byDevNo;
                    matrix_control_data.matrix_control_serial_pamater.serialLink.byDevProtocol = recv_para->serialLink.byDevProtocol;
                    matrix_control_data.matrix_control_serial_pamater.serialLink.byDevType = recv_para->serialLink.byDevType;

                    matrix_control_data.matrix_control_serial_pamater.serialAttr.byDataBit = recv_para->serialAttr.byDataBit+5;
                    matrix_control_data.matrix_control_serial_pamater.serialAttr.byFlowcontrol = recv_para->serialAttr.byFlowcontrol;
                    matrix_control_data.matrix_control_serial_pamater.serialAttr.byParity = recv_para->serialAttr.byParity;
                    matrix_control_data.matrix_control_serial_pamater.serialAttr.byStopBit = recv_para->serialAttr.byStopBit+1;

                    printf("byDataBit::::%x\n",cradle_head_control_data.cradle_head_control_serial_pamater.serialAttr.byDataBit);
                    printf("byDataBit::::%x\n",recv_para->serialAttr.byDataBit);

                    matrix_control_data.matrix_control_serial_pamater.serialAttr.dwBaudRate = com_baudrate_set(ntohl(recv_para->serialAttr.dwBaudRate));

                    set_serial_dev_parameter(fp_dev_config_file);
                    send_act_cmd(QULIFIED,fd,SXC_SET_CONNECT_PAMA,fd);
                }
                else
                {
                    send_act_cmd(NOSUPPORT,fd,SXC_SET_CONNECT_PAMA,fd);
                }
                break;
            }
        case 2:
            {
                if(LINK_DEV_CRADLE_HEAD == recv_para->serialLink.byDevType)
                {
                    cradle_head_control_data.cradle_head_control_serial_pamater.byPathNo = recv_para->byPathNo;
                    cradle_head_control_data.cradle_head_control_serial_pamater.serialLink.byDevNo = recv_para->serialLink.byDevNo;
                    cradle_head_control_data.cradle_head_control_serial_pamater.serialLink.byDevProtocol = recv_para->serialLink.byDevProtocol;
                    cradle_head_control_data.cradle_head_control_serial_pamater.serialLink.byDevType = recv_para->serialLink.byDevType;

                    cradle_head_control_data.cradle_head_control_serial_pamater.serialAttr.byDataBit = recv_para->serialAttr.byDataBit+5;



                    cradle_head_control_data.cradle_head_control_serial_pamater.serialAttr.byFlowcontrol = recv_para->serialAttr.byFlowcontrol;
                    cradle_head_control_data.cradle_head_control_serial_pamater.serialAttr.byParity = recv_para->serialAttr.byParity;
                    cradle_head_control_data.cradle_head_control_serial_pamater.serialAttr.byStopBit = recv_para->serialAttr.byStopBit +1;

                    cradle_head_control_data.cradle_head_control_serial_pamater.serialAttr.dwBaudRate = com_baudrate_set(ntohl(recv_para->serialAttr.dwBaudRate));

                    set_serial_dev_parameter(fp_dev_config_file);
                    send_act_cmd(QULIFIED,fd,SXC_SET_CONNECT_PAMA,fd);
                }
                else
                {
                    send_act_cmd(NOSUPPORT,fd,SXC_SET_CONNECT_PAMA,fd);
                }
                break;
            }
        case 4:
            {
                if(LINK_DEV_ALARM_CTL == recv_para->serialLink.byDevType)
                {
                    ck2316_alarm_data.ck2316_alarm_serial_pamater.byPathNo = recv_para->byPathNo;
                    ck2316_alarm_data.ck2316_alarm_serial_pamater.serialLink.byDevNo = recv_para->serialLink.byDevNo;
                    ck2316_alarm_data.ck2316_alarm_serial_pamater.serialLink.byDevProtocol = recv_para->serialLink.byDevProtocol;
                    ck2316_alarm_data.ck2316_alarm_serial_pamater.serialLink.byDevType = recv_para->serialLink.byDevType;

                    ck2316_alarm_data.ck2316_alarm_serial_pamater.serialAttr.byDataBit = recv_para->serialAttr.byDataBit+5;
                    ck2316_alarm_data.ck2316_alarm_serial_pamater.serialAttr.byFlowcontrol = recv_para->serialAttr.byFlowcontrol;
                    ck2316_alarm_data.ck2316_alarm_serial_pamater.serialAttr.byParity = recv_para->serialAttr.byParity;
                    ck2316_alarm_data.ck2316_alarm_serial_pamater.serialAttr.byStopBit = recv_para->serialAttr.byStopBit+1;

                    ck2316_alarm_data.ck2316_alarm_serial_pamater.serialAttr.dwBaudRate = com_baudrate_set(ntohl(recv_para->serialAttr.dwBaudRate));

                    set_serial_dev_parameter(fp_dev_config_file);
                    send_act_cmd(QULIFIED,fd,SXC_SET_CONNECT_PAMA,fd);
                }
                else
                {
                    send_act_cmd(NOSUPPORT,fd,SXC_SET_CONNECT_PAMA,fd);
                }
                break;
            }
        default:
            {
                send_act_cmd(NOSUPPORT,fd,SXC_SET_CONNECT_PAMA,fd);
                break;
            }


    }
    return 1;
    //return send_act_cmd(QULIFIED ,fd,SXC_SET_CONNECT_PAMA,fd);
}

/******************************************************
Fun: �����ֽ���ת��Ϊ������
para:   
timeout:
recv_buf, 
src_len��
 *********************************************************/
int channel_num_track(void)
{
    pthread_mutex_lock(&mutex_channel);
    channel_num_passive++;
    pthread_mutex_unlock(&mutex_channel);
    return channel_num_passive;
}

/*******************************************************************************
 * ����:             ���ͷ���ָ��
 * ������:          
 * ����:      
 * ���ݷ���: 
 * ���ݸ���:  
 * ����:          
 * ���:         
 * ����ֵ:         0:�ɹ���--����ʧ��
 *******************************************************************************/
int send_act_cmd(int status, int client_fd, int request_id, int usr_id)
{
    INTER_SXD_LOGIN_RET    ret_cmd;//sx+usrId
    int len;
    int check_ret=0;

    memset(&ret_cmd, 0, sizeof(INTER_SXD_HEAD));
    ret_cmd.sxdHeader.requestID = htonl(request_id);
    ret_cmd.sxdHeader.status = htonl(status);

    check_ret=1; //checksum_send(&ret_cmd.sxdHeader,0);
    ret_cmd.sxdHeader.checkSum=htonl(check_ret);

    if (status == QULIFIED&&(request_id == SXC_LOG_IN))
    {
        printf_debug("login reply\n");
        ret_cmd.sxdHeader.version = htonl(SDK_VERSION);
        ret_cmd.sxdHeader.requestID = htonl(SXC_LOG_IN);
        ret_cmd.sxdHeader.status = htonl(QULIFIED);  //sx
        ret_cmd.sxdHeader.length = htonl(sizeof(INTER_SXD_LOGIN_RET));            
        ret_cmd.usrID = htonl(usr_id);                      //usr_id
        ret_cmd.dvsInfo.pathNO = htonl(0);
        ret_cmd.dvsInfo.pathSum = htonl(channel_num_passive);
        ret_cmd.dvsInfo.alarmInNum = htonl(2);
        ret_cmd.dvsInfo.alarmOutNum = htonl(2);
#ifdef __1502AV_         
        ret_cmd.dvsInfo.deviceType = htonl(DEVICE_AV);    
#endif

#ifdef __1502DD_    
        ret_cmd.dvsInfo.deviceType = htonl(DEVICE_TS2303_DVR);    
#endif

#ifdef __TS2302HIAL_         
        ret_cmd.dvsInfo.deviceType = htonl(DEVICE_TS2302HIAL_DVR);   
#endif


        len = sizeof(INTER_SXD_LOGIN_RET);

    }
    else
    {
        len = sizeof(INTER_SXD_HEAD);
        ret_cmd.sxdHeader.length = htonl(sizeof(INTER_SXD_HEAD));     
    }
    return send_data_intime_over(client_fd, MAX_TIME_OUT_MS, (char *)&ret_cmd, len);



}

/***********************************************************************************
Fun:  �ڹ涨��ʱ���ڽ�recv_buf��ʼ�ģ�src_len���ȵ����ݷ���client_fd
para:   client_fd ���յ�socket
timeout:��ʱʱ�䣬��λΪ����[ms]
recv_buf,���ջ����� 
src_len�����յĳ���

ps:    This fun is diff from fun send_data_intime , because when net is busy, Use poll to probe untile
net is not busy

����:�÷��ͺ�����������poll�����Լ��ź��жϺ��������ܣ�
�ú���ֻ��socket�쳣�������ݷ������ʱ�ŷ���
 **************************************************************************************/
int  send_data_intime_over(int client_fd, int timeout, char *src_buf, int src_len)
{
    int res;
    int send_len, send_bytes;
    char *send_buf;
    int poll_timeout;
    struct pollfd poll_set[1];
    int try_times = 0;


    if (src_buf == NULL || src_len <= 0 )
    {
        err_print("send_buf:%p, src_len:%d...\n", send_buf, src_len);
        return -1;
    } 

    poll_set[0].fd = client_fd;
    poll_set[0].events = POLLOUT;// ���Խ���
    poll_timeout = timeout; //
    send_buf = src_buf;
    send_len = src_len;

    errno = 0;

    //probe
POLL_ANAIN:
    while ( (-1 == (res = poll(poll_set, 1, poll_timeout))) && (errno == EAGAIN || errno == EINTR ) );
    if (res == -1)
    {

        return -1;
    }
    else if (res == 0)//timeout
    {
        printf_debug("time out\n");
        return -1;
    }
    while(send_len>0)
    {
        send_bytes = send(client_fd, send_buf, send_len, MSG_NOSIGNAL);
        if (send_bytes == -1)
        {
            if (errno == EAGAIN || errno == EINTR || errno == EWOULDBLOCK)//busy or bolocked or be interruped
            {
                send_bytes = 0;
                try_times++;
                if (try_times>SOCKET_BUSY_TRY_TIMES)
                {
                    err_print("Clent:%d Send Try More than %d times-Poll Passed but send busy\n", client_fd, SOCKET_BUSY_TRY_TIMES);     
                    return -1;
                }
                errno = 0;                
                goto POLL_ANAIN;
            }
            else
            {
                //err_print("client_fd:%d send err:%d:%s\n", client_fd, errno, strerror(errno));      
                return -1;
            }

        }
        send_buf += send_bytes;
        send_len -= send_bytes;        

    }
    // printf_debug("send msg successful!!\n");

    return 1;
}

/*********************************************************************
  fun name    :ÿ����һ�����ӵ�ʱ��洢�û���Ϣ
func        :    û���һ���û��ڵ��ʱ�����û�������֤���ݵ�������
para:   
ret         : 
date        : 2012-08-15
auto        : 
 *********************************************************************/

user_info *link_insert(int fd)
{
    user_info *ph;

    pthread_mutex_lock(&mutex_link);

    if_have_net_client_connect = YES;

    //����ռ�  ��ʼ���ڵ�  	 
    ph=(user_info *)calloc(1,sizeof(user_info));
    ph->user_id = fd;
    ph->sockfd_client = fd;
#ifdef DEBUG_DATA
    printf("userid:%d--sockfd:%d\n",ph->user_id,ph->sockfd_client);
#endif
    // strcpy(ph->server_mac,mac);
    if((channel_num_passive+channe_num_active)==1)
    {
        head=ph;
        pf = ph;
    }
    else
    {
        pf->next = ph;

        pf = ph;
    }
    ph->next = NULL;

    pthread_mutex_unlock(&mutex_link);

    return head;
}

//=============================================================
// �������ƣ�link_print
// �����������������
//=============================================================
void link_print(user_info *head)
{
    user_info * pb=head;
    if(pb ==NULL)
    {
        printf_debug("no user on-line\n");
    }
    while(pb!=NULL)
    {
        printf_debug("userid=%d---clientfd=%d\n",pb->user_id,pb->sockfd_client);  // �������
        pb=pb->next;
    }
}

//=============================================================
// �������ƣ�link_delete(head,name_other)
// �������������û�����ʱ ���û�������ɾ���û�
//=============================================================
user_info *link_delete(int fd)
{
    user_info *p1,*p2;

    pthread_mutex_lock(&user_outline);

    p1 = p2 =head;
    if(head == NULL)
    {
        printf("link NULL");
        return head;
    }
    while ((p1->sockfd_client != fd)&&(p1->next!=NULL))
    {
        p2=p1;
        p1=p1->next;
    }
    if(p1->sockfd_client == fd)
    {
        if (p1 == head)
        {
            head = head->next;                   
        }
        if (p1 == NULL)
        {
            p2->next = NULL;
        }
        else
        {
            p2->next = p1->next;
        }
    }
    if(head != NULL)
    {
        if_have_net_client_connect = YES;
    }
    else
    {
        if_have_net_client_connect = NO;
    }
    pthread_mutex_unlock(&user_outline);
    return head;
}	

/*********************************************************************
  fun name    :ÿ��ʮ�뷢��˫�����ְ�
func        :    
para:   
ret         : 
date        : 2012-08-20
auto        : 
 *********************************************************************/
void *pthread_handshake(void *arg)
{
    while(1)
    {
        user_info *p_hand_shake = head;
        //user_info *p_hand_shake_do =head;

        sleep(10);
        //printf_debug("10s sleepping over,continue to do handshake\n");
        while(p_hand_shake != NULL)
        {
            if(send_act_cmd(0,p_hand_shake->sockfd_client,SXC_KEEPLIVE_B,p_hand_shake->sockfd_client) == 1)
            {
                //printf_debug("handshake success!!!\n");
                p_hand_shake = p_hand_shake->next;
            }
            /*
               else
               {
               printf_debug("handshake fail\n");
               link_delete(p_hand_shake_do->sockfd_client);//��Ӧ��ͨ���ر�ʱά�������û���
               }
               p_hand_shake_do = p_hand_shake;
               */
        }
    }     
    return NULL;
}

/*******************************************************************************
 * ����:        ��������Զ������, ʱ�����̶�30s���������ʧ�ܣ�����-1�����򷵻�
 ���ӵ�fd

 * ������:          
 * ����:      
 * ���ݷ���: 
 * ���ݸ���:  
 * ����:          
 * ���:         
 * ����ֵ:          0:�ɹ���--����ʧ��
 *******************************************************************************/

int api_connect_remote_client(int ip, short port, short time_out)
{
    int fd = -1;
    int socket_flags;
    if (-1 == (fd = socket(AF_INET, SOCK_STREAM, 0)))
    {
        //err_print("create socket failed-errno:%d:%s\n", errno, strerror(errno));
        printf_debug("fd = -1\n");
        goto DEAL_FAIL;

    }

    if ((socket_flags = fcntl(fd, F_GETFL, 0)) < 0) 
    {        
        // err_print("fcntl failed-errno:%d:%s\n", errno, strerror(errno));
        printf_debug("socket_flags < 0\n");
        goto DEAL_FAIL;
    } 

    //�����׽���Ϊ������ 
    if (fcntl(fd, F_SETFL, socket_flags | O_NONBLOCK) < 0) 
    { 
        printf_debug("fcntl < 0\n");
        //err_print("fcntl failed-errno:%d:%s\n", errno, strerror(errno));       
        goto DEAL_FAIL;
    } 

    if (0 != net_connect(fd, ip,  port, time_out))
    {
        printf_debug("connect fail\n");
        goto DEAL_FAIL;
    }
    return fd;

DEAL_FAIL:
    if (fd!=-1)
    {
        printf_debug("close the active_sockfd\n");
        cycle_close_socket(fd);
    }

    return -1;

}    

/*******************************************************************************
 * ����:             �������ӹ���, ��һ��TCP���׽��֣�����IPΪdst_ip���˿�Ϊdst_port
 ��Զ������������ܹ����̵�ʱ��Ϊtime_out,��λΪ��
 * ������:          
 * ����:      
 * ���ݷ���:  time_out--��λ��
 * ���ݸ���:  
 * ����:          
 * ���:         
 * ����ֵ:          0:�ɹ���--����ʧ��
 *******************************************************************************/

int net_connect(int client_id ,int dst_ip,short dst_port, short time_out)
{
    int len, err, ret, try_times = 0,num;
    char str[20] = "";
    struct sockaddr_in  client_addr;
    bzero (&(client_addr), sizeof(client_addr));
    client_addr.sin_family = AF_INET;
    client_addr.sin_port = htons(dst_port);//Word����,����tcpЭ�������short����
    client_addr.sin_addr.s_addr = htonl(dst_ip);

#if 1  
    num = htonl(dst_ip);
    inet_ntop(AF_INET,&num,str,INET_ADDRSTRLEN); 
#endif

    //client_addr.sin_addr.s_addr = htonl(dst_ip);     //host sequence
    do
    {
        errno = 0;
        if (-1 ==(connect(client_id, (struct sockaddr *) &client_addr,sizeof (struct sockaddr))))
        {
            if ((errno == EINTR) || (errno == EAGAIN))//this is errnno, because progress is already in progress state
            {
                printf_debug("test1 \n");
                try_times++;
                if (try_times>=4)
                {
                    //err_print("try_times...\n");
                    return -1;
                }

                usleep(10*1000);
                continue;
            }

            if (errno == EINPROGRESS)//no is in progress
            {

                ret = poll_wtime(client_id, 1000 * time_out);                
                if (!ret) //timeout
                {
                    printf_debug("test2 \n");
                    //err_print("timeout.........\n");
                    return -1;
                }

                //poll--success
                len = sizeof(int);
                getsockopt(client_id, SOL_SOCKET, SO_ERROR,&err,(socklen_t *)&len);
                if (err == 0) 
                {
                    return 0;
                }
                else//connect fail
                {
                    if (err == ECONNREFUSED) //��������У�11,3���Կ���������û�п��������˿ڣ��򱻾ܾ�
                    {
                        //��������£�connect����������--ǿ������
                        sleep(time_out);	
                    }
                    else //113--·�ɲ��ɴ��
                    {
                        //err_print("connect-fail.-err:%d:%s........\n", err, strerror(err)); 
                    }
                    return -1;
                }

            }

            else //·�ɲ��ɴ����������У�1,44�Ļ��������ڣ���connect�������ش˴��󣬹�ǿ������
            {
                //err_print("connect fail,errno:%d:%s....\n", errno, strerror(errno));
                if ( (ENETUNREACH != errno) && (EHOSTUNREACH != errno) && (101 != errno) )//no route or unreachable
                {
                    //err_print("connect fail,errno:%d:%s....\n", errno, strerror(errno));
                }

                //��������£�connect����������--ǿ������
                sleep(time_out);
                return -1;
            }

            break;
        }
        else
        {
            printf_debug("connect success imediately.........\n");            
            return 1;
        }

    }while(1);


    return 0;
}

int cycle_close_socket(int fd)
{
    int err_retry_times = 0;
    do
    {
        if ((-1 == close(fd)) && (errno == EINTR))
        {
            //err_print("close fail-errno:%d:%s....\n", errno, strerror(errno));
            sleep(1);
            err_retry_times++;
        }
        else
        {
            break;
        }
    }while(err_retry_times<4);

    return 0;
}

/******************************************************
Fun:  �ڹ涨��ʱ����̽��fd�Ƿ��д����λms
para:  
 *********************************************************/
int poll_wtime(int client_fd, int timeout)
{
    int res;
    int poll_timeout;
    struct pollfd poll_set[1];


    poll_set[0].fd = client_fd;
    poll_set[0].events = POLLOUT;// ���Է���
    poll_timeout = timeout; //

    errno = 0;

    while ( (-1 == (res = poll(poll_set, 1, poll_timeout))) && (errno == EINTR));

    if (res == -1)
    {
        return -1;
    }

    else if (res == 0)//timeout
    {
        return -1;
    }

    return 1;

}

/******************************************************
Fun:  �ж�ip�Ƿ�Ϊ�����ε�
para:  


result:1 ��ʾ��ͬһ����   -1 ��ʾ����ͬһ����
 *********************************************************/
int ip_judge(int ip_maste,int device_ip,int client_ip)
{
    int device_netaddr = 0,client_netaddr = 0;
    device_netaddr = ip_maste & device_ip;
    client_netaddr = ip_maste * client_ip;
    if(device_netaddr != client_netaddr)
    {
        err_print("client & server is't int the same Segment\n ");
        return -1;
    }

    return 1;
}

/*******************************************************************************
 * ����:              ���ñ���Ĭ������
 * ������:          
 * ����:      
 * ���ݷ���: 
 * ���ݸ���:  
 * ����:          gateway_value-->����ֵ: ������
 * ���:         
 * ����ֵ:         
 *******************************************************************************/
int  set_local_lan_gateway(char *gateway_cmd)
{
    int ret = 0;

    printf_debug("set_local_lan_gateway_cmd is :%s\n",gateway_cmd);
    ret = system(gateway_cmd);
    if(-1 == ret )
    {
        err_print("%s-%d--gateway_set Fail:%d:%s....\n", __FILE__, __LINE__, errno, strerror(errno));
        return -1;
    }
    else 
    {
        if (WIFEXITED(ret))
        {  
            if (0 == WEXITSTATUS(ret))
            {  
                printf_debug("run shell script successfully.\n");
            }  
            else
            {  
                printf_debug("run shell script fail, script exit code: %d,ret=%d\n", WEXITSTATUS(ret),ret);
                return -1;
            }  
        }  
        else
        {  
            printf_debug("exit status = [%d]\n", WEXITSTATUS(ret));
            return -1;
        }  
    }  

    return 0;
}

/*******************************************************************************
 * ����:        
 * ������:          
 * ����:      
 * ���ݷ���: 
 * ���ݸ���:  
 * ����:          
 * ���:         
 * ����ֵ:         
 *******************************************************************************/
void get_local_lan_mac(unsigned char *p_mac)
{
    register int fd; 
    struct ifreq freq_if; 

    if (NULL == p_mac)
    {
        err_print("mac is null.....\n");
        return ;
    }

    memset(p_mac, 0, 6);

    if ((fd = socket (AF_INET, SOCK_DGRAM, 0)) >= 0) 
    { 
        //err_print("socket success...\n");
        memset(freq_if.ifr_name, 0, sizeof(IFNAMSIZ));
        memcpy(freq_if.ifr_name, "eth0",  strlen("eth0") + 1); //len must be strlen + 1 , if not it will return err:no such device

        if (!(ioctl (fd, SIOCGIFHWADDR, (char *) &freq_if))) 
        { 
            memcpy(p_mac, (char *)freq_if.ifr_hwaddr.sa_data, 6);
        } 
        else 
        {
            err_print("%s-%d--ioctl Fail:%d:%s....\n", __FILE__, __LINE__, errno, strerror(errno));
        }
        errno = 0;

        cycle_close_socket(fd);

    }
    else 
    {
        err_print("%s-%d--Creat socket Fail:%d:%s....\n", __FILE__, __LINE__, errno, strerror(errno));
    }

    return;
}

/*******************************************************************************
 * ����:       ��������
 * ������:          
 * ����:      
 * ���ݷ���: 
 * ���ݸ���:  
 * ����:          
 * ���:         
 * ����ֵ:         
 *******************************************************************************/
int set_lan_netmask(unsigned netmask)
{ 
    int			fd;
    struct ifreq		ifr;
    struct sockaddr_in	*sin;


    if ((fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        // err_print("%s-%d--Creat socket Fail:%d:%s....\n", __FILE__, __LINE__, errno, strerror(errno));
        return -1;
    }

    memset(&ifr, 0, sizeof(ifr));
    memcpy(ifr.ifr_name, "eth0",  strlen("eth0"));
    sin = (struct sockaddr_in *)&ifr.ifr_addr;

    sin->sin_family = AF_INET;
    sin->sin_addr.s_addr = htonl(netmask);

    if (ioctl(fd, SIOCSIFNETMASK, &ifr) < 0)
    {
        err_print("%s-%d--ioctl Fail:%d:%s....\n", __FILE__, __LINE__, errno, strerror(errno));
    }

    errno = 0;

    cycle_close_socket(fd);

    return 0;

} 

/*******************************************************************************
 * ����:            ����mac��ַ
 * ������:          
 * ����:      
 * ���ݷ���: 
 * ���ݸ���:  
 * ����:          
 * ���:         
 * ����ֵ:        0--��ʾ�ɹ���-1��ʾʧ�� 
 *******************************************************************************/
int set_local_lan_mac(unsigned char *p_mac)
{
    register int fd; 
    struct ifreq freq_if; 
    int ret = -1;

    if (NULL == p_mac)
    {
        err_print("mac is null.....\n");
        return ret;
    }

    //memset(p_mac, 0, 6);

    if ((fd = socket (AF_INET, SOCK_DGRAM, 0)) >= 0) 
    {       
        memset(freq_if.ifr_name, 0, sizeof(IFNAMSIZ));
        memcpy(freq_if.ifr_name, "eth0",  strlen("eth0") + 1); //len must be strlen + 1 , if not it will return err:no such device

        freq_if.ifr_hwaddr.sa_family = ARPHRD_ETHER;


        memcpy((char *)freq_if.ifr_hwaddr.sa_data, p_mac,  6); 

        if (!(ioctl (fd, SIOCSIFHWADDR, (char *) &freq_if))) 
        { 
            ret = 0;
        } 
        else 
        {
            err_print("%s-%d--ioctl Fail:%d:%s....\n", __FILE__, __LINE__, errno, strerror(errno));
        }
        errno = 0;

        cycle_close_socket(fd);

    }
    else 
    {
        err_print("%s-%d--Creat socket Fail:%d:%s....\n", __FILE__, __LINE__, errno, strerror(errno));
    }

    return ret;
}

/*******************************************************************************
 * ����:            ��ȡ�豸����
 * ������:          
 * ����:      
 * ���ݷ���: 
 * ���ݸ���:  
 * ����:          
 * ���:         
 * ����ֵ:        1--��ʾ�ɹ���-1��ʾʧ�� 
 *******************************************************************************/
int dev_para_get(char *recv_buf,int fd)
{
    SX_DEVICE_WORKING_STATUS_SEND send_buf;
    int check_ret;

    memset(&send_buf,0,sizeof(SX_DEVICE_WORKING_STATUS_SEND));
    send_buf.para.byDevID = dev_para.byDevID;
    memcpy(send_buf.para.byDevSerialNum,dev_para.byDevSerialNum,sizeof(dev_para.byDevSerialNum));
    send_buf.para.bySoftVer = dev_para.bySoftVer;
    send_buf.para.byHardVer = dev_para.byHardVer;
    send_buf.para.byCommProVer = dev_para.byCommProVer;
    send_buf.para.bySerialNum = dev_para.bySerialNum;

    check_ret =1;
    send_buf.sxdHeader.version = htonl(SDK_VERSION);
    send_buf.sxdHeader.checkSum=htonl(check_ret);
    send_buf.sxdHeader.requestID = htonl(SXC_GET_ICC_DEV_PARAM);
    send_buf.sxdHeader.status = htonl(QULIFIED); 
    send_buf.sxdHeader.length = htonl(sizeof(SX_DEVICE_WORKING_STATUS_SEND));

    return send_data_intime_over(fd, MAX_TIME_OUT_MS, (char *)&send_buf, sizeof(SX_DEVICE_WORKING_STATUS_SEND));
}

/*******************************************************************************
 * ����:       �豸����
 * ������:          
 * ����:      
 * ���ݷ���: 
 * ���ݸ���:  
 * ����:          
 * ���:         
 * ����ֵ:         
 *******************************************************************************/
int dev_restart(int fd)
{
    char *cmd_sys = "reboot";
    int ret = 0;
    ret = system(cmd_sys);
    if(-1 == ret )
    {
        err_print("%s-%d--restart Fail:%d:%s....\n", __FILE__, __LINE__, errno, strerror(errno));
        return -1;
    }
    else 
    {
        if (WIFEXITED(ret))
        {  
            if (0 == WEXITSTATUS(ret))
            {  
                printf_debug("run shell script successfully.\n");
                send_act_cmd(QULIFIED,fd,SXC_REBOOT,fd);
            }  
            else
            {  
                printf_debug("run shell script fail, script exit code: %d,ret=%d\n", WEXITSTATUS(ret),ret);
                return -1;
            }  
        }  
        else
        {  
            printf_debug("exit status = [%d]\n", WEXITSTATUS(ret));
            return -1;
        }  
    }  

    return 1;
}

/*******************************************************************************
 * ����:      ϵͳУʱ
 * ������:          
 * ����:      
 * ���ݷ���: 
 * ���ݸ���:  
 * ����:          
 * ���:         
 * ����ֵ:         
 *******************************************************************************/
int system_time_set(char *recv_buf,int fd)
{
    char cmd_buf[50] = {0};
    int ret = 0;
    INTER_TIME *time_para = (INTER_TIME *)recv_buf;

    sprintf(cmd_buf,"%s%02d%02d%02d%02d%s%02d%c%02d","date -s ",time_para->Month,time_para->Day,time_para->Hour,time_para->Minute,"20",time_para->Year,'.',time_para->Second);
    ret = system(cmd_buf);
    if(-1 == ret )
    {
        err_print("%s-%d--time set:%d:%s....\n", __FILE__, __LINE__, errno, strerror(errno));
        return -1;
    }
    else 
    {
        if (WIFEXITED(ret))
        {  
            if (0 == WEXITSTATUS(ret))
            {  
                printf_debug("run shell script successfully.\n");
                send_act_cmd(QULIFIED,fd,SXC_SET_TIMECFG,fd);
            }  
            else
            {  
                printf_debug("run shell script fail, script exit code: %d,ret=%d\n", WEXITSTATUS(ret),ret);
                return -1;
            }  
        }  
        else
        {  
            printf_debug("exit status = [%d]\n", WEXITSTATUS(ret));
            return -1;
        }  
    }  

    //printf_debug("time:%s\n",cmd_buf);
    return 1;
}

/*******************************************************************************
 * ����:      �ָ�Ĭ�ϲ���
 * ������:          
 * ����:      
 * ���ݷ���: 
 * ���ݸ���:  
 * ����:          
 * ���:         
 * ����ֵ:         
 *******************************************************************************/
int acquiescent_para_recovery(int fd)
{
    int gatewayip;
    char gateway_buf[16] = {0};

    char gateway[50] =  "route add default gw  ";

    inet_pton(AF_INET,"192.168.11.222",&para_net.dwDeviceIP);//���޸�
    para_net.dwDeviceIP = ntohl(para_net.dwDeviceIP);            //ע��˴��ֽ����ת��
    //printf_debug("deviceip:%d\n",para_net.dwDeviceIP);

    inet_pton(AF_INET,"255.255.255.0",&para_net.dwDeviceIPMask);
    para_net.dwDeviceIPMask = ntohl(para_net.dwDeviceIPMask);

    para_net.wDevicePort = SERVER_PORT_TCP;

    inet_pton(AF_INET, "192.168.11.1",&para_net.dwGatewayIP);
    para_net.dwGatewayIP = ntohl(para_net.dwGatewayIP);

    inet_pton(AF_INET,"192.168.11.53",&para_net.dwManageHostIP);//���޸�
    para_net.dwManageHostIP = ntohl(para_net.dwManageHostIP);

    para_net.wManageHostPort = SERVER_PORT_TCP;

    memcpy(para_net.active_mode_name,"admin" ,5);

    memcpy(para_net.active_mode_pwd,"admin", 5);

    gatewayip = htonl(para_net.dwGatewayIP);
    inet_ntop(AF_INET,&gatewayip,gateway_buf,INET_ADDRSTRLEN); 
    sprintf(gateway,"%s%s",gateway,gateway_buf);

    //ʵ��ip �����ء� �������Ч**********************************************************

    if(set_lan_ip(para_net.dwDeviceIP) != 0)
    {
        err_print("set the ip fail\n");
    }
    if(set_local_lan_gateway(gateway) != 0)
    {
        printf_debug("she zhi wang guan shi bai\n");
    }
    if(set_lan_netmask(para_net.dwDeviceIPMask) != 0)
    {
        err_print("set the netmask fail\n");
    }

    return send_act_cmd(QULIFIED,fd,SXC_PARA_RESTORE,fd);;
}

/*******************************************************************************
 * ����:     ������ƥ��
 * ������:          
 * ����:      
 * ���ݷ���: 
 * ���ݸ���:  
 * ����:          
 * ���:         
 * ����ֵ:�����ʶ�Ӧ�ķ�������         
 *******************************************************************************/
int com_dwBaudRate_get(int rate)
{
    int num = 0;

    switch(rate)
    {
        case 600:
            num = 0;
            break;
        case 1200: 
            num = 1;
            break;
        case 2400: 
            num = 2;
            break;
        case 4800: 
            num = 3;
            break;
        case 9600: 
            num = 4;
            break;
        case 14400: 
            num = 5;
            break;
        case 19200: 
            num = 6;
            break;
        case 38400: 
            num = 7;
            break;
        case 57600: 
            num = 8;
            break;
        case 115200: 
            num = 9;
            break;
        default:
            break;
    }

    return num;
}

int com_baudrate_set(int rate)
{
    int rate_value = 0;
    switch(rate)
    {
        case 0:
            rate_value = raudrate[0];
            break;
        case 1:
            rate_value = raudrate[1];
            break;
        case 2:
            rate_value = raudrate[2];
            break;
        case 3:
            rate_value = raudrate[3];
            break;
        case 4:
            rate_value = raudrate[4];
            break;
        case 5:
            rate_value = raudrate[5];
            break;
        case 6:
            rate_value = raudrate[6];
            break;
        case 7:
            rate_value = raudrate[7];
            break;
        case 8:
            rate_value = raudrate[8];
            break;
        case 9:
            rate_value = raudrate[0];
            break;
    }

    return rate_value;
}

/*******************************************************************************
 * ����:     s�ֶ���ȡ�����ϴ�
 * ������:          
 * ����:      
 * ���ݷ���: 
 * ���ݸ���:  
 * ����:          
 * ���:         
 * ����ֵ:�����ʶ�Ӧ�ķ�������         
 *******************************************************************************/
int alarm_upload_get(char *recv_buf,int fd)
{
    time_t tm;
    struct tm *t;
    INTER_DOOR_ALMOUT *recv_para = (INTER_DOOR_ALMOUT *)recv_buf;

    tm = time(NULL);
    t = localtime(&tm);

    if(recv_para->byAlmType == 1)
    {
        return alarm_upload(t,ALARM_DEV_UPLOAD,NO);
    }
    else if(recv_para->byAlmType == 0)
    {
        return alarm_upload(t,ENTRANCE_ALARM_UPLOAD,NO);
    }
    else if(recv_para->byAlmType == 2)
    {
        return alarm_upload(t,SWITCH_ALARM_UPLOAD,NO);
    }
    else
    {	
        return send_act_cmd(NOSUPPORT,fd,SXC_ALARM_UPLOAD_MANU,fd);
    }
    return 1;
}
