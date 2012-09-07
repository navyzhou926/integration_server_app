#ifndef _MATRIX_CONTROL_H
#define _MATRIX_CONTROL_H

#define MATRIX_CONTROL_NO_VALID_COMMAND         0x00
#define MATRIX_SWITCHOVER_COMMAND                   0x01
#define MATRIX_CANCEL_SWITCHOVER_COMMAND            0x02
#define MATRIX_CANCEL_ALL_SWITCHOVER_COMMAND         0x03

typedef struct 
{
    int current_fd;
    /* unsigned int setup_command_set 介绍
    字节4：保留(高字节)
    字节3：保留
    字节2：保留
    字节1：
    2-7：保留
    1：矩阵取消切换
    0：矩阵切换         */
    unsigned int setup_command_set;         //存储设置命令(低16位，每一位表示一个命令) 
    unsigned char matrix_current_switchover_in_value;  //存储矩阵视频输入通道号
    unsigned char matrix_current_switchover_out_value; //存储矩阵视频输出通道号
    //char if_ck2316_alive;                   //当前CK2316报警主机是否在线
    //char ck2316_defence_status;             //CK2316报警主机布撤防的状态(布防，撤防)
    //unsigned char ck2316_defence_area_alarm_status; //CK2316防区报警状态(0:无报警 1:普通防区报警 2:紧急防区报警 3:普通和紧急防区都有报警)
    #if 0
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
    #endif
}matrix_control_arg;

matrix_control_arg matrix_control_data;

void *pthread_matrix_control(void *arg);

#endif
