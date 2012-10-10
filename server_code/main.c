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

//#define DEBUG


#ifdef DEBUG
#define printf_debug(fmt, arg...)  printf(fmt, ##arg)
#else       
#define printf_debug(fmt,arg...) do{ } while (0)
#endif 

#define PATH_HOME "/home"

 FILE *fp_dev_config_file;

//unsigned char net_recv_verify_code[] = {0xAA,0x55,0xAA,0x55,0xAA,0x55,0xAA,0x55,0xAA,0x55,0xAA,0x55,0xAA,0x55,0xAA};
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

#if 0
if (strncmp(net_recv_buffer, net_recv_verify_code, 9) == 0) 
            {
                switch(net_recv_buffer[14])
                {
                    case 0x00: //û��ָ��,�������������������
                        alarm_input_output_data.setup_command_set = ALARM_INPUT_OUTPUT_NORMAL_OPERATION;
                        break;
                    case 0x01: //���ñ���������Ӧ��ϵ
                        if (alarm_input_output_data.real_time_alarm_output_objcet) 
                        {
                            //���緢����ȡ�����б��������Ȼ�������ñ���������Ӧ��ϵ
                            printf_debug("FUNC[%s] LINE[%d]\tPlease cancel alarm output, then try it again!\n",__FUNCTION__, __LINE__);
                        }
                        else
                        {
                            if (net_recv_buffer[12] < 1 || net_recv_buffer[12] > MAX_ALARM_LINKAGE_INPUT_OBJECT) 
                            {
                                //���緢�� ��Ч����
                                printf_debug("FUNC[%s] LINE[%d]\tInvalid arg alarm_linkage_input_object, it should be 1 to 9\n",__FUNCTION__, __LINE__);
                            }
                            else
                            {
                                alarm_input_output_data.alarm_linkage_input_object = net_recv_buffer[12];
                                alarm_input_output_data.setup_command_set = ALARM_INPUT_OUTPUT_SET_LINKAGE_INFO;
                                alarm_input_output_data.alarm_input_linkage_output_object[alarm_input_output_data.alarm_linkage_input_object-1] = net_recv_buffer[13];
                            }
                        }
                        break;
                    case 0x02: //��ȡ����������Ӧ��ϵ
                        if (net_recv_buffer[12] < 1 || net_recv_buffer[12] > MAX_ALARM_LINKAGE_INPUT_OBJECT) 
                        {
                            //���緢�� ��Ч����
                            printf_debug("FUNC[%s] LINE[%d]\tInvalid arg alarm_linkage_input_object, it should be 1 to 9\n",__FUNCTION__, __LINE__);
                        }
                        else
                        {
                            alarm_input_output_data.alarm_linkage_input_object = net_recv_buffer[12];
                            //���緢�� ��ȡ����������Ӧ��ϵ
                            printf_debug("FUNC[%s] LINE[%d]\tChannel %d: 0x%X\n",__FUNCTION__, __LINE__, alarm_input_output_data.alarm_linkage_input_object, alarm_input_output_data.alarm_input_linkage_output_object[alarm_input_output_data.alarm_linkage_input_object-1]);
                        }
                        break;
                    case 0x03: //ȡ���������
                            if (net_recv_buffer[11] < 0 || net_recv_buffer[11] > MAX_ALARM_INPUT_LINKAGE_OUTPUT_OBJECT) 
                            {
                                //���緢�� ��Ч����
                                printf_debug("FUNC[%s] LINE[%d]\tInvalid arg cancel_linkage_alarm_channel, it should be 0 to 8\n",__FUNCTION__, __LINE__);
                            }
                            else
                            {
                                alarm_input_output_data.setup_command_set = ALARM_INPUT_OUTPUT_CANCEL_LINKAGE_ALARM;
                                alarm_input_output_data.cancel_linkage_alarm_channel = net_recv_buffer[11];
                            }
                        break;
                    case 0x04: //�ָ�Ĭ�ϲ���
                        if (alarm_input_output_data.real_time_alarm_output_objcet) 
                        {
                            //���緢����ȡ�����б��������Ȼ�������ñ���������Ӧ��ϵ
                            printf_debug("FUNC[%s] LINE[%d]\tPlease cancel alarm output, then try it again!\n",__FUNCTION__, __LINE__);
                        }
                        else
                        {
                            alarm_input_output_data.setup_command_set = ALARM_INPUT_OUTPUT_RESTORE_TO_DEFAULT;
                        }
                        break;
                    case 0x05: //���ñ����������ʱ��
                        if (alarm_input_output_data.real_time_alarm_output_objcet) 
                        {
                            //���緢����ȡ�����б��������Ȼ�������ñ���������Ӧ��ϵ
                            printf_debug("FUNC[%s] LINE[%d]\tPlease cancel alarm output, then try it again!\n",__FUNCTION__, __LINE__);
                        }
                        else
                        {
                                if (net_recv_buffer[11] < 0 || net_recv_buffer[11] > MAX_ALARM_INPUT_LINKAGE_OUTPUT_OBJECT) 
                                {
                                    //���緢�� ��Ч����
                                    printf_debug("FUNC[%s] LINE[%d]\tInvalid arg alarm_linkage_output_object, it should be 1 to 8\n",__FUNCTION__, __LINE__);
                                
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
                    default :
                        //���緢����Ч����
                        printf_debug("FUNC[%s] LINE[%d]\tAlarm input and output invalid net command!\n",__FUNCTION__, __LINE__);
                        alarm_input_output_data.setup_command_set = ALARM_INPUT_OUTPUT_NORMAL_OPERATION;
                        break;
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

