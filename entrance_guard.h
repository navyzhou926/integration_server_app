#ifndef _ENTRANCE_GUARD_H
#define _ENTRANCE_GUARD_H

#define IS_OPENED       1
#define IS_CLOSED       0
#define OPEN_DOOR       1
#define CLOSE_DOOR      0

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

typedef struct 
{
    int current_fd;
    char if_entrance_guard_alive;                 //当前门禁是否在线
    unsigned int door_lock_relay_status_setup;   //门锁继电器状态设置(常开(默认)，常闭)
    unsigned int door_contact_detection_mode_setup;      //门磁检测方式设置(短路(默认), 开路)
    unsigned int door_status;              //门的状态(开门，关门)
    //char if_is_setup_command;             //整合服务器发给门禁的码是否是设置命令

    /* unsigned int setup_command_set 介绍
    字节4：保留(高字节)
    字节3：保留
    字节2：保留
    10：门磁取消报警状态
    9：门磁检测方式设置(短路)(默认)
    8：门磁检测方式设置(开路)
    字节1：
    7：获取报警消息
    6：获取普通消息
    5：开门按钮设置门的保持时间(1-255秒)
    4：客户端设置门的保持时间(1-255秒)
    3：门锁继电器状态设置(常闭)
    2：门锁继电器状态设置(常开)
    1：门状态设置(关门)
    0：门状态设置(开门)          */
    unsigned int setup_command_set;         //存储设置命令(低16位，每一位表示一个命令) 
    unsigned int client_set_door_hold_time;//客户端设置门保持时间
    unsigned int button_set_door_hold_time;//开门按钮设置门保持时间
    unsigned char current_handshake_num;    //当前握手码的基值
    unsigned char if_set_door_hold_time_is_valid;//设置门保持时间是否合法
    unsigned char if_has_delete_offline_alarm;//判断是否删除门禁离线后发生的报警
    unsigned char if_has_entrance_guard_alarm; //判断是否有门禁报警(非法开门)
    unsigned int current_normal_message_num; //当前文件中存储的普通消息总数
    unsigned int current_alarm_message_num;  //当前文件中存储的报警消息总数

}entrance_guard_arg;


void *pthread_entrance_guard(void *arg);
extern entrance_guard_arg entrance_guard_data;

#endif

