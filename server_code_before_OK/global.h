#ifndef _GLOABL_H
#define _GLOABL_H

#define YES 1//Z�����Զ��ϴ�
#define NO  0//�����ֶ��ϴ�

#define ENTRANCE_GUARD_UART_DEVICE      UART_DEVICE_ttyS1    //Ĭ��COM1 = UART_DEVICE_ttyS0
#define CK2316_UART_DEVICE              UART_DEVICE_ttyS5		    //Ĭ��COM6  = UART_DEVICE_ttyS5
#define MATRIX_UART_DEVICE              UART_DEVICE_ttyS2		   //Ĭ��COM2 =  UART_DEVICE_ttyS1
#define CRADLE_HEAD_UART_DEVICE         UART_DEVICE_ttyS3 	  //Ĭ��COM3 = UART_DEVICE_ttyS2

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

void init_serial_dev_parameter(void);
int set_serial_dev_parameter(FILE *fp_dev_config_file);

#endif
