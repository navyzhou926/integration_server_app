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
#include "cradle_head_control.h"
#include "net_server.h"
#include "alarm_input_output.h"

#define DEBUG


#ifdef DEBUG
#define printf_debug(fmt, arg...)  printf(fmt, ##arg)
#else       
#define printf_debug(fmt,arg...) do{ } while (0)
#endif 

#define PATH_HOME "/home"

 FILE *fp_dev_config_file;

int server_listen_fd = -1,server_connect_fd = -1;

//�ļ�·��
char *net_para_file_path = "./net_para.init";
int if_have_net_client_connect = NO;

int main(int argc, char *argv[])
{
    pthread_t tid_entrance_guard;
    pthread_t tid_ck2316_alarm;
    pthread_t tid_matrix_control;
    pthread_t tid_cradle_head_control;
    pthread_t tid_handshake;
    pthread_t tid_server_passive;
    pthread_t tid_server_active;
    pthread_t tid_alarm_input_output;
    pthread_t tid_watchdog;
    int net_para_file_len = 0;
	 
    net_para_file_len = sizeof(INTER_NETCFG_FILE);
    net_para_init(net_para_file_path,net_para_file_len);
    init_serial_dev_parameter();
     #if 1
    //创建线程，完成被动模式通信的任�?    
    if (pthread_create(&tid_server_passive, NULL, pthread_server_passive,NULL) != 0) 
    {
        printf("FUNC[%s] LINE[%d]\tCan't create server passive thread !\n",__FUNCTION__, __LINE__);
        exit(1);
    }
    #endif
    #if 1
    //����ģʽ��ͨ��
    if (pthread_create(&tid_server_active, NULL, pthread_server_active,NULL) != 0) 
    {
        printf("FUNC[%s] LINE[%d]\tCan't create server passive thread !\n",__FUNCTION__, __LINE__);
        exit(1);
    }
    #endif
     
    #if 1
    //创建线程，完成被动模式通信的任�?  
    if (pthread_create(&tid_handshake, NULL, pthread_handshake,NULL) != 0) 
    {
        printf("FUNC[%s] LINE[%d]\tCan't create server passive thread !\n",__FUNCTION__, __LINE__);
        exit(1);
    }
    #endif

    #if 1
    //创建与门禁通信的线�?   
    if (pthread_create(&tid_entrance_guard, NULL, pthread_entrance_guard, NULL) != 0) 
    {
        printf("FUNC[%s] LINE[%d]\tCan't create entrance guard thread !\n",__FUNCTION__, __LINE__);
        exit(1);
    }
    #endif

    #if 1
    //创建与报警器通信的线�?    
    if (pthread_create(&tid_ck2316_alarm, NULL, pthread_ck2316_alarm, NULL) != 0) 
    {
        printf("FUNC[%s] LINE[%d]\tCan't create ck2316 alarm thread !\n",__FUNCTION__, __LINE__);
        exit(1);
    }
    #else
        #if 0
    if (pthread_create(&tid_ck2316_alarm, NULL, pthread_temp_test, NULL) != 0) 
    {
        printf("FUNC[%s] LINE[%d]\tCan't create temp test thread !\n",__FUNCTION__, __LINE__);
        exit(1);
    }
        #endif
    #endif

    #if 1
    //创建与矩阵通信的线�?    
    if (pthread_create(&tid_matrix_control, NULL, pthread_matrix_control, NULL) != 0) 
    {
        printf("FUNC[%s] LINE[%d]\tCan't create maxtrix control thread !\n",__FUNCTION__, __LINE__);
        exit(1);
    }
    #endif

    #if 1
    //创建与云台通信的线�?   
    if (pthread_create(&tid_cradle_head_control, NULL, pthread_cradle_head_control, NULL) != 0) 
    {
        printf("FUNC[%s] LINE[%d]\tCan't create cradle head control thread !\n",__FUNCTION__, __LINE__);
        exit(1);
    }
    #endif

    #if 1
    //创建与报警输入输出通信的线�?   
    if (pthread_create(&tid_alarm_input_output, NULL, pthread_alarm_input_output, NULL) != 0) 
    {
        printf("FUNC[%s] LINE[%d]\tCan't create alarm input and output thread !\n",__FUNCTION__, __LINE__);
        exit(1);
    }
    #endif

    #if 1
    //���Ź��̡߳� 
    if (pthread_create(&tid_watchdog, NULL, pthread_watchdog, NULL) != 0) 
    {
        printf("FUNC[%s] LINE[%d]\tCan't create watchdog thread !\n",__FUNCTION__, __LINE__);
        exit(1);
    }
    #endif

 	pthread_join(tid_entrance_guard,NULL);
	pthread_join(tid_ck2316_alarm,NULL);
	pthread_join(tid_matrix_control,NULL);
	pthread_join(tid_cradle_head_control,NULL);
	pthread_join(tid_handshake,NULL);
	pthread_join(tid_alarm_input_output,NULL);
	pthread_join(tid_server_passive,NULL);
	pthread_join(tid_server_active,NULL);
	
	while ( (-1 == close(server_connect_fd)) && (errno == EINTR) );
	while ( (-1 == close(server_listen_fd)) && (errno == EINTR) );
	exit(EXIT_SUCCESS);
}

