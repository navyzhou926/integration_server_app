/*************************************************************************************
 * 版权:    2008-2009  北京声讯电子有限公司 
 * 文件名:       
 * 作者:         
 * 描述:    
 * 版本号:        
 * 主要功能:   
 *     1.
 * 修改历史:    
 *    【姓名】   【日期】      【修改内容】
*************************************************************************************/
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
    
#include <termios.h>    /*PPSIX 终端控制定义*/
#include <errno.h>      /*错误号定义*/
#include <string.h>
#include <sys/ioctl.h>
#include <fcntl.h>      /*文件控制定义*/
    
#include <string.h>

#include "serial_common.h"

typedef unsigned int U32;

/***@brief  设置串口通信速率
*param  fd     类型 int  打开串口的文件句柄
*param  speed  类型 int  串口速度，波特率
*param nEvent :奇偶校验设置，0:无校验，1:奇校验，2:偶校验
*param nStop : 停止位，设置为0，或者1
*return  void*/
int set_opt(int fd,int nSpeed,int nBits,int nEvent,int nStop)
{
    struct termios newtio,oldtio;

    if(tcgetattr(fd,&oldtio))
    {
        printf("tcgetattr in set_opt");
        return -1;
    }

    bzero(&newtio,sizeof(newtio));
    newtio.c_cflag |= CLOCAL|CREAD;
    newtio.c_cflag &= ~CSIZE;

    //newtio.c_oflag |= OPOST;

    switch(nBits)
    {
        case 0:
            newtio.c_cflag |= CS5;
            break;
        case 1:
            newtio.c_cflag |= CS6;
            break;
        case 2:
            newtio.c_cflag |= CS7;
            break;
        case 3:
            newtio.c_cflag |= CS8;
            break;
        default:
            break;
    }

    switch(nEvent)
    {
        case 0:
            newtio.c_cflag &= ~PARENB;
            break;
        case 1:   //奇校验。  // odd
            newtio.c_cflag |= PARENB;
            newtio.c_cflag |= PARODD;
            //newtio.c_iflag |= (INPCK|ISTRIP);
            newtio.c_iflag |= (INPCK);
            break;
        case 2:   //偶校验。  // even
            newtio.c_cflag |= PARENB;
            newtio.c_cflag &= ~PARODD;
            //newtio.c_iflag |= (INPCK|ISTRIP);
            newtio.c_iflag |= (INPCK);
            break;
    }
    //newtio.c_iflag |= (IGNPAR);
    //newtio.c_iflag |= (PARMRK | IGNPAR);
    //newtio.c_iflag |= (PARMRK);

    switch(nSpeed)
    {
        case 1200:
            cfsetispeed(&newtio,B1200);
            cfsetospeed(&newtio,B1200);
            break;
        case 2400:
            cfsetispeed(&newtio,B2400);
            cfsetospeed(&newtio,B2400);
            break;
        case 4800:
            cfsetispeed(&newtio,B4800);
            cfsetospeed(&newtio,B4800);
            break;
        case 9600:
            cfsetispeed(&newtio,B9600);
            cfsetospeed(&newtio,B9600);
            break;
        case 19200:
            cfsetispeed(&newtio,B19200);
            cfsetospeed(&newtio,B19200);
            break;
        case 38400:
            cfsetispeed(&newtio,B38400);
            cfsetospeed(&newtio,B38400);
            break;
        case 57600:
            cfsetispeed(&newtio,B57600);
            cfsetospeed(&newtio,B57600);
            break;
        case 115200:
            cfsetispeed(&newtio,B115200);
            cfsetospeed(&newtio,B115200);
            break;
        default:
            break;
    }

    //停止位：1，2。
    if(nStop==0)
    {
        newtio.c_cflag &=~CSTOPB;
    }
    else if(nStop==1) 
    {
        newtio.c_cflag |=CSTOPB;
    }


    //navy add
    //newtio.c_oflag &= ~OPOST;
    //newtio.c_lflag &= ~(ICANON|ECHO|ECHOE|ISIG);
    //tcflush(fd, TCIOFLUSH);  //navy add
    //end navy

    newtio.c_cc[VTIME]=0;
    newtio.c_cc[VMIN]=0;

    if(0!=(tcsetattr(fd,TCSANOW,&newtio)))
    {
        printf("com set error\n");
        return -1;
    }
    return 0;
}

/*******************************************************************************
 * 功能:        
 * 调用者:      打开串口设备       
 * 调用:      
 * 数据访问: 
 * 数据更新:  
 * 输入:          
 * 输出:         
 * 返回值:         
 *******************************************************************************/
int OpenDev(char *Dev)
{
    int fd;

    //fd = open( Dev, O_RDWR|O_NOCTTY|O_NONBLOCK );         //| O_NOCTTY | O_NDELAY   
    fd = open( Dev, O_RDWR|O_NOCTTY);         //| O_NOCTTY | O_NDELAY   
    if (-1 == fd)      
    {                             
        printf("Can't Open Serial Port\n");
        return -1;                    
    }        
    else     
        return fd;
}


/*******************************************************************************
 * 功能:                 看看某一个串口描述符是否有时间产生
                                 时间颗粒度，是1ms,但是在OXFORD的机器上
                                 测试时，结果是10ms <看打印>，循环500次
                                 的结果
 * 调用者:          
 * 调用:      
 * 数据访问: 
 * 数据更新:  
 * 输入:          
 * 输出:         
 * 返回值:         
 *******************************************************************************/
int select_port(int fd, int type)
{
    fd_set   fs_set;
    struct timeval tv_timeout;
    int retval;

    FD_ZERO (&fs_set);
    FD_SET (fd, &fs_set);
    tv_timeout.tv_sec = 0;
    tv_timeout.tv_usec = 500000;
		
	if (type == READ_PORT)
	{
		retval = select (fd + 1, &fs_set, NULL, NULL, &tv_timeout);
	}	
	else if (WRITE_PORT == type)
	{
	  retval = select (fd + 1, NULL, &fs_set, NULL, &tv_timeout);
	}
    
	if (retval < 0)	
	{
        printf("select..\n");
        return SELECT_PORT_ERR;
	}
	else if (retval == 0)//time out
	{
	    //printf("select time out..\n");
        return SELECT_PORT_TIMEOUT;
	}
    else
    {       
		retval = FD_ISSET(fd, &fs_set);
		if (retval > 0)
		{
			//printf("select success..\n");
			return SELECT_PORT_SUCCESS;
		}
		else
		{
			return SELECT_PORT_TIMEOUT;    
		}
    }
		
	return SELECT_PORT_SUCCESS;
}


/*******************************************************************************
 * 功能:               从串口中读取数据
 * 调用者:          
 * 调用:      
 * 数据访问: 
 * 数据更新:  
 * 输入:          
 * 输出:         
 * 返回值:         
 *******************************************************************************/


int ReadComPort(int fd, void *data, int datalength)
{
    int RxCount,nread=0;
    unsigned char *p_read;
    int ret=0;
    errno = 0;		   
    	
    RxCount = datalength;	
    p_read = (unsigned char *)data;
    
    while(RxCount>0)
    {  
        ret = select_port(fd, READ_PORT);
        if(SELECT_PORT_SUCCESS==ret)
        {
            if ((nread = read(fd, p_read, RxCount))>0)
            {
                RxCount -= nread;
                p_read += nread;
            }
             
            else
            {
                printf("read error:nread:%d.\n", nread);
                return -1;
            }   

        }
        else
        {
            //printf("read select err:%d.\n", ret);
            return ret;
        }
        	    	
    }	

    return 0;

}


/*******************************************************************************
 * 功能:               向端口中写数据
 * 调用者:          
 * 调用:      
 * 数据访问: 
 * 数据更新:  
 * 输入:          
 * 输出:         
 * 返回值:         
 *******************************************************************************/
 
int WriteComPort(int fd, void *data, int datalength)
{
    int write_byte, write_size;
    unsigned char *p_write;

    errno = 0;		   

    write_size = datalength;	
    p_write = (unsigned char *)data;
		
    while(write_size>0)
    {  
    	 while(select_port(fd, WRITE_PORT));//
    	 
		  if ((write_byte = write(fd, p_write, write_size))>0)
		  {
				write_size -= write_byte;
				p_write += write_byte;	
//				printf("write :%d byte success \n", write_byte);	
		  }
		
		 else
		 {
			printf("write error:write_byte:%d\n", write_byte);
			return -1;
		 }		    	
	}	

    return 0;
}
/*******************************************************************************
 * 功能:             探测，查看串口是否可读、写或者异常
 * 调用者:          
 * 调用:      
 * 数据访问: 
 * 数据更新:  
 * 输入:          
 * 输出:         
 * 返回值:         
 *******************************************************************************/
int serial_probe(int fd, int type, struct timeval time_out)
{
    fd_set   fs_set;
    struct timeval tv_timeout;
    int retval;

    FD_ZERO (&fs_set);
    FD_SET (fd, &fs_set);
    tv_timeout.tv_sec = time_out.tv_sec;
    tv_timeout.tv_usec = time_out.tv_usec;
    
	if (type == READ_PORT)
	{
		retval = select (fd + 1, &fs_set, NULL, NULL, &tv_timeout);
	}	
	else if (WRITE_PORT == type)
	{
	  retval = select (fd + 1, NULL, &fs_set, NULL, &tv_timeout);
	}
	if (retval < 0)	
	{
        printf("select..\n");
        return -2;
	}
	else if (retval == 0)
	{
        return -1;
	}
    else
    {       
		retval = FD_ISSET(fd, &fs_set);
		if (retval > 0)
		{
			//printf("select success..\n");
			return 0;
		}
		else
		{
			return -1;    
		}
    }
		
	return 0;
}


/*******************************************************************************
 * 功能:        
 * 调用者:          从一个字符串中，查找一个字的位置，返回偏移地址
 * 调用:      
 * 数据访问: 
 * 数据更新:  
 * 输入:          
 * 输出:         
 * 返回值:         
 *******************************************************************************/
int word_position_find(unsigned char *p_src, int len, U32 dst_num)
{
    int i;
    unsigned char *p_tmp = p_src;
    
    if (NULL == p_src)
    {
        printf("Err:find contxt in str:invalid para....\n");
        return len;
    }

    for (i = 0; i < len && *(U32*)p_tmp != dst_num; i++, p_tmp++);
    
    if (i>=len)
    {
        return len;
    }

    return (p_tmp - p_src);
    
}


/*******************************************************************************
 * 功能:                       重定向终端
 * 调用者:          
 * 调用:      
 * 数据访问: 
 * 数据更新:  
 * 输入:          
 * 输出:         
 * 返回值:         
 *******************************************************************************/
int CloseConsole(void)
{ 
    int fp;
    //struct termios options;
    printf("Opening console...");
    fp = open("/dev/console",O_RDONLY); // 改变console
    if(fp==-1)
    {
        printf("open..\n");
        return -1;
    }
    printf("Success...\n");
    ioctl(fp,TIOCCONS);
    close(fp);

    return 0;
}


/*******************************************************************************
 * 功能:          在一个字符串中，找到一个合适的32位整数
 * 调用者:          
 * 调用:      
 * 数据访问: 
 * 数据更新:  
 * 输入:          
 * 输出:         
 * 返回值:         
 *******************************************************************************/
int find_dword_in_str(unsigned char *p_src, int len, unsigned int dst_num)
{
    int i;
    unsigned char *p_tmp = p_src;
    
    if (NULL == p_src)
    {
        printf("Err:find contxt in str:invalid para....\n");
        return len;
    }

    for (i = 0; i < len && *(U32*)p_tmp != dst_num; i++, p_tmp++);
    
    if (i>=len)
    {
        printf("Warning didn't find obj nun...\n");

        return len;
    }

    return (p_tmp - p_src);
    
}

/*******************************************************************************
 * 功能:        在目的字符串中，找到一个子字符串，相当于
                       strstr===但是strstr有局限，依赖于strlen。而我们的
                       需求中，每个字符串中有多个0--需要注意的是
                       oxford对于整形和short的取值有问题，必须为字或者
                       双字对齐。所以不能采用一个int一个int的快速比较
                       算法- 而必须一个char一个char比较
                          
 * 调用者:          
 * 调用:      
 * 数据访问: 
 * 数据更新:  
 * 输入:          
 * 输出:         
 * 返回值:         
 *******************************************************************************/
int find_dst_string_in_str(unsigned char *p_src, int len, unsigned char *p_dst, int cmp_len)
{
	int i, j;
	
	for (i = 0; i <= len - cmp_len; i++)
	{
		for (j = 0; j < cmp_len; j++)
		{
			if (p_src[i + j] != p_dst[j])
				break;
		}
		
		if (j == cmp_len)
		{
			return i;
		}
	}

    if (i >= len - cmp_len)
    {
        i = len;
    }
	return i;
}

