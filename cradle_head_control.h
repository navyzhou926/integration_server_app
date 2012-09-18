#ifndef _CRADLE_HEAD_CONTROL_H
#define _CRADLE_HEAD_CONTROL_H

#include "net_server.h"

#define CRADLE_HEAD_CONTROL_NO_VALID_COMMAND         0xff

//云台控制相关--命令
#define PTZ_UP                  0   //以SS速度向上
#define PTZ_DOWN                1   //以SS速度向下
#define PTZ_LEFT                2   //以SS速度向左
#define PTZ_RIGHT               3   //以SS速度向右
#define PTZ_APERTURE_BIG        4   //光圈调节，变大
#define PTZ_APERTURE_SMALL      5   //光圈调节，缩小
#define PTZ_FOCUS_IN            6   //聚焦近，焦点前调
#define PTZ_FOCUS_OUT           7   //聚焦远，焦点后调
#define PTZ_ZOOM_IN             8   //变倍长(特写, 放大)，焦距变大
#define PTZ_ZOOM_OUT            9   //变倍短(广角, 缩小)， 焦距变小
#define PTZ_AUTO_ZOOM           10  //开始自动焦距（自动倍率）
#define PTZ_AUTO_FOCUS          11  //开自动调焦
#define PTZ_AUTO_APERTURE       12  //开自动光圈
#define PTZ_SET_PTZBIT          13  //设置预置点
#define PTZ_CLE_PTZBIT          14  //调用预置点
#define PTZ_STOP_ALL            15  //停止所有连续量（镜头，云台）动作
#define AUX_PWRON1              16  //辅助功能1开
#define AUX_PWROFF1             17  //辅助功能1关
#define AUX_PWRON2              18  //辅助功能2开
#define AUX_PWROFF2             19  //辅助功能2关

//云台控制相关--协议类型
#define Palco_D   1
#define Palco_P   2
#define YAAN      3
#define Philips   4     //飞利浦高速球通讯协议（需要485－曼码转换器）
#define TOTA120   5     //TOTA解码器通讯协议
#define RV800     6     //科立解码器通讯协议
#define S1601     7     //三乐协议
#define CLT_168   8     //保千里的协议
#define TD_500    9     //保千里高速球通讯协议
#define V1200     10    //银信V1200解码器、银信矩阵通讯协议
#define ZION      11    //锡安数码 CBR2400
#define ANT       12    //中晖协议
#define CBC       13    //日本CBC高速球通讯协议
#define CS850A    14    //松下CS850A高速球通讯协议
#define CONCORD   15    //2000b、维多B01及美国concord智能快球通讯协议
#define HD600     16    //CCVE闭路监控系统通讯协议
#define SAMSUNG   17    //三星641－643高速球通讯协议
#define PIH       18    //利凌PIH717解码器通讯协议
#define MG_CS160  19    //明景B01通讯协议
#define WISDOM    20    //WISDOM解码器通讯协议
#define NEOCOM    21    //耐康姆协议


#define CRADLE_HEAD_SERIAL_PORT          2
#define CRADLE_HEAD_UART_BOARD           9600
#define CRADLE_HEAD_UART_DATA_BIT        3  //8bit
#define CRADLE_HEAD_UART_CHECK_BIT       0
#define CRADLE_HEAD_UART_STOP_BIT        0

typedef struct 
{
    int current_fd;
    unsigned int setup_command_set;         //存储设置命令(低16位，每一位表示一个命令) 
    unsigned char cradle_head_protocol_type;     //云台协议类型
    unsigned char cradle_head_address;      //云台地址(1-255)
    unsigned char cradle_head_move_speed;   //云台移动速度(1-255)
    unsigned char cradle_head_preset_point; //云台预置点(1-32, 33-64)
    SERIAL_PAMATER cradle_head_control_serial_pamater;
}cradle_head_control_arg;

cradle_head_control_arg cradle_head_control_data;

void *pthread_cradle_head_control(void *arg);

#endif

