#ifndef _ALARM_INPUT_OUTPUT_H
#define _ALARM_INPUT_OUTPUT_H
#include "global.h"

//#define CK2316_CONFIG_FILE                          "ck2316.config"
//#define ENTRANCE_GUARD_NORMAL_MESSAGE_FILE          "entrance_guard_normal_message.log"
//#define ENTRANCE_GUARD_ALARM_MESSAGE_FILE           "ck2316_alarm_message.log"
//#define ENTRANCE_GUARD_MESSAGE_COUNT_FILE           "entrance_guard_message_count.log"
#define ALARM_INPUT_OUTPUT_CONFIG_FILE              "alarm_input_output.config"
#define ALARM_INPUT_OUTPUT_TIMED_ALARM_FILE         "alarm_input_output_timed_alarm.config"

//#define ALARM_INPUT_OUTPUT_NO_VALID_COMMAND         0x00
#define ALARM_INPUT_OUTPUT_NORMAL_OPERATION                 0x00  //����������������
#define ALARM_INPUT_OUTPUT_SET_LINKAGE_INFO                 0x01  //���ñ���������Ӧ��ϵ
#define ALARM_INPUT_OUTPUT_GET_LINKAGE_INFO                 0x02  //��ȡ����������Ӧ��ϵ
#define ALARM_INPUT_OUTPUT_SET_AND_CANCEL_LINKAGE_ALARM     0x03  //ȡ���������
#define ALARM_INPUT_OUTPUT_RESTORE_TO_DEFAULT               0x04  //�ָ�Ĭ�ϲ���
#define ALARM_INPUT_OUTPUT_SET_ALARM_DURATION               0x05  //���ñ����������ʱ��
#define ALARM_INPUT_OUTPUT_GET_ALARM_DURATION               0x06  //��ȡ�����������ʱ��


#define MAX_ALARM_DURATION                          1440   //24Сʱ
#define MAX_ALARM_LINKAGE_INPUT_OBJECT              9
#define MAX_ALARM_INPUT_LINKAGE_OUTPUT_OBJECT       8

typedef struct 
{
    int current_fd;
    unsigned int if_alarm_input_output_device_is_ready; //������������豸�Ƿ�׼����
    /* unsigned int setup_command_set ����
    ���¾�Ϊʮ������,����Ϊ����ֵ,ĳһλΪ1,��ʾ��λ��Ӧ��������Ч
    �ֽ�4������(���ֽ�)
etup_command_set   �ֽ�3������
    �ֽ�2������
    �ֽ�1��
    2-7������
    1�����ñ���������Ӧ��ϵ
    0����ȡ����������Ӧ��ϵ */
    unsigned int setup_command_set;         //�洢��������(��8λ��ÿһλ��ʾһ������) 
    /* unsigned int alarm_linkage_input_object ����
    ���¾�Ϊʮ����,����Ŀȡֵ1-9,����Ϊ����ֵ
    9���Ž��Ƿ����ű�������
    8����������8
    7����������7
    6����������6
    5����������5
    4����������4
    3����������3
    2����������2
    1����������1
    0����Ч�����������         */
    unsigned int max_alarm_linkage_input_object;  //default is 9
    unsigned int alarm_linkage_input_object;//1-9, ���������������
    unsigned int max_alarm_input_linkage_output_object; //default is 8
    unsigned int alarm_linkage_output_object;//1-8, ���������������
    unsigned int alarm_input_linkage_output_object[32]; //ÿ����������������󣬶�Ӧ���32������������������뱨�������������һһ��Ӧ��ÿ��int��ÿһλ��Ӧһ�����������������һ�������������������Զ�Ӧ������������������
    unsigned int real_time_alarm_output_objcet;  //���ڱ����������������ÿһλ��Ӧһ·�����1Ϊ���ڱ�����0�ޱ���
    /* unsigned int cancel_linkage_alarm ����
    ���¾�Ϊʮ����,����Ŀȡֵ0-9,����Ϊ����ֵ
    8��ȡ���������8
    7��ȡ���������7
    6��ȡ���������6
    5��ȡ���������5
    4��ȡ���������4
    3��ȡ���������3
    2��ȡ���������2
    1��ȡ���������1
    0��ȡ�����б�������������������       */
    unsigned int set_and_cancel_linkage_alarm_channel;
    unsigned int alarm_duration;  //��������ʱ��
    LAST_ALARM_UPLOAD_TIME upload_time;

}alarm_input_output_arg;


void *pthread_alarm_input_output(void *arg);
alarm_input_output_arg alarm_input_output_data;

#endif
