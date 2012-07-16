#ifndef __UART_H__
#define __UART_H__

#define UART_DEVICE_ttyUSB0 "/dev/ttyUSB0"
#define UART_DEVICE_ttyS0 "/dev/ttyS0"
#define UART_DEVICE_ttyS1 "/dev/ttyS1"
#define UART_DEVICE_ttyS2 "/dev/ttyS2"
#define UART_DEVICE_ttyS3 "/dev/ttyS3"
#define UART_DEVICE_ttyS4 "/dev/ttyS4"
#define UART_DEVICE_ttyS5 "/dev/ttyS5"
#define UART_DEVICE_ttyS6 "/dev/ttyS6"

#define RECV_TIMEOUT  2000  //1.8 seconds, the units is ms
#define RECV_OVER_TIME 400000


//0 115200
//1 57600
//2 38400
//3 19200     
//4 9600
//5 4800
//6 2400
//7 1200
//8 300
#define BOARD_RATE 4

/*int InitCom(int speed);*/
int InitCom(char *UART_DEVICE_ttySx, int speed);
int SendDataToCom(int Device ,unsigned char * DataBuf ,int DataLen);
//int RecvDataFromCom(int DeviceNo,unsigned char *DataBuf,unsigned int *Len,unsigned int OverTime);
int RecvDataFromCom(int DeviceNo, unsigned char *DataBuf, unsigned int *Len, unsigned int OverTime, unsigned int recv_over_time);
int CloseCom(int Device);
int ClrCom(int Device);


#endif
