#ifndef _SERIAL_COMMON_
#define _SERIAL_COMMON_


#include <pthread.h>


enum
{
	READ_PORT,
	WRITE_PORT,
	EXEP_PORT,
	OTHER_CHOICE
};


enum
{
   SELECT_PORT_SUCCESS=0,
   SELECT_PORT_ERR = -2,
   SELECT_PORT_TIMEOUT = -1,
   
};

int set_opt(int fd,int nSpeed,int nBits,int nEvent,int nStop);
int OpenDev(char *Dev);
int WriteComPort(int fd, void *data, int datalength);
int ReadComPort(int fd, void *data, int datalength);
int select_port(int fd, int type);

int CloseConsole(void);

int find_dword_in_str(unsigned char *p_src, int len, unsigned int dst_num);
int find_dst_string_in_str(unsigned char *p_src, int len, unsigned char *p_dst, int cmp_len);

#endif

