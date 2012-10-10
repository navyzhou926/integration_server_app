#ifndef _NET_SERVER_H_
#define _NET_SERVER_H_

/*------------命令宏定义-----------------*/

#define   MAX_DAYS                              7
#define   MAX_TIMESEGMENT                4
#define SERVER_PORT_TCP                10000
#define USER_PWD_LEN                         16
#define USER_NAME_LEN                       32
#define N_NET_LISTNING_PORT            10
#define MAX_TIME_INTERNAL_FOR_CTL 15                                   //链接时限
#define SERIALNO_LEN                            32
#define SOCKET_BUSY_TRY_TIMES	 5
#define QULIFIED 					1 					//确认 
#define ERRORPASSWD				2  					//用户名密码错误 
#define ERRORDATA 					10					//客户端发送的数据有误
#define LOWPRI 					3 					//权限不足 
#define VERSIONNOMATCH 			4 					//版本不匹配 
#define NOSPECCHANNEL 			5	   			        //没有指定的通道
#define SXC_LOG_IN 	    			0xF10000 	                        // 客户端登录 客户端退出登陆命令：
#define CURRENT_NOALIVE  			  57 				 	//设备不在线
#define SXC_LOG_OUT				 0xF10001	                        // 客户端退出 
#define SDK_VERSION                   		   1                                   //版本号
#define NOSUPPORT 					   9 		                        //设备不支持该操作
#define MAX_TIME_OUT_MS      		10*1000
#define SXC_KEEPLIVE_B 			0xF10200	            		  // 双向握手包命令
#define SXD_LOG_IN				0xFA0000			 	 // 设备端登录
#define SXD_LOG_OUT				0xFA0001 				 // 设备端退出 
#define SXC_PTZ 					 0xF30200	                         //云台控制 
//-------------------------------------------------------------------------------云台控制命令。
#define PTZ_UP					1			//以ptzBit速度向上
#define PTZ_DOWN				2			//以ptzBit速度向下
#define PTZ_LEFT				3	    	 	 //以ptzBit速度向左
#define PTZ_RIGHT				4			//以ptzBit速度向右
#define PTZ_APERTURE_BIG		5  			//光圈调节，变大
#define PTZ_APERTURE_SMALL	6			//光圈调节，缩小
#define PTZ_FOCUS_IN			7			//焦点前调
#define PTZ_FOCUS_OUT			8 			//焦点后调
#define PTZ_ZOOM_IN			9  			//焦距变大
#define PTZ_ZOOM_OUT			10			//焦距变小
#define PTZ_AUTO_ZOOM			11 			//开始自动焦距（自动倍率）
#define PTZ_AUTO_FOCUS		12			//开自动调焦
#define PTZ_AUTO_APERTURE		13			//开自动光圈
#define PTZ_SET_PTZBIT			14			//设置预置点
#define PTZ_CLE_PTZBIT			15			//调用预置点
#define PTZ_STOP_ALL			 16			//停止所有连续量（镜头，云台）动作
#define AUX_PWRON1			17  			//辅助功能1开
#define AUX_PWROFF1			18  			//辅助功能1关
#define AUX_PWRON2			19 			 //辅助功能2开
#define AUX_PWROFF2			20 			//辅助功能2关
#define PTZ_EX_LEFT_UP			21			// 左上方
#define PTZ_EX_LEFT_DOWN		22			// 左下方
#define PTZ_EX_RIGHT_UP		23			// 右上方
#define PTZ_EX_RIGHT_DOWN	24			// 右下方

#define SXC_EXCHAGE_MATRIX_INOUT 	0xE10000			//切换矩阵输入输出
#define SXC_CANCEL_MATRIX_SINGLE_INOUT 	0xE10002	//取消矩阵单通道输出
#define SXC_CANCEL_MATRIX_ALL_INOUT 	0xE10003		//取消所有矩阵通道输出

#define MATRIX_MAX_CHANNEL_IN  8
#define MATRIX_MAX_CHANNEL_OUT 4
//------------------------------------------------------------------------------------------------门禁命令
#define SXC_DOOR_CTL_CMD		0xE30000					//控制门开/关
#define SXC_SET_DOORCFG  0xE300C1		    			        // 设置门禁参数
#define SXC_GET_DOORCFG  0xE300C2	             			        // 获取门禁参数
#define SXC_GET_DOOR_STATUS 0xE300B0	       				// 门禁的状态
//------------------------------------------------------------------------------------------------报警
#define SXC_ALARMCTL_DEV_GET_STATE_V2  	0xD20000		// 获取报警控
#define SXC_ALARMCTL_DEV_CTL_CMD_V2 	0xD20001			 // 控制报警控制器:打开， 关闭，布防，
#define SXC_ALARMCTL_DEV_SET_PARAM	0xE20006			// 设置报警参数
#define SXC_GETALAOUT_STATE		0xF30202 				//客户端获取报警输出状态
#define SXC_SETALAOUT			0xF30201 					//客户端控制报警输出

#define SXC_ALARM_UPLOAD_MANU  0xE300A0				// 手动报警上传
#define SXC_ALARM_UPLOAD_AUTO   0xE300A1 				  //自动报警上传 


#define SXC_ALARMCTL_DEV_SET_ADDR_PARAM	0xE20007		// 设置报警主机的地址参数
#define SXC_ALARMCTL_DEV_SET_PWD_PARAM	0xE20008		// 设置报警主机的密码参数

#define SXC_GET_ALARM_LINKAGE_CFG_V2 	0xD40000		//获取报警联动配置参数
#define SXC_SET_ALARM_LINKAGE_CFG_V2    0xD40001		//设置报警联动配置参数

#define SXC_GET_ALARM_OUTPUT_DURATION_TIME	0xD40002	//获取报警输出持续时间
#define SXC_SET_ALARM_OUTPUT_DURATION_TIME 	0xD40003	//设置报警输出持续时间

#define SXC_RESET_ALARM_LINKAGE_AND_TIMEPARA 	0xD40004	//恢复报警输入输出默认参数

//------------------------------------------------------------------------------------------------设置获取网络参数
#define SXC_GET_NETCFG		         0xF20100					// 获取网络参数 
#define SXC_SET_NETCFG 	                 0xF20101					// 设置网络参数
#define IPMISMATCH 				18						//IP地址不匹配 
#define MACMISMATCH 				19						//MAC地址不匹配 
//----------------------------------------------------------------------------------------------------开关量输入输出
#define  SXC_GET_ALARMOUTCFG 	0xF20420 						//获取开关量输出参数
#define  SXC_SET_ALARMOUTCFG 	0xF20421 						//设置开关量输出参数

#define SXC_GET_ALARMINCFG	0xE20410					 	//获取报警输入参数
#define SXC_SET_ALARMINCFG	0xE20411 						 //设置报警输入参数

//-----------------------------------------------------------------------------------------------------串口属性
#define SXC_GET_CONNECT_PAMA  0xE0C000						//获取串口连接设备参数
#define SXC_SET_CONNECT_PAMA  0xE0C001						//设置串口连接设备参数
#define SXC_DOOR_ALMIN_V2	 0xE300A0						// 门磁报警输出

#define SXC_DEV_NOALIVE 		0xE20502 						//设备不在线
//---------------------------------------------------------------------------- 获取多功能服务器工作状态
#define	SXC_GET_ICC_DEV_PARAM		0xE70C00

//----------------------------------------------------------------设备信息
#define   DEV_ID_VERSION     1.0	
#define   DEV_SERIAL_Num     1234567
#define   DEV_SOFT_VERSION     1.0
#define   DEV_HARD_VERSION     1.0
#define   DEV_COMM_VERSION     1.0
#define   DEV_COM_NUM           7

#define SXC_REBOOT 			0xF30900 			//重启

#define SXC_SET_TIMECFG 		0xF20501 			//设置设备时间

#define SXC_PARA_RESTORE 		0xF30901 			//恢复默认参数

#ifdef __WINDOWS_
#include <winsock2.h>
#define MSG_NOSIGNAL 0
#endif

/*---------------结构定义-----------------------------------------------------------------------------*/

//-------------------------------------------------------------------------------------客户端发送命令头格式
typedef struct
{
    int  version;   							//网络通讯协议版本，目前为0x01
    int  length;	       							// 整合数据包的长度
    int checkSum;							// 校验和
    int cmdWord;							// 客户端命令
    int status;								// 作为命令头时为0，返回头时为状态
    int usrID;								// 用户ID
    char  clientMAC[6];						// 客户端MAC地址
    char  savestate;		       				// 对于参数设置操作，0：确认；1：保存。
    // 对于其它操作该位为保留位
    char  spare[1];							//保留
} INTER_SXC_HEAD;

//---------------------------------------------------------------------------------------设备端发送命令头格式,weilei.
typedef struct
{
    int version;
    int length;
    int checkSum;
    int requestID;   //
    int status;      							//做为命令头时为0，返回头时为状态
    char  clientMAC[6];
    char  spare[6];
}INTER_SXD_HEAD,*LPINTER_SXD_HEAD;

//-------------------------------------------------------------------------------------------客户端登录申请参数
typedef struct
{
    char   usrName[16];
    char   reserved1[16];
    char   usrPwd[32];
}SX_INTER_SXC_LOGIN,*LPSX_INTER_SXC_LOGIN;

//-----------------------------------------------------------------------------------------设备端登录申请参数
typedef struct{
    int user_id;
    int pathNO;
    int pathSum;
    int alarmInNum;
    int alarmOutNum;
    int deviceType;
    char  deviceMAC[6];
}SX_INTER_SXD_LOGIN,*LPSX_INTER_SXD_LOGIN; 

typedef struct{
    char   		usrName[16];
    char   		reserved[16];					// 保留
    char   		usrPwd[32];					// MD5加密
    int 		UserId;   						// 用户ID,分配给客户端的用户ID

    //设备序列号=设备型号+生产日期+流水号+生产总数
    char 		sSerialNumber[SERIALNO_LEN];	// 出厂序列号 32字节
    char  		macAddr[6]; 
    char			channelNums; 
    char 		byStartChan; 			   		// 起始通道号 
    char 		byAlarmInPortNum; 			// 设备报警输入个数 
    char 		byAlarmOutPortNum; 			// 设备报警输出个数
    char 		res[30]; 
}SX_INTER_SXD_LOGIN_V1,*LPSX_INTER_SXD_LOGIN_V1; 
//-----------------------------------------------------------------------------------------设备端登录申请包
typedef struct
{
    INTER_SXD_HEAD device_send_head;
    SX_INTER_SXD_LOGIN_V1 device_send_data;
}SX_INTER_SXD_LOGIN_SEND_V1,*LPSX_INTER_SXC_LOGIN_SEND_V1;

// --------------------------------------------------------------------设备返回时参数定义
typedef struct
{
    int pathNO;								// 起始通道号。
    int pathSum;							// 通道总数。128.
    int alarmInNum;							// 报警输入个数。
    int alarmOutNum;						// 报警输出个数。
    int deviceType;							// 设备类型。1:dvs;2:dvr 2:mms.
    char  mmsMAC[6];						// 设备端MAC地址
}SX_INTER_MMSINFO;


//----------------------------------------------------------------------客户端登录时设备返回的参数。(设备发送指令)
typedef struct
{
    int usrID;
    SX_INTER_MMSINFO mmsInfo;
}SX_INTER_SXD_LOGIN_RET, *LPSX_INTER_SXD_LOGIN_RET;
//----------------------------------------------------------------------客户端登陆确认与退出登陆
typedef struct
{
    INTER_SXD_HEAD sxdHeader;	
    int usrID;
    SX_INTER_MMSINFO dvsInfo;
}INTER_SXD_LOGIN_RET,*LP_INTER_SXD_LOGIN_RET;

//----------------------------------------------------------------------保存链接的socket和接受的数据包头
typedef struct ck_fd{
    INTER_SXC_HEAD Inter_sxc_head;
    int connect_fd;
    int client_ip;

}LISTEN_ACQUIRED_PARA;

//	--------------------------------------------------------------------socket套接字的类型
typedef enum tag_sock_type
{
    TCP_SOCK,
    UDP_SOCK,
    BRD_SOCK,
    E_SOCK_TYPE_DEFAULT_PATCH
}E_SOCK_TYPE;


//-----------------------------------------------------------------------初始化的socket属性
typedef struct tag_SOCK_ATTR{
    int reused;//1：表示可以重用，0：表示默认
    int noblocked;//1：表示非阻塞， 0：表示默认
    int stf;											//>0表示设置了发送超时时间，0：表示默认
    //struct timeval send_timeout;
    int rtf;											//>0表示设置了接收超时时间，0：表示默认
    //struct timeval recv_timeout;
    int reserved;

}SOCK_ATTR;

//-------------------------------------------------------------------------用户信息
typedef struct userlist{
    int user_id;
    int sockfd_client;
    //char server_mac[36];
    struct userlist *next;
}user_info;

// -------------------------------------------------------------------------客户端云台控制命令参数：
typedef struct  {
    long ptzID;								// 云台ID，4个字节
    long ptzCommand;						// 云台控制命令，4个字节
    long ptzBit;								// 该参数表示控制速度，4个字节
}SX_INTER_PTZ_REQ, *LPSX_INTER_PTZ_REQ;

// --------------------------------------------------------------------------控制矩阵切换结构体
typedef struct
{
    long  matrixID;							// 设备ID，4个字节
    long  nValueIn;							// 值输入，4个字节
    long  nValueOut;							// 值输出，4个字节
}SX_INTER_MATRIX_REQ, *LPSX_INTER_MATRIX_REQ;


//--------------------------------------------------------------------------- 控制门禁命令类型
typedef enum
{
    DOORCTL_CLOSE=0,					     		// 关闭
    DOORCTL_OPEN ,					  	        // 打开
    DOORCTL_CMD_NUM,
}DOORCTL_CMD_TYPE;
//---------------------------------------------------------------------------- 控制门禁结构体
typedef struct
{
    char byDevNo;						    // 门禁控制器主机id。范围：1,2,3…255
    char byDoorNo;				 		    // 门ID。范围：1,2,3…255
    char byCmdType;					    // 控制命令类型。
    // 详见枚举类型，DOORCTL_CMD_TYPE
    char byRsv[1];
}SXC_INTER_DOOR_CTL_CMD;

// -----------------------------------------------------------------------------门磁状态
typedef enum
{
    DOOR_CONTACT_OPEN_CIRCUIT = 0,	             // 开路
    DOOR_CONTACT_SHORT_CIRCUIT,	             // 短路
    DOOR_CONTACT_OPEN_SHORT_CIRCUIT, 	     // 开路和短路都检测
    DOOR_CONTACT_OPEN_SHORT_CIRCUT_NO,    // 开路和短路都不检测
}SXC_DOOR_CONTACT_CHECK_MODE;
//------------------------------------------------------------------------------- 门禁参数
typedef struct
{
    char byDevNo;						     // 门禁控制器主机id。范围：1,2,3…255
    char byDoorNo;			                             // 门ID。范围：1,2,3…255
    char byDoorLock;			                     // 门锁。0-常开，1-常闭。
    char byDelayCloseDoor1;	                             // 门禁主机操控延时关门时间
    char byDelayCloseDoor2;                               // 外部操控延时关门时间
    char byDoorContMode;	                             // SXC_DOOR_CONTACT_CHECK_MODE
    char byReserved[2];		                             // 保留
}SX_INTER_DOOR_PARA_REQ, *LP_INTER_DOOR_PARA_REQ;

// -------------------------------------------------------------------------------门禁当前的状态
typedef struct
{
    char byDevNo;							// 门禁控制器主机id。范围：1,2,3…255
    char byDoorNo;							// 门ID。范围：1,2,3…255
    char byDoorStatus;						// 门当前状态。e.g.: 打开；关闭
    char byDoorContactDetectionMode;			//门磁检测方式设置(短路)(默认)
    char byDoorLockRelayStatus;				// 门锁继电器状态设置(常开) 
    char resv[3];							// 保留

}INTER_DOOR_STATUS,*LP_INTER_DOOR_STATUS;

//--------------------------------------------------------------------------------发送当前门禁参数
typedef struct 
{
    INTER_SXD_HEAD sxdHeader;
    SX_INTER_DOOR_PARA_REQ sxddoorpara;
}INTER_SXD_DOOR_PARA_SEND;

//--------------------------------------------------------------------------------发送当前门禁状态
typedef struct 
{
    INTER_SXD_HEAD sxdHeader;
    INTER_DOOR_STATUS sxddoorstate;
}INTER_SXD_DOOR_STATUS_SEND;

//-------------------------------------------------------------------------------- 状态查询命令类型
typedef enum
{
    ALARMCTL_DEV_RUNNING_STATE=0,		// 设备开关状态
    ALARMCTL_DEV_PATROL_CHECK,			// 巡检
    ALARMCTL_DEV_WORK_STATE,			// 布撤防查询功能
    ALARMCTL_DEV_STATE_CMD_NUM,	
}ALARMCTL_DEV_STATE_CMD_TYPE;

// -------------------------------------------------------------------------------   状态类型
typedef enum 
{
    ALARMCTL_DEV_CLOSED=0,				// 设备关闭
    ALARMCTL_DEV_OPENED,				// 设备启用
} ALARMCTL_DEV_STATE;

typedef enum
{
    ALARMCTL_DEV_DISARMED = 0,			// 未布防
    ALARMCTL_DEV_ARMED ,					// 已布防
} ALARMCTL_DEV_DEFENCE_STATE;

typedef struct 
{
    int byAlarmState;						//防区报警状态（哪个防区报警了）
    int byBypassState;						//防区旁路状态（哪个防区被旁路了）
    int byDefenceAlarmType;					//哪种类型的报警(普通报警，紧急报警)
} ALARMCTL_DEV_ALARM_STATE;

//-----------------------------------------------------------------------------------获取报警控制器状态
typedef struct
{
    char byDevNo;		            				// 报警控制主机id
    char byCmdType;	          				  // 查询功能类型:巡检
    char byDevState;                        			  //取值见ALARMCTL_DEV_STATE
    char byDevDefenceState;       				 //取值见ALARMCTL_DEV_DEFENCE_STATE
    char byRsv[4];			    				 // 保留
    ALARMCTL_DEV_ALARM_STATE  byAlarmState;
} SXC_INTER_ALARMCTL_DEV_GET_STATE, *LP_SXC_INTER_ALARMCTL_DEV_GET_STATE;

//-----------------------------------------------------------------------------------发送当前报警器状态
typedef struct 
{
    INTER_SXD_HEAD sxdHeader;
    SXC_INTER_ALARMCTL_DEV_GET_STATE sxdalarmstate;
} INTER_SXD_ALARM_STATUS_SEND;

//------------------------------------------------------------------------------------报警控制器控制命令类型
typedef enum
{
    ALARMCTL_DEV_OPEN=0,					// 打开命令
    ALARMCTL_DEV_CLOSE,					// 关闭命令
    ALARMCTL_DEV_ARMING,        				 //布防命令
    ALARMCTL_DEV_DISARMING,				// 撤防命令
    ALARMCTL_DEV_FIRE_POLICE,			// 消警命令
    ALARMCTL_DEV_RESET,					// 复位命令
    ALAEMCTL_DEV_OTHER_DISARMING,		// 旁路防区
    ALARMCTL_DEV_CMD_NUM,

} ALARMCTL_DEV_CMD_TYPE;

//--------------------------------------------------------------------------------------报警控制器控制
typedef struct
{
    char byDevNo;							// 报警控制器主机id
    char byAreaNo;							// 报警控制器区域id
    short wGuardNo;						// 报警控制器防区id，1-16
    char byCmdType;						// 控制命令类型:打开+关闭+ 布防+ 撤防+ 消警+旁路
    char byRsv[3];	
} SXC_INTER_ALARMCTL_DEV_CTL_CMD;

//-------------------------------------------------------------------------------------- 报警控制器参数设置
typedef struct 
{
    char byAlarmHostID;						// 报警主机ID
    char byEnterDelay;						// 进入延时时间10-150s
    char byOutDelay;						// 外出延时时间10-300s
    char byAlarmDelay;						// 警告延时时间2分钟，5分钟，10分钟，15分钟，							//  30分钟
}SXC_INTER_ALARMCTL_SET_PARAM_REQ;

//---------------------------------------------------------------------------------------报警输出参数
typedef struct
{	
    INTER_SXD_HEAD sxdHeader;
    int  alarm_state;	
}SXC_INTER_ALARM_OUT_STATE;

//---------------------------------------------------------------------------------------报警输出控制
typedef struct
{
    int channel;   //报警输出端口，从0开始，表示单个报警输出报警端口，0xFF表示控制全部输出，
}SXC_INTER_ALARM_OUT_CTRL;

// ---------------------------------------------------------------------------------------开关量输入参数
typedef struct
{ 
    //开始时间 
    char byStartHour;						// 时
    char byStartMin; 						// 分钟
    //结束时间 
    char byStopHour; 						// 时
    char byStopMin; 							// 分钟
}INTER_SCHEDTIME,*LPINTER_SCHEDTIME;

typedef struct
{ 
    char byAlarminNo;						// 报警输入的通道号。
    char byAlarmInDelay;						// 输入延时。范围： 0-60S
    char byAlarmType; 						// 报警器类型。0：常开,1：常闭  
    char byAlarmInHandle; 					// 是否处理。 
    char sAlarmInName[16];					// 名称。
    char byRsv[16];							// 保留
    // 处理方式。默认处理值：声音、上传、联动对应的报警输出通道
    //INTER_HANDLEEXCEPTION struAlarmHandleType; 

    // 布防时间。7天4个时间段，每天的第一个时间段00：00 ~23:59
    INTER_SCHEDTIME struAlarmTime[7][4];	 

    int dwPtzPathNo;						// 报警触发的图像通道号，0~31，

    // 0xff不连图像。缺省设置1对1
    char byPresetNo; 						// 调用的云台预置号，1-128有效，

    // 0xff表示不调用预置点。
    int dwRecordChan; 						// 报警触发的录象通道按位，

    // bit0：通道1；bit1：通道2；
    // bit2通道3；bit3：通道4…；bit
}INTER_MMS_ALARMINCFG,*LPINTER_MMS_ALARMINCFG;



//---------------------------------------------------------------------------------------开关量输出参数
typedef struct
{ 
    char byAlarmoutNo;					// 报警输出的通道号。
    char sAlarmOutName[16];				// 名称 16字节 
    char reserved[16];					// 保留 
    int dwAlarmOutDelay;				// 输出延时(-1（0xf…）为无限， 
    // 手动关闭)   [0~600S]   5S，
    // 报警发出后，输出的延迟时间

    // 报警输出激活时间段;7天4个时间段，星期一表示第一天，
    // 每天的第一个时间段00：00 ~23:59，当设置为全天时：送第
    // 一个时间段 00：00-24：00表示全天，
    // 其它3个时间段添00：00-00：00	
    INTER_SCHEDTIME 	struAlarmOutTime[MAX_DAYS][MAX_TIMESEGMENT];
}INTER_ALARMOUTCFG,*LPINTER_ALARMOUTCFG;


//---------------------------------------------------------------------------------------主动模式控制中心info
typedef struct
{
    int client_id;
    char mode;
    char user_table_idx;
    char *name;
    char *pwd;
    int ip;
} T_ACTIVE_CONTEXT;

//---------------------------------------------------------------------------------------网络参数
typedef struct
{ 
    int  dwSize;
    int dwDeviceIP;					//设备 IP地址, 默认值：192.168.10.64
    //修改后重启生效 
    int dwDeviceIPMask;				//设备 IP地址掩码, 默认值：255.255.255.0
    //修改后重启生效 
    int dwNetInterface;				//网络接口不可设
    short wDevicePort;					// tcp端口号 ，默认为10000
    char byMACAddr[6]; 				// 物理地址，只用于显示 6字节, 不可设
    int dwGatewayIP; 					//网关地址默认值：192.168.10.100
    int dwManageHostIP;				// 远程管理主机地址，主动模式主动连接
    // 的IP地址, 默认值：122.200.70.151
    short wManageHostPort; 				// 远程管理主机端口号，默认为10000 
    short wRev;
    int dwCataSvrIP1;					// 目录服务器ip1端口固定为1008，
    // 目录服务器IP参数用于
    // 《远程系统联网协议_企标》
    int dwCataSvrIP2;					// 目录服务器ip2 端口固定为1008，
    // WEB服务端口固定为80
    char PPPOEUserName[16];			// PPPOE用户名
    char reserved[16];					// 保留
    char PPPOEPassWord[16];			// PPPOE密码
    char reserved2[16];				// 保留
    int    PPPOEIP;						// 不可设，获取参数				
    char PPPOEUSEDTYPE;				// 0-不使用 1-保持在线 2-按需上线
    //（包括：故障、报警、智能识别） 
    // 3-唤醒上线
    int  MultiCastAddr;
    char byNetMode;					// 网络类型0：局域网 1：PPPOE  2：CDMA
    // 当网络类型为0时PPPOEUSEDTYPE 不可选，
    // 强制为 1-保持在线。当网络类型为1: 								
    // PPPOEPassWord PPPOEPassName可设置，
    // 其余不可设置
    char active_mode_name[16];		// 主动模式-认证用户名 
    char reserved3[16];					// 保留
    char active_mode_pwd[16];			// 主动模式-认证用户密码 
    char reserved4[32];					// 保留
}INTER_NETCFG,*LPINTER_NETCFG;

//---------------------------------------------------------------------------------------网络参数+修改标志位:0未设置 1 已设置
typedef struct
{
    int flag;
    INTER_NETCFG para;
}INTER_NETCFG_FILE,*LPINTER_NETCFG_FILE;

//-------------------------------------------------------------------------------------发送网络参数数据包
typedef struct{
    INTER_SXD_HEAD sxdHeader;
    INTER_NETCFG  para_net_send;
}INTER_NETCFG_SEND,*LPINTER_NETCFG_SEND;

//---------------------------------------------------------------------------------------设备类型
typedef enum  
{
    LINK_DEV_UNDEFAULT = 0,
    LINK_DEV_MATRIX, 					// 矩阵
    LINK_DEV_ALARM_CTL,				// 报警控制器
    LINK_DEV_DOOR_CLT,				// 门禁控制器
    LINK_DEV_CRADLE_HEAD,			// 云台
    LINK_DEV_TYPE_NUM, 
}SERIAL_LINK_DEV_TYPE;

// --------------------------------------------------------------------------------------RS232串口属性 
typedef struct
{ 
    int dwBaudRate;						// 波特率
    char byDataBit; 						// 数据位。 0-5位，1-6位，2-7位，3-8位 
    char byStopBit;						// 停止位。0-1位，1-2位
    char byParity; 						// 校验。0-无校验，1-奇校验，2-偶校验 
    char byFlowcontrol;					// 0-无，1-软流控，2-硬流控 
    int dwRsv[2];
}RS232_ATTR;

// ---------------------------------------------------------------------------------------矩阵协议类型
typedef enum  
{
    MATRIX_PROTOCOL_UNKNOW=0, 
    MATRIX_PROTOCOL_PELCO,        
    MATRIX_PROTOCOL_AD,
    MATRIX_PROTOCOL_SX,
    MATRIX_PROTOCOL_NUM, 
}MATRIX_PROTOCOL_TYPE;

//------------------------------------------------------------------------------------------报警控制器协议类型
typedef enum  
{
    ALARM_CTL_PROTOCOL_UNKNOW=0,        
    ALARM_CTL_PROTOCOL_CK2316,        
    ALARM_CTL_PROTOCOL_VISTA120,        
    ALARM_CTL_PROTOCOL_NUM, 
}ALARM_CLT_PROTOCOL_TYPE;

//------------------------------------------------------------------------------------------门禁控制器协议类型
typedef enum  
{
    DOOR_CTL_PROTOCOL_UNKNOW=0,        
    DOOR_CTL_PROTOCOL_COSON,        
    DOOR_CTL_PROTOCOL_WESTINGHOUSE,        
    DOOR_CTL_PROTOCOL_NUM, 
}DOOR_PROTOCOL_TYPE;

// ------------------------------------------------------------------------------------------设备选择定义
typedef struct
{
    // 0:连接设备 SERIAL_LINK_DEV_TYPE;
    char byDevType;		

    // 1:矩阵 2:报警控制器 3:门禁控制器，4：云镜// 解码器
    char byDevNo;// 设备号 

    char byDevProtocol;	// 设备协议，详见以上枚举类型
    // e.g. 如果为矩阵，设备号为1，
    // 协议就从MATRIX_PROTOCOL_TYPE
    // 中选择，其他的类似
}SERIAL_LINK;

//------------------------------------------------------------------------------------------- 串口类型定义
typedef struct
{
    char byPathNo;					// 串口号
    char byRsv[3];					// 保留
    SERIAL_LINK  serialLink;			// 选择连接设备
    RS232_ATTR  serialAttr;			// 连接设备的串口属性
    int dwRsv[1];
}SERIAL_PAMATER;
//-------------------------------------------------------------------------------------------待发送的串口参数
typedef struct
{
    INTER_SXD_HEAD sxdHeader;
    SERIAL_PAMATER com_para;
}SERIAL_PAMATER_SEND;

//-------------------------------------------------------------------------------------------- 门禁报警类型

typedef enum
{
    ENTRANCE_ALARM_UPLOAD,
    ALARM_DEV_UPLOAD,
    SWITCH_ALARM_UPLOAD,
}SXC_SYS_ALARM_TYPE;

typedef enum
{
    DOOR_OPEN = 0,
    DOOR_CLOSE,
    DOOR_FORCE_OPEN,
    DOOR_NO_ALARM,
}SXC_DOOR_SYS_ALARM_TYPE;

// ---------------------------------------------------------------------------------------------门禁报警
typedef struct
{
    char byDevNo;						// 门禁控制器主机id。范围：1,2,3…255
    char byDoorNo;						// 门ID。范围：1,2,3…255
    char byDoorAlmType;					// SXC_DOOR_SYS_ALARM_TYPE门禁
    char byRsv [9];						//保留
}SXC_DOOR_SYS_ALM_INFO;

//-------------------------------------------------------------------------------------------- 报警主机报警
typedef struct
{
    char byAlarmCtlID;					// 报警控制主机ID
    char byHostAlmType;					// 报警类型，0-普通防区，1-紧急防区 02  无报警，
    char byRsv[2];						// 保留
    int dwAlarmID;						// 防区号
    ;									// 保留
}SXC_ALARM_HOST_INFO;

//---------------------------------------------------------------------------------------------- 开关量报警
typedef struct
{
    int byChannelID;						// 开关量通道号 
    int byStatus;						// 开关量的状态 0-变化，1-未变化
    char byRservered[4];					// 保留
}SXC_ALRAM_SWITCH_IN;

typedef  union tag_alarm_ctl{
    SXC_DOOR_SYS_ALM_INFO sDoorSysAlm;
    SXC_ALARM_HOST_INFO   sAlarmHost;
    SXC_ALRAM_SWITCH_IN   sSwitchIn;
}alarmCtl_S;

//----------------------------------------------------------------------------------------------- 报警上传参数定义
typedef struct
{
    char Year;							// 年。与2000年的差值，范围00~99。
    // 例如：09，表示2009年，
    char Month;							// 月。范围：一年12个月份
    char Day;							// 日。 根据月份来确定。范围：1~31
    // 2月份需要考虑闰年的天数处理。
    char Hour; 							// 时。范围： 0~23
    char Minute;						// 分。范围： 0~59
    char Second;						// 秒。范围： 0~59
    char byDevID;						// 设备ID
    char byAlmType;						// 门禁；报警控制器；开关量
    alarmCtl_S alarmCtl ;
    char byReserved[4];					// 保留
}INTER_DOOR_ALMOUT, *LP_INTER_DOOR_ALMOUT;
//------------------------------------------------------------------------------------------------报警上传数据包
typedef struct
{
    INTER_SXD_HEAD sxdHeader;
    INTER_DOOR_ALMOUT alarm_para;	
}INTER_DOOR_ALMOUT_SEND;

//-------------------------------------------------------------------------------------------------报警联动配置 
typedef struct 
{
    char   byAlarmInType;				// 报警输入的报警类型。取值1-9
    int      byAlarmLinkOutChan;   			  // 报警联动输出的通道号。低8位，某一位为1表示联动该通道号
}ALARM_LINKAGE_PARA;
//-------------------------------------------------------------------------------------------------联动配置参数发送
typedef struct
{
    INTER_SXD_HEAD sxdHeader;
    ALARM_LINKAGE_PARA alarm_linkage_para;
}ALARM_LINKAGE_PARA_SEND;
//报警输出持续时间配置 
typedef struct 
{
    char  byAlarmOutChan;     // 报警输出的通道号。取值0-8，0表示设置所有通道的报警输出持续时间；1-8表示对应通道的报警输出持续时间；如果为获取时，通道号不能为0。如果时间为0时0分，则表示该通道的报警输出永久持续。
    char  byHour;           		  // 报警输出持续时间（小时）最大为24小时
    char  byMinute;	         	 // 报警输出持续时间（分钟）
    char  byReserved;
}ALARM_OUTPUT_DURATION_CFG_V1;

//-------------------------------------------------------------------------------------------------联动报警输出时间发送
typedef struct
{
    INTER_SXD_HEAD sxdHeader;
    ALARM_OUTPUT_DURATION_CFG_V1 linkage_time;
}ALARM_OUTPUT_DURATION_CFG_SEND;

//---------------------------------------------------------------------------------------------------报警主机密码和地址设置
typedef struct
{
    char byPassword[4];  					 //报警主机的模拟键盘的用户密码，4位密码都为0-9之间的数字
}SXC_ALARMCTL_PWD_PARA;

typedef struct
{
    char byAddress; 						  //报警主机的模拟键盘的地址（0-15）
}SXC_ALARMCTL_ADDR_PARA;


typedef struct
{ 
    //开始时间 
    char byStartHour;					// 时
    char byStartMin; 					// 分钟
    //结束时间 
    char byStopHour; 					// 时
    char byStopMin; 						// 分钟
} SX_INTER_SCHEDTIME,*LPSX_INTER_SCHEDTIME;

//---------------------------------------------------------------------------------------- 开关量输入参数
typedef struct
{ 
    char byAlarminNo;					// 报警输入的通道号。
    char sAlarmInName[16];				// 名称。
    char reserved[16];		   			 //保留
    char byAlarmType; 					// 报警器类型。0：常开,1：常闭
    char byAlarmInHandle; 				// 是否处理。 
    //SX_INTER_HANDLEEXCEPTION struAlarmHandleType;	//处理方式  默认处理值：声音、上传、联动对应的报警输出通道
    SX_INTER_SCHEDTIME struAlarmTime[7][4]; // 布防时间 7天4个时间段 每天的第一个时间段00：00 ~23:59	 
    int  byPtzPathNo;					// 报警触发的图像通道号，0~31，
    // 0xff不连图像。缺省设置1对1
    char byPresetNo; 					// 调用的云台预置号，1-128有效，
    // 0xff表示不调用预置点。
    int dwRecordChan; 					// 报警触发的录象通道按位，
    // bit0：通道1；bit1：通道2；
    // bit2通道3；bit3：通道4…；bit
    char byAlarmInDelay;					// 输入延时。范围： 0-60S
} SX_INTER_ALARMINCFG,*LPSX_INTER_ALARMINCFG;

//-------------------------------------------------------------------------------------------- 设备参数定义
typedef struct 
{
    char byDevID;					// 设备ID
    char byRsv[3];					// 保留
    char byDevSerialNum[128];		// 设备序列号
    char bySoftVer;  					// 软件版本号
    char byHardVer;  				// 硬件版本号
    char byCommProVer;				// 通信协议版本
    char bySerialNum;				// 串口个数
    char  rsv2[8];
} SX_DEVICE_WORKING_STATUS, LP_SX_DEVICE_WORKING_STATUS;
typedef struct 
{
    INTER_SXD_HEAD sxdHeader;
    SX_DEVICE_WORKING_STATUS para;
} SX_DEVICE_WORKING_STATUS_SEND, LP_SX_DEVICE_WORKING_STATUS_SEND;

typedef struct
{ 
    char  Year;	// 年。与2000年的差值，范围00~99。
    // 例如：09，表示2009年，
    char  Month;	// 月。范围：一年12个月份
    char  Day;	// 日。 根据月份来确定。范围：1~31
    // 2月份需要考虑闰年的天数处理。
    char  Hour; 	// 时。范围： 0~23
    char  Minute;	// 分。范围： 0~59
    char  Second;	// 秒。范围： 0~59
    char	rev[2];	// 保留。
}INTER_TIME,*LPINTER_TIME;

/*---------------变量声明-----------------------------------------------------------------*/
extern int server_listen_fd ,server_connect_fd ;
INTER_NETCFG para_net;//网络参数设置
extern char *net_para_file_path;
extern int if_have_net_client_connect;
extern user_info *head;
extern  FILE *fp_dev_config_file;

/*---------------函数声明-----------------------------------------------------------------*/
void *pthread_server_passive(void *arg);     //被动通信模式线程
void *pthread_server_active(void *arg);        //主动模式通信线程
void *pthread_handshake(void *arg);            //双向握手线程
int  net_para_init(char *file_path,int len);
int send_act_cmd(int status, int client_fd, int request_id, int usr_id);
int send_data_intime_over(int client_fd, int timeout, char *src_buf, int src_len);
int ck2316_user_password_setup(unsigned int user_password[4]);

extern int enable_net_listen();

extern int init_socket(int addr, int port, int listen_flag, int listen_num, int sock_type);

extern void *pthread_visit_handel(void *arg);

extern int recv_data_intime(int client_fd, int timeout, char *recv_buf, int src_len,int client_ip);

extern int recv_data(int client_fd, char *dst, int num,int client_ip);

extern INTER_SXC_HEAD *databyte_net_to_host(char *buffer);

extern int cmd_data_deal(INTER_SXC_HEAD *data_head,int client_fd,int para_len,int client_ip);

extern int channel_num_track(void);

extern int cycle_close_socket(int fd);

extern int set_lan_ip(unsigned int ip);

extern int set_local_lan_gateway(char *gateway_cmd);

extern void get_local_lan_mac(unsigned char *p_mac);

extern int set_local_lan_mac(unsigned char *p_mac);

extern int set_lan_netmask(unsigned netmask);

extern int ip_judge(int ip_maste,int device_ip,int client_ip);

extern int log_in_reply(char *recv_para,INTER_SXC_HEAD *data_head,int client_fd);//登陆申请回复

extern int login_apply(void );                                                                           //主动模式登录请求

extern int main_service_ctl(void);

extern user_info *link_insert(int fd);

extern  void link_print(user_info *head);

extern  user_info * link_delete(int fd);

extern int cradle_ctrl_reply(char *recv_para,INTER_SXC_HEAD *data_head,int fd);

extern int MATRIX_ctrl_change(char *recv_para,INTER_SXC_HEAD *data_head,int fd);

extern int MATRIX_ctrl_cancel_singal(char *recv_para,INTER_SXC_HEAD *data_head,int fd);

extern int MATRIX_ctrl_cancel_all(char *recv_para,INTER_SXC_HEAD *data_head,int fd);

extern int entrance_open_close_ctrl(char *recv_para,INTER_SXC_HEAD *data_head,int fd);//门禁开关

extern int entrance_para_get(char *recv_para,INTER_SXC_HEAD *data_head,int fd);

extern int entrance_para_set(char *recv_para,INTER_SXC_HEAD *data_head,int fd);

extern int api_connect_remote_client(int ip, short port, short time_out);

extern int net_connect(int client_id ,int dst_ip,short dst_port, short time_out);

extern int poll_wtime(int client_fd, int timeout);

extern int entrance_status_send(char *recv_para,INTER_SXC_HEAD *data_head,int fd);

extern int alarm_status_get(char *recv_buf,int fd);

extern int alarm_ctrl(char *recv_buf,int fd);

extern int alarm_para_set(char *recv_buf,int fd);

extern int net_para_set(char *recv_para,INTER_SXC_HEAD *data_head,int fd,int client_ip);

extern int net_para_send(char *recv_para,INTER_SXC_HEAD *data_head,int fd);

extern int ctl_lan_default_route(unsigned int route_gate_way, unsigned int genmask, unsigned int dst_addr, int cmd);

extern int send_data_intime(int client_fd, int timeout, char *send_buf, int src_len);

extern int send_data(int client_fd, char *src, int num);

extern int alarm_out_state(char *recv_buf,int fd);

extern int alarm_out_ctrl(char *recv_buf,int fd);

extern int switch_para_send(char *recv_buf,int fd);

extern int switch_para_set(char *recv_buf,int fd);

extern int switch_in_para_set(char *recv_buf,int fd);

extern int com_para_send(char *recv_buf,int fd);

extern int com_para_set(char *recv_buf,int fd);

extern int alarm_user_pwd_setup(char *recv_buf,int fd);

extern int alarm_user_addr_setup(char *recv_buf,int fd);

extern int get_alarm_linkage(char *recv_buf,int fd);

extern int set_alarm_linkage(char *recv_buf,int fd);

extern int alarmlinkage_output_time_set(char *recv_buf,int fd);

extern int alarmlinkage_output_time_get(char *recv_buf,int fd);

extern int alarmlinkage_output_time_cancle(char *recv_buf,int fd);

extern int dev_para_get(char *recv_buf,int fd);

extern int dev_restart(int fd);

extern int system_time_set(char *recv_buf,int fd);

extern int acquiescent_para_recovery(int fd);

extern int com_dwBaudRate_get(int rate);

extern int com_baudrate_set(int rate);

extern int alarm_upload_get(char *recv_buf,int fd);

#endif

