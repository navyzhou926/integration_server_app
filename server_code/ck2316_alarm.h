#ifndef _CK2316_ALARM_H
#define _CK2316_ALARM_H

#include "net_server.h"
#include "global.h"
#include <time.h>

#define CK2316_MAX_RECV_SIZE                        64
#define CK2316_HANDSHAKE_SIZE                        3

#define CK2316_RECV_TIMEOUT                         2
#define CK2316_RECV_FINISH_TIMEOUT                  15000

#define IS_DEFENCED                                 1   //�ǲ���״̬
#define IS_ABANDONED                                0   //�ǳ���״̬

#define CK2316_CONFIG_FILE                          "ck2316.config"
//#define ENTRANCE_GUARD_NORMAL_MESSAGE_FILE          "entrance_guard_normal_message.log"
//#define ENTRANCE_GUARD_ALARM_MESSAGE_FILE           "ck2316_alarm_message.log"
//#define ENTRANCE_GUARD_MESSAGE_COUNT_FILE           "entrance_guard_message_count.log"

#define CK2316_NO_VALID_COMMAND                     0x00
#define CK2316_SYSTEM_DEFENCE                       0x01
#define CK2316_SYSTEM_ABANDON                       0x02
#define CK2316_ELIMINATE_ALARM_MEMORY               0x04
#define CK2316_BYPASS_DEFENCE_AREA                  0x08
#define CK2316_ALARM_HOST_RESET                     0x10
//#define CK2316_SIMULATE_KEYBOARD_ADDRESS_SETUP      0x20
//#define CK2316_SIMULATE_KEYBOARD_PASSWORD_SETUP     0x40
#define CK2316_CHECK_COMMAND_IF_SETUP_SUCCESSFUL    0x80000000

#define CK2316_MAX_ALARM_MESSAGE_NUM                200
#define CK2316_MAX_NORMAL_MESSAGE_NUM               200

#define CK2316_SERIAL_PORT          4
#define CK2316_UART_BOARD           2400
#define CK2316_UART_DATA_BIT        3
#define CK2316_UART_CHECK_BIT       2
#define CK2316_UART_STOP_BIT        0


typedef struct 
{
    int current_fd;
    char if_ck2316_alive;                   //��ǰCK2316���������Ƿ�����
    char ck2316_defence_status;             //CK2316����������������״̬(����������)
    unsigned char ck2316_defence_area_alarm_status; //CK2316��������״̬(0:�ޱ��� 1:��ͨ�������� 2:������������ 3:��ͨ�ͽ����������б���)
    unsigned char ck2316_defence_area_alarm_memory_value[2]; //CK2316������������
    unsigned char ck2316_defence_area_real_time_alarm_value[2];//CK2316ʵʱ�����ķ���
    unsigned char ck2316_defence_area_bypass_value[2]; //��16λ�ֱ��ʾ16����·������
    //char if_is_setup_command;             //���Ϸ����������Ž������Ƿ�����������

    /* unsigned int setup_command_set ����
    �ֽ�4������(���ֽ�)
    �ֽ�3������
    �ֽ�2������
    �ֽ�1��
    7������
    6�����������û���������
    5����������ģ����̵�ַ����
    4������������λ
    3��ϵͳ��·
    2��������������
    1��ϵͳ����
    0��ϵͳ����          */
    unsigned int setup_command_set;         //�洢��������(��16λ��ÿһλ��ʾһ������) 
    unsigned int ck2316_simulate_keyboard_address; //ģ�����CK2316���������ļ��̵�ַ
    unsigned int ck2316_user_password[4];

    //unsigned char if_has_delete_offline_alarm;//�ж��Ƿ�ɾ���Ž����ߺ����ı���
    //unsigned int current_normal_message_num; //��ǰ�ļ��д洢����ͨ��Ϣ����
    //unsigned int current_alarm_message_num;  //��ǰ�ļ��д洢�ı�����Ϣ����
    SERIAL_PAMATER ck2316_alarm_serial_pamater;
   LAST_ALARM_UPLOAD_TIME upload_time;
   unsigned char last_alarm_status;
   unsigned int last_alarm_memory_channle;

}ck2316_alarm_arg;

extern unsigned char if_search_keyboard;
extern FILE *fp_ck2316_config_file;

void *pthread_ck2316_alarm(void *arg);
ck2316_alarm_arg ck2316_alarm_data;
int ck2316_simulate_keyboard_address_setup(unsigned int address);
extern unsigned char bypass_defence_area_code[][3];
int ck2316_user_password_setup(unsigned int user_password[4]);
extern int alarm_upload(struct tm *t,int alarm_type,int flag);

#endif
