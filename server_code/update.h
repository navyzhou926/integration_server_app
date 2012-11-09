/*****************************************************
file name   : main.c
func        : 
include     :
date        : 2010-03-23
auto        : xx
*****************************************************/
typedef int DWORD;
typedef char BYTE;

/*
升级包头 ： 
*/

typedef enum tag_protocal_type 
{
	UPDATE_REQUEST, //升级请求
	UPDATE_START,    //升级开始
	UPDATE_DATA,     //升级数据
	UPDATE_CTL,      //升级控制
	UPDATE_REFRESH_PROGE,      //升级进度
	UPDATE_STATE_UP,      //升级状态上传
	UPDATE_STATE_GET,      //升级状态获取
	
	MAX_UPDATE_TYPE
}E_UPDATE_PROTOCOL_TYPE;

typedef struct tag_update_protocol_head
{
	BYTE byType;           //类型
	BYTE byRsv[3];         //填充
	DWORD dwResult;        //指令处理结果，设备反馈时使用，具体参见错误码
	DWORD dwLen;           //数据长度
}T_UPDATE_PROTOCOL_HEAD, *LP_T_UPDATE_PROTOCOL_HEAD;


/*
升级请求 ： 参数：SX_DEVICE_VERSION_INFO
					
SDK—Device不携带参数
Device- SDK—携带参数：
*/

/*
设备类型 ： 
*/
typedef enum tag_device_type
{
    DEVICE_DVS, //DVS
    DEVICE_EVR, //EVR   
    DEVICE_AV,  //视频报警控制器

    MAX_DEVICE_TYPE
}E_DEVICE_TYPE;

/*
视频报警控制器子程序类型 ： 
*/

typedef enum tag_sx_av_packet_type
{
//主控单元
	PACKET_MAIN_UNIT_TYPE_START = 0, //填充提示作用
	PROG_MAIN_UNIT, //主程序
	//------后续新增需求，在此增加--<比如说单独的库形式>
	PACKET_MAIN_UNIT_TYPE_END, //填充提示作用
	
//报警单元
	PACKET_ALARM_UNIT_TYPE_START = 0xE0, //填充提示作用
	PROG_ALARM_UNIT,  //主程序
	PACKET_ALARM_UNIT_TYPE_END, //填充提示作用
	
}SX_AV_PACKET_TYPE;

typedef struct tag_sub_prog_attr
{
	BYTE byType;            //类型--SX_AV_PACKET_TYPE
	BYTE byRsv[3];          //字节对齐保留字
	DWORD dwTime;           //子程序生成时间-time_t	
	DWORD dwSize;           //子程序大小	
	DWORD dwVersion;  	    //版本号
}SX_SUB_PROG_ATTR_INFO, *LP_SX_SUB_PROG_ATTR_INFO;

typedef struct tag_previous_update_state
{ 	
	BYTE dwPausedFlag;    //升级中断标示
	BYTE dwRsv[3];         //保留字-补齐用
	DWORD dwFileID;        //中断升级文件的Id号，用于匹配 
	DWORD dwTime;          //中断升级文件的生成日期，用于匹配
	DWORD dwUpdatedSize;  //已经接收到的文件的大小
}SX_PREVIOUS_UPDATE_STATE, *LP_SX_PREVIOUS_UPDATE_STATE;

/*
 typedef struct tag_device_version_info
{ 	
	DWORD dwDeviceType;  	//设备类型
	DWORD dwHardVersion;    //硬件版本
	DWORD dwSoftVersion;    //软件版本
	
	SX_PREVIOUS_UPDATE_STATE tPreviousState;  	
	DWORD dwSubProgSum;  	//设备当前有多少个子程序
	
	//可变长度库属性信息 dwLibSum * sizeof(SX_SUB_PROG_ATTR_INFO)
}SX_DEVICE_VERSION_INFO, *LP_SX_DEVICE_LIB_INFO;
*/


/*
开始升级
							参数：SX_UPDATE_ATTR
SDK—Device： 携带参数
Device- SDK—  不携带参数
*/

/*
升级类型 ： 
*/
typedef enum tag_update_type
{
    RESET_UPDATE, //重新传输
    IMPLEMENT_UPDATE, //断点传输

    MAX_UPDATE_TYPE_DUMMY
}E_UPDATE_TYPE;


typedef struct tag_update_file_cxt
{
	DWORD dwFileID;       //此升级包的ID号，随机数，用户断点传输
	DWORD dwTime;         //升级文件的日期，用户断点传输
	DWORD dwSize;            //升级数据的大小
	DWORD dwSubProgSum;     //子程序的数目
	//后面子程序的内容—大小为dwSize  * sizeof(SX_SUB_PROG_ATTR_INFO)
}SX_UPDATE_FILE_CXT, *LP_SX_UPDATE_FILE_CXT;


typedef struct tag_update_attr
{
	BYTE byType;            //类型---E_UPDATE_TYPE
	BYTE byRsv[3];          //字节对齐保留字

	SX_UPDATE_FILE_CXT  tFileCxt;//如果为重新升级的模式，则此结构有效

}SX_UPDATE_ATTR, *LP_SX_UPDATE_ATTR;


/*
发送数据
参数：SX_UPDATE_DATA
SDK—Device： 携带参数
Device- SDK—  不携带参数
*/

typedef struct tag_update_date
{
	DWORD dwPackId;   //此数据包的编号，从1开始 
	DWORD dwPackLen;  //此数据包的长度
	
//数据包内容, 长度为 dwPackLen

}SX_UPDATE_DATA, *LP_SX_UPDATE_DATA;

/*
升级控制
参数：SX_UPDATE_CTL
*/

typedef enum tag_update_ctl_type
{
    CANCELED_UPDATE,  //取消升级
    PAUSED_TRANSFER,  //暂停传输

    MAX_UPDATE_CTL_TYPE
}E_UPDATE_CTL_TYPE;

typedef struct tag_update_ctl
{
	DWORD dwPackNum;   //此数据包的编号，从1开始 
	
}SX_UPDATE_CTL, *LP_SX_UPDATE_CTL;

/*
升级进度
参数：SX_UPDATE_PROGRESS
*/

typedef struct tag_update_progress
{
	DWORD dwRefreshSize;   //更新的字节数
	BYTE  byPct;             //更新的百分比
	BYTE  byRsv[3];          //保留

}SX_UPDATE_PROGRESS, *LP_SX_UPDATE_PROGRESS;


/*
升级状态上传
参数：SX_UPDATE_STATE
SDK—Device： 不携带参数
Device- SDK—  携带参数
*/

typedef struct tag_update_state
{
	DWORD dwState;   
}SX_UPDATE_STATE, *LP_SX_UPDATE_STATE;


/*
升级状态获取
参数：SX_UPDATE_STATE 
SDK—Device： 不携带参数
Device- SDK—  携带参数
*/

/*
错误码定义
*/
#define AV_UPDATE_NO_RIGHT       0xEE000001 //无权限
#define AV_UPDATE_DATA_ERR       0xEE000002 //数据错误
#define AV_UPDATE_VER_ERR        0xEE000003 //版本错误
#define AV_UPDATE_REFRESH_ERR    0xEE000004 //版本更新错误
#define AV_MEMORY_LACK           0xEE000005 //内存不足
#define AV_DOWNLOAD_FAIL         0xEE000006 //加载版本失败
#define AV_UP_STATE_ERR          0xEE000007 //当前状态不处理该请求	
#define AV_UP_FLASH_ERR          0xEE000008 //flash访问异常	


/*
文件头
功能：完成整体校验
*/


typedef struct tag_update_head
{
	BYTE tag[4];   //文件的tag值-‘s' 'x' 's' 'j'
	DWORD dwDeviceType;   //升级包对应的设备类型
	DWORD dwFileID;       //此升级包的ID号，随机数，用户断点传输
	DWORD dwTime;         //升级文件的日期，用户断点传输
	DWORD size;    //数据区大小，不包括数据头
	BYTE md5[32];  //升级数据的md5校验,从数据头开始计算
	DWORD dwDataHeadOffset; //数据头的偏移地址。当前为sizeof(SX_UPDATE_FILE_HEAD)
	DWORD dwHardVersion;    //适合的硬件平台版本号
	DWORD dwSDKVersion;     //该版本对应的SDK版本号	
	BYTE  byRev[4];          //保留字
}SX_UPDATE_FILE_HEAD, *LP_SX_UPDATE_FILE_HEAD;

/*
数据头
功能：数据的索引功能
*/


typedef struct tag_sub_packet_head
{
	DWORD dwOffset; //包在文件中的偏移地址
	SX_SUB_PROG_ATTR_INFO attr;
}SX_SUB_PACKET_HEAD, *LP_SX_SUB_PACKET_HEAD;


/*
升级包的数据头
功能：匹配
*/

typedef struct tag_update_date_head
{
	DWORD dwSoftVersion;     //整体的软件版本号
	DWORD dwUpdateLibSum;  	 //要升级的库的数量
	
	//可变子包的头 dwLibSum * sizeof(SX_SUB_PACKET_HEAD)

}SX_AV_UPDATE_DATA_HEAD, *LP_SX_AV_UPDATE_DATA_HEAD;

typedef struct
{
    DWORD dwHardVersion;   	//硬件版本号。高16位是主版本,低16位是次版本 当前版本 0x00010002（v1.2），客户端显示为v1.2的形式
    DWORD dwSoftVersion;  	
    DWORD dwRev;       				//保留，软件版本扩展
    DWORD dwARMVersion;             //ARM软件版本，版本定义格式同软件主版本定义
    DWORD dwEncodeDSPVersion;       //编码DSP软件版本，版本定义格式同软件主版本定义
    DWORD dwDecodeDSPVersion;       //解码DSP软件版本，版本定义格式同软件主版本定义
    BYTE INSoftVersion [12];       		
    //内部软件版本=ARM版本+编码DSP软件版本+解码DSP软件版本
    	DWORD dwSoftBuildDate;	//软件生成日期,0xYYYYMMDD 【年（最高二个字节）】【月（次高一个字节）】【日（最后一个字节）】，十进制显示
    DWORD language; 					//语言
    DWORD byAlarmOutPortNum; 		   	//DVS报警输出个数
    DWORD TransFlag; 		   			
    //传输数据标志位，按位，bit0：ARM传输标志位，0：表示数据不传输，1：表示数据传输；bit1：编码DSP传输标志位，0：表示数据不传输，1：表示数据传输；bit2：解码DSP传输标志位，0：表示数据不传输，1：表示数据传输*/
    DWORD total_length;   				//升级文件总长度
    DWORD ARM_start_Addr;   		   	//ARM起始地址
    DWORD ARM_length;   				//ARM升级文件长度
    BYTE ARM_MD5[32];					//ARM升级数据的MD5计算值，用于校验升级数据的完整性
    DWORD EncodeDSP_start_Addr;   		//编码DSP起始地址
    DWORD EncodeDSP_length;   			//编码DSP升级文件长度
    BYTE EncodeDSP_MD5[32];			    //编码DSP升级数据的MD5计算值，用于校验升级数据的完整性
    DWORD DecodeDSP_start_Addr;   		//解码DSP起始地址
    DWORD DecodeDSP_length;   			//解码DSP升级文件长度
    BYTE DecodeDSP_MD5[32];			    //解码DSP升级数据的MD5计算值，用于校验升级数据的完整性
}SX_AV_UPDATE_VERIFY_HEAD, *LP_SX_AV_UPDATE_VERIFY_HEAD;


