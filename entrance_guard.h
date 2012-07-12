#ifndef _ENTRANCE_GUARD_H
#define _ENTRANCE_GUARD_H

#define IS_OPENED       1
#define IS_CLOSED       0
#define OPEN_DOOR       1
#define CLOSE_DOOR      0

#define MAX_RECV_SIZE                      29
#define SEARCH_ENTRANCE_GUARD_RECV_SIZE    29
#define HANDSHAKE_SETUP_RECV_SIZE          256

#define MATCH_HANDSHAKE_TIMEOUT            300000


#define ENTRANCE_GUARD_CONFIG_FILE         "entrance_guard.config"

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

typedef struct 
{
    char if_entrance_guard_alive;           //当前门禁是否在线
    unsigned char door_lock_relay_status;   //门锁继电器状态(常开，常闭)
    unsigned char door_status;              //门的状态(开门，关门)
    unsigned char current_handshake_num;    //当前握手码的基值
    //char if_is_setup_command;               //整合服务器发给门禁的码是否是设置命令

    /* unsigned int setup_command_set 介绍
    字节4：保留(高字节)
    字节3：保留
    字节2：保留
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
    unsigned char client_set_door_hold_time;//客户端设置门保持时间
    unsigned char button_set_door_hold_time;//开门按钮设置门保持时间
}entrance_guard_arg;


void *pthread_entrance_guard(void *arg);
extern entrance_guard_arg entrance_guard_data;

#endif
