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

/**************************************************************************
  ���ܣ���ʼ�����ڣ�����ֻ��һ�����ڣ����Բ������ô��ںţ������Դ��ڲ���
  ����������Ҫ�����ñ�������

  ���룺speed�������ʿ�����������
  ���� 0-B57600,
  1-B38400
  2-B19200,
  3-B9600,
  4-B4800,
  5-B2400,
  6-B1200,
  7-B300
  �����أ���ȷ-���ش��ںž��
  ���󣭷��أ�1��ʾ�򿪴���ʧ��
 **************************************************************************/
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
    attr.c_iflag = 0;
    attr.c_oflag = 0;
    attr.c_lflag = 0;
    attr.c_cflag = speed_arr[speed] | CS8 | CLOCAL | CREAD;
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

