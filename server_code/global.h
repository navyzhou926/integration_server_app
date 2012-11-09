#ifndef _GLOABL_H
#define _GLOABL_H

#define YES 1//Z报警自动上传/远程升级中/门禁控制器不在线
#define NO  0//报警手动上传/远程升级结束/报警控制器不在线

#define ENTRANCE_GUARD_UART_DEVICE      UART_DEVICE_ttyS0    //默认COM1 = UART_DEVICE_ttyS0
#define CK2316_UART_DEVICE              UART_DEVICE_ttyS5		    //默认COM6  = UART_DEVICE_ttyS5
#define MATRIX_UART_DEVICE              UART_DEVICE_ttyS1		   //默认COM2 =  UART_DEVICE_ttyS1
#define CRADLE_HEAD_UART_DEVICE         UART_DEVICE_ttyS2	  //默认COM3 = UART_DEVICE_ttyS2

#define SET_IO_DATA_AS_LOW          0
#define SET_IO_DATA_AS_HIGH         1
#define GET_IO_DATA                 3
#define CHANGE_INPUT_OUTPUT_STATUS  4
#define MAX_COMMAND_NUM             4

#define MAX_LED_NUM         4
#define OPEN_LED    SET_IO_DATA_AS_LOW
#define CLOSE_LED   SET_IO_DATA_AS_HIGH


#define UART_DEV_CONFIG_FILE            "dev_config_file.config"

#define MAX_UART_DEV_NUM                7

typedef struct
{
	char Year;				// 年。与2000年的差值，范围00~99。
						// 例如：09，表示2009年，
	char Month;			// 月。范围：一年12个月份
	char Day;				// 日。 根据月份来确定。范围：1~31
						// 2月份需要考虑闰年的天数处理。
	char Hour; 			// 时。范围： 0~23
	char Minute;			// 分。范围： 0~59
	char Second;			// 秒。范围： 0~59
}LAST_ALARM_UPLOAD_TIME;

extern int led_driver_fd;

void init_serial_dev_parameter(void);
int set_serial_dev_parameter(FILE *fp_dev_config_file);
extern int open_LED_light(int led_fd, int led_num);
extern int close_LED_light(int led_fd, int led_num);

#endif
