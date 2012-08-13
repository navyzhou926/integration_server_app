/*************************************************************************************
 * ��Ȩ:    2008-2009  ������Ѷ�������޹�˾ 
 * �ļ���:       
 * ����:         
 * ����:    
 * �汾��:        
 * ��Ҫ����:   
 *     1.
 * �޸���ʷ:    
 *    ��������   �����ڡ�      ���޸����ݡ�
*************************************************************************************/
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
    
#include <termios.h>    /*PPSIX �ն˿��ƶ���*/
#include <errno.h>      /*����Ŷ���*/
#include <string.h>
#include <sys/ioctl.h>
#include <fcntl.h>      /*�ļ����ƶ���*/
    
#include <string.h>

#include "serial_common.h"

typedef unsigned int U32;

/***@brief  ���ô���ͨ������
*param  fd     ���� int  �򿪴��ڵ��ļ����
*param  speed  ���� int  �����ٶȣ�������
*param nEvent :��żУ�����ã�0:��У�飬1:��У�飬2:żУ��
*param nStop : ֹͣλ������Ϊ0������1
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
        case 1:   //��У�顣  // odd
            newtio.c_cflag |= PARENB;
            newtio.c_cflag |= PARODD;
            //newtio.c_iflag |= (INPCK|ISTRIP);
            newtio.c_iflag |= (INPCK);
            break;
        case 2:   //żУ�顣  // even
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

    //ֹͣλ��1��2��
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
 * ����:        
 * ������:      �򿪴����豸       
 * ����:      
 * ���ݷ���: 
 * ���ݸ���:  
 * ����:          
 * ���:         
 * ����ֵ:         
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
 * ����:                 ����ĳһ�������������Ƿ���ʱ�����
                                 ʱ������ȣ���1ms,������OXFORD�Ļ�����
                                 ����ʱ�������10ms <����ӡ>��ѭ��500��
                                 �Ľ��
 * ������:          
 * ����:      
 * ���ݷ���: 
 * ���ݸ���:  
 * ����:          
 * ���:         
 * ����ֵ:         
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
 * ����:               �Ӵ����ж�ȡ����
 * ������:          
 * ����:      
 * ���ݷ���: 
 * ���ݸ���:  
 * ����:          
 * ���:         
 * ����ֵ:         
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
 * ����:               ��˿���д����
 * ������:          
 * ����:      
 * ���ݷ���: 
 * ���ݸ���:  
 * ����:          
 * ���:         
 * ����ֵ:         
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
 * ����:             ̽�⣬�鿴�����Ƿ�ɶ���д�����쳣
 * ������:          
 * ����:      
 * ���ݷ���: 
 * ���ݸ���:  
 * ����:          
 * ���:         
 * ����ֵ:         
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
 * ����:        
 * ������:          ��һ���ַ����У�����һ���ֵ�λ�ã�����ƫ�Ƶ�ַ
 * ����:      
 * ���ݷ���: 
 * ���ݸ���:  
 * ����:          
 * ���:         
 * ����ֵ:         
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
 * ����:                       �ض����ն�
 * ������:          
 * ����:      
 * ���ݷ���: 
 * ���ݸ���:  
 * ����:          
 * ���:         
 * ����ֵ:         
 *******************************************************************************/
int CloseConsole(void)
{ 
    int fp;
    //struct termios options;
    printf("Opening console...");
    fp = open("/dev/console",O_RDONLY); // �ı�console
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
 * ����:          ��һ���ַ����У��ҵ�һ�����ʵ�32λ����
 * ������:          
 * ����:      
 * ���ݷ���: 
 * ���ݸ���:  
 * ����:          
 * ���:         
 * ����ֵ:         
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
 * ����:        ��Ŀ���ַ����У��ҵ�һ�����ַ������൱��
                       strstr===����strstr�о��ޣ�������strlen�������ǵ�
                       �����У�ÿ���ַ������ж��0--��Ҫע�����
                       oxford�������κ�short��ȡֵ�����⣬����Ϊ�ֻ���
                       ˫�ֶ��롣���Բ��ܲ���һ��intһ��int�Ŀ��ٱȽ�
                       �㷨- ������һ��charһ��char�Ƚ�
                          
 * ������:          
 * ����:      
 * ���ݷ���: 
 * ���ݸ���:  
 * ����:          
 * ���:         
 * ����ֵ:         
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

