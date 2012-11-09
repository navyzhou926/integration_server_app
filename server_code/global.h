#ifndef _GLOABL_H
#define _GLOABL_H

#define YES 1//Z�����Զ��ϴ�/Զ��������/�Ž�������������
#define NO  0//�����ֶ��ϴ�/Զ����������/����������������

#define ENTRANCE_GUARD_UART_DEVICE      UART_DEVICE_ttyS0    //Ĭ��COM1 = UART_DEVICE_ttyS0
#define CK2316_UART_DEVICE              UART_DEVICE_ttyS5		    //Ĭ��COM6  = UART_DEVICE_ttyS5
#define MATRIX_UART_DEVICE              UART_DEVICE_ttyS1		   //Ĭ��COM2 =  UART_DEVICE_ttyS1
#define CRADLE_HEAD_UART_DEVICE         UART_DEVICE_ttyS2	  //Ĭ��COM3 = UART_DEVICE_ttyS2

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
	char Year;				// �ꡣ��2000��Ĳ�ֵ����Χ00~99��
						// ���磺09����ʾ2009�꣬
	char Month;			// �¡���Χ��һ��12���·�
	char Day;				// �ա� �����·���ȷ������Χ��1~31
						// 2�·���Ҫ�����������������
	char Hour; 			// ʱ����Χ�� 0~23
	char Minute;			// �֡���Χ�� 0~59
	char Second;			// �롣��Χ�� 0~59
}LAST_ALARM_UPLOAD_TIME;

extern int led_driver_fd;

void init_serial_dev_parameter(void);
int set_serial_dev_parameter(FILE *fp_dev_config_file);
extern int open_LED_light(int led_fd, int led_num);
extern int close_LED_light(int led_fd, int led_num);

#endif
