#ifndef _ENTRANCE_GUARD_H
#define _ENTRANCE_GUARD_H

#include "net_server.h"
#include "global.h"

#define IS_OPENED       1
#define IS_CLOSED       0
#define OPEN_DOOR       1
#define CLOSE_DOOR      0
#define NORMAL_OPEN   0
#define NORMAL_CLOSE 1

#define CLIENT_CONTROL_DOOR_HOLD_TIME_COUNT         21
#define BUTTON_CONTROL_DOOR_HOLD_TIME_COUNT         17     

#define HANDSHAKE_SEND_SIZE                         11
#define OPEN_DOOR_SEND_SIZE                         11
#define CLOSE_DOOR_SEND_SIZE                        11
#define DOOR_LOCK_DEFAULT_OPEN_SETUP_SEND_SIZE      19
#define DOOR_LOCK_DEFAULT_CLOSE_SETUP_SEND_SIZE     19
#define CLIENT_CONTROL_DOOR_HOLD_TIME_SEND_SIZE     27 
#define BUTTON_CONTROL_DOOR_HOLD_TIME_SEND_SIZE     19 
#define DOOR_CONTACT_DEFAULT_OPEN_SETUP_SEND_SIZE   19
#define DOOR_CONTACT_DEFAULT_CLOSE_SETUP_SEND_SIZE   19

#define MAX_RECV_SIZE                               29
#define SEARCH_ENTRANCE_GUARD_RECV_SIZE             29
#define HANDSHAKE_SETUP_RECV_SIZE                   256

#define MATCH_HANDSHAKE_TIMEOUT                     300000
#define ENTRANCE_GUARD_RECV_OVER_TIME               400000


#define ENTRANCE_GUARD_CONFIG_FILE                  "entrance_guard.config"
#define ENTRANCE_GUARD_NORMAL_MESSAGE_FILE          "entrance_guard_normal_message.log"
#define ENTRANCE_GUARD_ALARM_MESSAGE_FILE           "entrance_guard_alarm_message.log"
#define ENTRANCE_GUARD_MESSAGE_COUNT_FILE           "entrance_guard_message_count.log"

#define ENTRANCE_GUARD_NO_VALID_COMMAND             0x00
#define ENTRANCE_GUARD_OPEN_DOOR                    0x01
#define ENTRANCE_GUARD_CLOSE_DOOR                   0x02
#define ENTRANCE_GUARD_RELAY_OPEN_DOOR              0x04
#define ENTRANCE_GUARD_RELAY_CLOSE_DOOR             0x08
#define ENTRANCE_GUARD_CLIENT_SET_DOOR_HOLD_TIME    0x10
#define ENTRANCE_GUARD_BUTTON_SET_DOOR_HOLD_TIME    0x20
#define ENTRANCE_GUARD_GET_NORMAL_MESSAGE           0x40    
#define ENTRANCE_GUARD_GET_ALARM_MESSAGE            0x80
#define ENTRANCE_GUARD_DOOR_CONTACT_NORMALLY_OPEN   0x100
#define ENTRANCE_GUARD_DOOR_CONTACT_NORMALLY_CLOSE  0x200
#define ENTRANCE_GUARD_CANCEL_ALARM_STATUS          0x400


#define VALID_OPEN_DOOR_LOCK                        0x0c
#define VALID_CLOSE_DOOR_LOCK                       0x0d
#define VALID_OPEN_DOOR_CONTACT_1                   0x12
#define VALID_OPEN_DOOR_CONTACT_2                   0x10
#define VALID_CLOSE_DOOR_CONTACT                    0x88                    
#define INVALID_OPEN_DOOR_CONTACT_1                 0x93
#define INVALID_OPEN_DOOR_CONTACT_2                 0x91

#define MAX_ALARM_MESSAGE_NUM                       200
#define MAX_NORMAL_MESSAGE_NUM                      200

#define ENTRANCE_GUARD_BOARD_RATE   4

#define ENTRANCE_GUARD_SERIAL_PORT          0
#define ENTRANCE_GUARD_BOARD                9600
#define ENTRANCE_GUARD_UART_DATA_BIT        3
#define ENTRANCE_GUARD_UART_CHECK_BIT       0
#define ENTRANCE_GUARD_UART_STOP_BIT        0


typedef struct 
{
    int current_fd;
    char if_entrance_guard_alive;                 //��ǰ�Ž��Ƿ�����
    unsigned int door_lock_relay_status_setup;   //�����̵���״̬����(����(Ĭ��)������)
    unsigned int door_contact_detection_mode_setup;      //�Ŵż�ⷽʽ����(��·(Ĭ��), ��·)
    unsigned int door_status;              //�ŵ�״̬(���ţ�����)
    //char if_is_setup_command;             //���Ϸ����������Ž������Ƿ�����������

    /* unsigned int setup_command_set ����
    �ֽ�4������(���ֽ�)
    �ֽ�3������
    �ֽ�2������
    10���Ŵ�ȡ������״̬
    9���Ŵż�ⷽʽ����(��·)(Ĭ��)
    8���Ŵż�ⷽʽ����(��·)
    �ֽ�1��
    7����ȡ������Ϣ
    6����ȡ��ͨ��Ϣ
    5�����Ű�ť�����ŵı���ʱ��(1-255��)
    4���ͻ��������ŵı���ʱ��(1-255��)
    3�������̵���״̬����(����)
    2�������̵���״̬����(����)
    1����״̬����(����)
    0����״̬����(����)          */
    unsigned int setup_command_set;         //�洢��������(��16λ��ÿһλ��ʾһ������) 
    unsigned int client_set_door_hold_time;//�ͻ��������ű���ʱ��
    unsigned int button_set_door_hold_time;//���Ű�ť�����ű���ʱ��
    unsigned char current_handshake_num;    //��ǰ������Ļ�ֵ
    unsigned char if_set_door_hold_time_is_valid;//�����ű���ʱ���Ƿ�Ϸ�
    unsigned char if_has_delete_offline_alarm;//�ж��Ƿ�ɾ���Ž����ߺ����ı���
    unsigned char if_has_entrance_guard_alarm; //�ж��Ƿ����Ž�����(�Ƿ�����)
    unsigned int current_normal_message_num; //��ǰ�ļ��д洢����ͨ��Ϣ����
    unsigned int current_alarm_message_num;  //��ǰ�ļ��д洢�ı�����Ϣ����
    SERIAL_PAMATER entrance_guard_serial_pamater;
    LAST_ALARM_UPLOAD_TIME upload_time;
}entrance_guard_arg;


void *pthread_entrance_guard(void *arg);
extern entrance_guard_arg entrance_guard_data;

#endif

