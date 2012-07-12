#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>

#include <unistd.h>     /*Unix 标准函数定义*/
#include <sys/types.h>  
#include <sys/stat.h>   
#include <fcntl.h>      /*文件控制定义*/
#include <termios.h>    /*PPSIX 终端控制定义*/
#include <errno.h>      /*错误号定义*/
#include <sys/time.h> 

#include "uart.h"

/**************************************************************************
  功能：初始化串口，本机只能一个串口，所以不用设置串口号，如果想对串口操作
  　　　　需要先运用本函数。

  输入：speed－波特率可输入项如下
  　　 0-B57600,
  1-B38400
  2-B19200,
  3-B9600,
  4-B4800,
  5-B2400,
  6-B1200,
  7-B300
  　返回：正确-返回串口号句柄
  错误－返回－1表示打开串口失败
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
  功能：向串口发送数据，运行本函数前要先调用初始化函数
  输入：int Device ：串口的设置号
  unsigned char * DataBuf ：发送数据的数据指针
  int DataLen：发送数据的长度
  　      返回：正确-0
  错误－返回非０
 *************************************************************************/       
int SendDataToCom(int Device ,unsigned char * DataBuf ,int DataLen)
{
    //  ClrCom(Device);
    if (write(Device, &DataBuf[0], DataLen) == DataLen)
        return 0;
    return -1;
}

/********************************************************************
  接收字符串
  int    RecvDataFromCom(int DeviceNo,BYTE *DataBuf,WORD *Len,WORD OverTime);
  功能：
  自串口接收字符串
  运行本函数前要先调用初始化函数
  输入：
DeviceNo:串口句柄号
DataBuf:待接收数据地址
Len:接收字符串个数地址
OverTime:超时时间(单位毫秒)
返回：
0-成功，Len:实际接收字符数
-2-通信错
-1-超时，Len:实际接收字符数
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
  功能：关闭串口
  输入：Device ：串口的设置号
  返回：返回值定义在errno.h中
 *************************************************************************/ 
int CloseCom(int Device)
{
    return(close(Device));
}
/**************************************************************************
  功能：清除串口内容
  输入：Device ：串口的设置号
  返回：返回值定义在errno.h中
 *************************************************************************/ 
int ClrCom(int Device)
{
    tcflush(Device,TCIFLUSH);
    return 0; 
}

