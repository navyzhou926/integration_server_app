#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>     /*Unix ��׼��������*/
#include <sys/types.h>  
#include <sys/stat.h>   
#include <fcntl.h>      /*�ļ����ƶ���*/
#include <termios.h>    /*PPSIX �ն˿��ƶ���*/
#include <errno.h>      /*����Ŷ���*/
#include <sys/time.h> 
#include "uart.h"
#include "ck2316_alarm.h"

extern unsigned char ck2316_keyboard_address_code[];
extern unsigned char ck2316_actual_keyboard_address_code[];
extern unsigned char ck2316_handshake_code[];

/*

*/
int InitCom(char *UART_DEVICE_ttySx, int speed)
{
    int fd ;
    //int int_ret;
    struct termios attr;
    //unsigned char buf[20];
    int speed_arr[] ={B115200,B57600,B38400,B19200,B9600,B4800,B2400,B1200,B300};

    //fd = open(UART_DEVICE_ttyS1, O_RDWR| O_NOCTTY | O_NDELAY);
    fd = open(UART_DEVICE_ttySx, O_RDWR| O_NOCTTY | O_NDELAY);
    //open uart
    if (fd==-1)
    {
/*#ifdef DEBUG  
          ScrCls();
          ScrPrintStr("open uart error\n");
          getkey(); 
#endif*/
        close(fd);

        return(-1);
    }
    // set uart attribution
    tcgetattr(fd, &attr);
    attr.c_cc[VMIN] = 0;
    attr.c_cc[VTIME] = 15; 
    attr.c_iflag = 0; //input options
    attr.c_oflag = 0; //output options
    attr.c_lflag = 0; //local mode flags
    attr.c_cflag = speed_arr[speed] | CS8 | CLOCAL | CREAD; //control options
    tcsetattr(fd, TCSANOW, &attr);  
    return (fd);
}

//2400bps 8E1
int InitCom_ck2316(char *UART_DEVICE_ttySx, int speed)
{
    int fd ;
    //int int_ret;
    struct termios attr;
    //unsigned char buf[20];
    int speed_arr[] ={B115200,B57600,B38400,B19200,B9600,B4800,B2400,B1200,B300};

    fd = open(UART_DEVICE_ttySx, O_RDWR| O_NOCTTY | O_NDELAY);
    if (fd==-1)
    {
        close(fd);
        return(-1);
    }
    // set uart attribution
    tcgetattr(fd, &attr);
    attr.c_cc[VMIN] = 0;
    attr.c_cc[VTIME] = 15; 
    attr.c_iflag = 0; //input options
    attr.c_oflag = 0; //output options
    attr.c_lflag = 0; //local mode flags
    attr.c_cflag = speed_arr[speed] | CS8 | CLOCAL | CREAD; //control options

    //navy 20120806 add 
    #if 1
    #if 1 //even
    attr.c_cflag |= PARENB;
    attr.c_cflag &= ~PARODD;
    //attr.c_iflag |= (INPCK | ISTRIP); //input options
    attr.c_iflag |= (INPCK); //input options

    #else //odd
    attr.c_cflag |= PARENB;
    attr.c_cflag |= PARODD;
    //attr.c_iflag |= (INPCK | ISTRIP); //input options
    attr.c_iflag |= (INPCK); //input options
    #endif

    attr.c_cflag &= ~CSTOPB;  //stop bit 1
    //attr.c_cflag &= ~CSIZE;  //
    #endif
    //end navy

    tcsetattr(fd, TCSANOW, &attr);  
    return (fd);
}

/**************************************************************************
  ���ܣ��򴮿ڷ������ݣ����б�����ǰҪ�ȵ��ó�ʼ������
  ���룺int Device �����ڵ����ú�
  unsigned char * DataBuf ���������ݵ�����ָ��
  int DataLen���������ݵĳ���
  ��      ���أ���ȷ-0
  ���󣭷��طǣ�
 *************************************************************************/       
int SendDataToCom(int Device ,unsigned char * DataBuf ,int DataLen)
{
    //  ClrCom(Device);
    if (write(Device, &DataBuf[0], DataLen) == DataLen)
        return 0;
    return -1;
}

/********************************************************************
  �����ַ���
  int    RecvDataFromCom(int DeviceNo,BYTE *DataBuf,WORD *Len,WORD OverTime);
  ���ܣ�
  �Դ��ڽ����ַ���
  ���б�����ǰҪ�ȵ��ó�ʼ������
  ���룺
DeviceNo:���ھ����
DataBuf:���������ݵ�ַ
Len:�����ַ���������ַ
OverTime:��ʱʱ��(��λ����)
���أ�
0-�ɹ���Len:ʵ�ʽ����ַ���
-2-ͨ�Ŵ�
-1-��ʱ��Len:ʵ�ʽ����ַ���
 ********************************************************************/

int RecvDataFromCom(int DeviceNo, unsigned char *DataBuf, unsigned int *Len, unsigned int OverTime, unsigned int recv_over_time)
{
    fd_set fds;
    struct timeval tv;
    unsigned char Ret,Chr;
    unsigned int Num;

    FD_ZERO(&fds);
    FD_SET(DeviceNo,&fds);
    tv.tv_sec = OverTime/1000;
    tv.tv_usec = (unsigned int)(OverTime%1000)*1000;
    if(select(DeviceNo+1,&fds,NULL,NULL,&tv)<=0)
    {
        *Len=0;
        return -1;
    }
    tv.tv_sec = 0;
    //tv.tv_usec = 300000;
    tv.tv_usec = recv_over_time;

    Num=*Len;
    while(Num)
    {
        if(select(DeviceNo+1,&fds,NULL,NULL,&tv)<=0)
            break;
        Ret = read(DeviceNo,&Chr,1);
        if(Ret!=1)
            return -2;
        #if 0
        if (Chr == '\r') 
        {
            ClrCom(DeviceNo);
            goto out;
        }
        #endif
        *DataBuf++=Chr;
        Num--;
    }
//out:
    *Len-=Num;
    if(!Num)
        return 0;
    return -1;
}

#if 1
int read_uart_data(int fd, unsigned char *data_buf, int len, unsigned int recv_timeout) 
{
    int ret = 0;
    int i = 0;
    unsigned char ch;
    fd_set fds;
    struct timeval tv;

    tv.tv_sec = recv_timeout;
    tv.tv_usec = 0;
    //while (i < (len-1)) 
    while (i < len) 
    {
        FD_ZERO(&fds);
        FD_SET(fd,&fds);
        if((ret = select(fd+1,&fds,NULL,NULL,&tv)) > 0)
        {
            if (read(fd, &ch, 1) > 0) 
            {
                data_buf[i] = ch;
                if (i > 0) 
                {
                    if (ck2316_alarm_data.if_ck2316_alive == 1)//normal handshake 
                    {
                        //if ((data_buf[i-1] == ck2316_actual_keyboard_address_code[0]) && (data_buf[i] == ck2316_keyboard_address_code[1])) 
                        if ((data_buf[i-1] == ck2316_alarm_data.ck2316_simulate_keyboard_address) && (data_buf[i] == ck2316_keyboard_address_code[1])) 
                        {
                            //return i + 1;
                            // navy ����04 03���FE 04 03������������
                            //if (ck2316_alarm_data.ck2316_simulate_keyboard_address != 0x04) 
                            if (ck2316_alarm_data.ck2316_simulate_keyboard_address != 0x04 && ck2316_alarm_data.ck2316_simulate_keyboard_address != 0x05) 
                            {
                                //printf("1\n");
                                return i + 1;
                            }
                            else
                            {
                                if (i > 1) 
                                {
                                    if (data_buf[i-2] != 0xFE) 
                                    {
                                        //printf("2\n");
                                        return i + 1;
                                    }
                                }
                                else
                                {
                                    //printf("3\n");
                                    return i + 1;
                                }
                            }
                            //end navy
                        }
                    }
                    else  //search keyboard
                    {
                        //if (((data_buf[i-1] == ck2316_actual_keyboard_address_code[0]) || (data_buf[i-1] == ck2316_keyboard_address_code[0])) && (data_buf[i] == ck2316_keyboard_address_code[1])) 
                        if (((data_buf[i-1] <= ck2316_actual_keyboard_address_code[0]) && (data_buf[i-1] >= ck2316_keyboard_address_code[0])) && (data_buf[i] == ck2316_keyboard_address_code[1])) 
                        {
                            if (if_search_keyboard == NO) 
                            {
                                ck2316_alarm_data.ck2316_simulate_keyboard_address = data_buf[i-1];
                            }
                            // navy ����04 03/05 03���FE 04 03������������
                            if (ck2316_alarm_data.ck2316_simulate_keyboard_address != 0x04) 
                            //if (ck2316_alarm_data.ck2316_simulate_keyboard_address != 0x04 && ck2316_alarm_data.ck2316_simulate_keyboard_address != 0x05) 
                            {
                                if (ck2316_alarm_data.ck2316_simulate_keyboard_address == 0x05) 
                                {
                                    if (i > 1) 
                                    {
                                        if ((0xF0 != (data_buf[i-2] & 0xF0)) && data_buf[i-2] != 0x00) 
                                        {
                                            //printf("5\n");
                                            //ck2316_alarm_data.if_ck2316_alive = 1; 
                                            return i + 1;
                                        }
                                    }
                                    else
                                    {
                                        //printf("6\n");
                                        return i + 1;
                                    }
                                }
                                else
                                {
                                    //printf("4\n");
                                    return i + 1;
                                }
                            }
                            else
                            {
                                if (i > 1) 
                                {
                                    if (data_buf[i-2] != 0xFE) 
                                    {
                                        //printf("5\n");
                                        //ck2316_alarm_data.if_ck2316_alive = 1; 
                                        return i + 1;
                                    }
                                }
                                else
                                {
                                    //printf("6\n");
                                    return i + 1;
                                }
                            }
                            //end navy
                        }
                    }
                    //if ((data_buf[i-1] == ck2316_actual_keyboard_address_code[0]) && (data_buf[i] == ck2316_keyboard_address_code[1])) 

                }
                i++;
            }
            else
            {
                return -2;
            }
        }
        else if(ret == 0)
        {
            return -1;
        }
        else
        {
            return -2;
        }
    }

    return 0;
}
#else
int read_uart_data(int fd, unsigned char *data_buf, int len) 
{
    int ret = 0;
    int i = 0;
    unsigned char ch;
    //unsigned char temp[] = {0xfe, 0x04, 0x03};
    fd_set fds;
    struct timeval tv;

    //while (read(fd, &ch, 1) > 0) 
    while (1)
    {
        if (read(fd, &ch, 1) > 0) 
        {
            data_buf[i++] = ch;
            if (i > 1) 
            {
                if (i > len) 
                {
                    //i = 0;
                    return i;
                }
                if (((data_buf[i-2] == ck2316_actual_keyboard_address_code[0]) || (data_buf[i-2] == ck2316_keyboard_address_code[0])) && (data_buf[i-1] == ck2316_keyboard_address_code[1])) 
                //if ((data_buf[i-2] == ck2316_keyboard_address_code[0]) && (data_buf[i-1] == ck2316_keyboard_address_code[1])) 
                //if ((data_buf[i-1] == ck2316_keyboard_address_code[1])) 
                {
                    //tcflush(fd,TCIOFLUSH);
                    //write(fd, temp, 2);
                    //SendDataToCom(fd, ck2316_handshake_code, 3);
                    return i;
                }
            }
        }
    }
    return 0;
    //while (i < (len-1)) 
    //while (i < len) 
    {
        FD_ZERO(&fds);
        FD_SET(fd,&fds);
        tv.tv_sec = 2;
        tv.tv_usec = 0;
        if((ret = select(fd+1,&fds,NULL,NULL,&tv)) > 0)
        {
            if (read(fd, data_buf, 2) > 0) 
            {
                //if (((data_buf[i-1] == 0x0B) || (data_buf[i-1] == ck2316_keyboard_address_code[0])) && (data_buf[i] == ck2316_keyboard_address_code[1])) 
                if ((data_buf[0] == ck2316_keyboard_address_code[0]) && (data_buf[1] == ck2316_keyboard_address_code[1])) 
                //if ((data_buf[i] == ck2316_keyboard_address_code[1])) 
                {
                    //tcflush(fd,TCIOFLUSH);
                    //write(fd, temp, 2);
                    //SendDataToCom(fd, ck2316_handshake_code, 3);
                    return 2;
                }
            }
            else
            {
                return -2;
            }
        }
        else if(ret == 0)
        {
            return -1;
        }
        else
        {
            return -2;
        }
    }

    return  0;
}
#endif

/**************************************************************************
  ���ܣ��رմ���
  ���룺Device �����ڵ����ú�
  ���أ�����ֵ������errno.h��
 *************************************************************************/ 
int CloseCom(int Device)
{
    return(close(Device));
}
/**************************************************************************
  ���ܣ������������
  ���룺Device �����ڵ����ú�
  ���أ�����ֵ������errno.h��
 *************************************************************************/ 
int ClrCom(int Device)
{
    tcflush(Device,TCIFLUSH);
    return 0; 
}
