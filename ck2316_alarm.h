#ifndef _CK2316_ALARM_H
#define _CK2316_ALARM_H

#define CK2316_MAX_RECV_SIZE                        64
#define CK2316_HANDSHAKE_SIZE                        3

#define CK2316_RECV_TIMEOUT                         2 //2s
#define CK2316_RECV_FINISH_TIMEOUT                  15000

#define IS_DEFENCED                                 1   //是布防状态
#define IS_ABANDONED                                0   //是撤防状态

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

typedef struct 
{
    char if_ck2316_alive;                   //当前CK2316报警主机是否在线
    char ck2316_defence_status;             //CK2316报警主机布撤防的状态(布防，撤防)
    unsigned char ck2316_defence_area_alarm_status; //CK2316防区报警状态(0:无报警 1:普通防区报警 2:紧急防区报警 3:普通和紧急防区都有报警)
    unsigned char ck2316_defence_area_alarm_memory_value[2]; //CK2316防区报警记忆
    unsigned char ck2316_defence_area_real_time_alarm_value[2];//CK2316实时报警的防区
    unsigned char ck2316_defence_area_bypass_value[2]; //低16位分别表示16个旁路防区号
    //char if_is_setup_command;             //整合服务器发给门禁的码是否是设置命令

    /* unsigned int setup_command_set 介绍
    字节4：保留(高字节)
    字节3：保留
    字节2：保留
    字节1：
    7：保留
    6：报警主机用户密码设置
    5：报警主机模拟键盘地址设置
    4：报警主机复位
    3：系统旁路
    2：消除报警记忆
    1：系统撤防
    0：系统布防          */
    unsigned int setup_command_set;         //存储设置命令(低16位，每一位表示一个命令) 
    unsigned int ck2316_simulate_keyboard_address; //模拟控制CK2316报警主机的键盘地址
    unsigned int ck2316_user_password[4];

    //unsigned char if_has_delete_offline_alarm;//判断是否删除门禁离线后发生的报警
    //unsigned int current_normal_message_num; //当前文件中存储的普通消息总数
    //unsigned int current_alarm_message_num;  //当前文件中存储的报警消息总数

}ck2316_alarm_arg;

extern FILE *fp_ck2316_config_file;

void *pthread_ck2316_alarm(void *arg);
ck2316_alarm_arg ck2316_alarm_data;
int ck2316_simulate_keyboard_address_setup(unsigned int address);
extern unsigned char bypass_defence_area_code[][3];

#endif
