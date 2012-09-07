#ifndef _ALARM_INPUT_OUTPUT_H
#define _ALARM_INPUT_OUTPUT_H


//#define CK2316_CONFIG_FILE                          "ck2316.config"
//#define ENTRANCE_GUARD_NORMAL_MESSAGE_FILE          "entrance_guard_normal_message.log"
//#define ENTRANCE_GUARD_ALARM_MESSAGE_FILE           "ck2316_alarm_message.log"
//#define ENTRANCE_GUARD_MESSAGE_COUNT_FILE           "entrance_guard_message_count.log"
#define ALARM_INPUT_OUTPUT_CONFIG_FILE              "alarm_input_output.config"
#define ALARM_INPUT_OUTPUT_TIMED_ALARM_FILE         "alarm_input_output_timed_alarm.config"

//#define ALARM_INPUT_OUTPUT_NO_VALID_COMMAND         0x00
#define ALARM_INPUT_OUTPUT_NORMAL_OPERATION                 0x00  //报警联动正常操作
#define ALARM_INPUT_OUTPUT_SET_LINKAGE_INFO                 0x01  //设置报警联动对应关系
#define ALARM_INPUT_OUTPUT_GET_LINKAGE_INFO                 0x02  //获取报警联动对应关系
#define ALARM_INPUT_OUTPUT_SET_AND_CANCEL_LINKAGE_ALARM     0x03  //取消报警输出
#define ALARM_INPUT_OUTPUT_RESTORE_TO_DEFAULT               0x04  //恢复默认参数
#define ALARM_INPUT_OUTPUT_SET_ALARM_DURATION               0x05  //设置报警输出持续时间
#define ALARM_INPUT_OUTPUT_GET_ALARM_DURATION               0x06  //获取报警输出持续时间


#define MAX_ALARM_DURATION                          1440   //24小时
#define MAX_ALARM_LINKAGE_INPUT_OBJECT              9
#define MAX_ALARM_INPUT_LINKAGE_OUTPUT_OBJECT       8

typedef struct 
{
    int current_fd;
    unsigned int if_alarm_input_output_device_is_ready; //报警输入输出设备是否准备好
    /* unsigned int setup_command_set 介绍
    以下均为十六进制,其它为保留值,某一位为1,表示该位对应的命令生效
    字节4：保留(高字节)
etup_command_set   字节3：保留
    字节2：保留
    字节1：
    2-7：保留
    1：设置报警联动对应关系
    0：获取报警联动对应关系 */
    unsigned int setup_command_set;         //存储设置命令(低8位，每一位表示一个命令) 
    /* unsigned int alarm_linkage_input_object 介绍
    以下均为十进制,本项目取值1-9,其它为保留值
    9：门禁非法开门报警输入
    8：报警输入8
    7：报警输入7
    6：报警输入6
    5：报警输入5
    4：报警输入4
    3：报警输入3
    2：报警输入2
    1：报警输入1
    0：无效联动输入对象         */
    unsigned int max_alarm_linkage_input_object;  //default is 9
    unsigned int alarm_linkage_input_object;//1-9, 报警联动输入对象
    unsigned int max_alarm_input_linkage_output_object; //default is 8
    unsigned int alarm_linkage_output_object;//1-8, 报警联动输出对象
    unsigned int alarm_input_linkage_output_object[32]; //每个报警联动输入对象，对应最多32个报警联动输出对象。与报警联动输入对象一一对应，每个int的每一位对应一个报警联动输入对象，一个报警联动输入对象可以对应多个报警联动输出对象。
    unsigned int real_time_alarm_output_objcet;  //正在报警的输出联动对象，每一位对应一路输出，1为正在报警，0无报警
    /* unsigned int cancel_linkage_alarm 介绍
    以下均为十进制,本项目取值0-9,其它为保留值
    8：取消报警输出8
    7：取消报警输出7
    6：取消报警输出6
    5：取消报警输出5
    4：取消报警输出4
    3：取消报警输出3
    2：取消报警输出2
    1：取消报警输出1
    0：取消所有报警输入的联动报警输出       */
    unsigned int set_and_cancel_linkage_alarm_channel;
    unsigned int alarm_duration;  //报警持续时间

}alarm_input_output_arg;


void *pthread_alarm_input_output(void *arg);
alarm_input_output_arg alarm_input_output_data;

#endif
