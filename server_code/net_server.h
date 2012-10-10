#ifndef _NET_SERVER_H_
#define _NET_SERVER_H_

/*------------����궨��-----------------*/

#define   MAX_DAYS                              7
#define   MAX_TIMESEGMENT                4
#define SERVER_PORT_TCP                10000
#define USER_PWD_LEN                         16
#define USER_NAME_LEN                       32
#define N_NET_LISTNING_PORT            10
#define MAX_TIME_INTERNAL_FOR_CTL 15                                   //����ʱ��
#define SERIALNO_LEN                            32
#define SOCKET_BUSY_TRY_TIMES	 5
#define QULIFIED 					1 					//ȷ�� 
#define ERRORPASSWD				2  					//�û���������� 
#define ERRORDATA 					10					//�ͻ��˷��͵���������
#define LOWPRI 					3 					//Ȩ�޲��� 
#define VERSIONNOMATCH 			4 					//�汾��ƥ�� 
#define NOSPECCHANNEL 			5	   			        //û��ָ����ͨ��
#define SXC_LOG_IN 	    			0xF10000 	                        // �ͻ��˵�¼ �ͻ����˳���½���
#define CURRENT_NOALIVE  			  57 				 	//�豸������
#define SXC_LOG_OUT				 0xF10001	                        // �ͻ����˳� 
#define SDK_VERSION                   		   1                                   //�汾��
#define NOSUPPORT 					   9 		                        //�豸��֧�ָò���
#define MAX_TIME_OUT_MS      		10*1000
#define SXC_KEEPLIVE_B 			0xF10200	            		  // ˫�����ְ�����
#define SXD_LOG_IN				0xFA0000			 	 // �豸�˵�¼
#define SXD_LOG_OUT				0xFA0001 				 // �豸���˳� 
#define SXC_PTZ 					 0xF30200	                         //��̨���� 
//-------------------------------------------------------------------------------��̨�������
#define PTZ_UP					1			//��ptzBit�ٶ�����
#define PTZ_DOWN				2			//��ptzBit�ٶ�����
#define PTZ_LEFT				3	    	 	 //��ptzBit�ٶ�����
#define PTZ_RIGHT				4			//��ptzBit�ٶ�����
#define PTZ_APERTURE_BIG		5  			//��Ȧ���ڣ����
#define PTZ_APERTURE_SMALL	6			//��Ȧ���ڣ���С
#define PTZ_FOCUS_IN			7			//����ǰ��
#define PTZ_FOCUS_OUT			8 			//������
#define PTZ_ZOOM_IN			9  			//������
#define PTZ_ZOOM_OUT			10			//�����С
#define PTZ_AUTO_ZOOM			11 			//��ʼ�Զ����ࣨ�Զ����ʣ�
#define PTZ_AUTO_FOCUS		12			//���Զ�����
#define PTZ_AUTO_APERTURE		13			//���Զ���Ȧ
#define PTZ_SET_PTZBIT			14			//����Ԥ�õ�
#define PTZ_CLE_PTZBIT			15			//����Ԥ�õ�
#define PTZ_STOP_ALL			 16			//ֹͣ��������������ͷ����̨������
#define AUX_PWRON1			17  			//��������1��
#define AUX_PWROFF1			18  			//��������1��
#define AUX_PWRON2			19 			 //��������2��
#define AUX_PWROFF2			20 			//��������2��
#define PTZ_EX_LEFT_UP			21			// ���Ϸ�
#define PTZ_EX_LEFT_DOWN		22			// ���·�
#define PTZ_EX_RIGHT_UP		23			// ���Ϸ�
#define PTZ_EX_RIGHT_DOWN	24			// ���·�

#define SXC_EXCHAGE_MATRIX_INOUT 	0xE10000			//�л������������
#define SXC_CANCEL_MATRIX_SINGLE_INOUT 	0xE10002	//ȡ������ͨ�����
#define SXC_CANCEL_MATRIX_ALL_INOUT 	0xE10003		//ȡ�����о���ͨ�����

#define MATRIX_MAX_CHANNEL_IN  8
#define MATRIX_MAX_CHANNEL_OUT 4
//------------------------------------------------------------------------------------------------�Ž�����
#define SXC_DOOR_CTL_CMD		0xE30000					//�����ſ�/��
#define SXC_SET_DOORCFG  0xE300C1		    			        // �����Ž�����
#define SXC_GET_DOORCFG  0xE300C2	             			        // ��ȡ�Ž�����
#define SXC_GET_DOOR_STATUS 0xE300B0	       				// �Ž���״̬
//------------------------------------------------------------------------------------------------����
#define SXC_ALARMCTL_DEV_GET_STATE_V2  	0xD20000		// ��ȡ������
#define SXC_ALARMCTL_DEV_CTL_CMD_V2 	0xD20001			 // ���Ʊ���������:�򿪣� �رգ�������
#define SXC_ALARMCTL_DEV_SET_PARAM	0xE20006			// ���ñ�������
#define SXC_GETALAOUT_STATE		0xF30202 				//�ͻ��˻�ȡ�������״̬
#define SXC_SETALAOUT			0xF30201 					//�ͻ��˿��Ʊ������

#define SXC_ALARM_UPLOAD_MANU  0xE300A0				// �ֶ������ϴ�
#define SXC_ALARM_UPLOAD_AUTO   0xE300A1 				  //�Զ������ϴ� 


#define SXC_ALARMCTL_DEV_SET_ADDR_PARAM	0xE20007		// ���ñ��������ĵ�ַ����
#define SXC_ALARMCTL_DEV_SET_PWD_PARAM	0xE20008		// ���ñ����������������

#define SXC_GET_ALARM_LINKAGE_CFG_V2 	0xD40000		//��ȡ�����������ò���
#define SXC_SET_ALARM_LINKAGE_CFG_V2    0xD40001		//���ñ����������ò���

#define SXC_GET_ALARM_OUTPUT_DURATION_TIME	0xD40002	//��ȡ�����������ʱ��
#define SXC_SET_ALARM_OUTPUT_DURATION_TIME 	0xD40003	//���ñ����������ʱ��

#define SXC_RESET_ALARM_LINKAGE_AND_TIMEPARA 	0xD40004	//�ָ������������Ĭ�ϲ���

//------------------------------------------------------------------------------------------------���û�ȡ�������
#define SXC_GET_NETCFG		         0xF20100					// ��ȡ������� 
#define SXC_SET_NETCFG 	                 0xF20101					// �����������
#define IPMISMATCH 				18						//IP��ַ��ƥ�� 
#define MACMISMATCH 				19						//MAC��ַ��ƥ�� 
//----------------------------------------------------------------------------------------------------�������������
#define  SXC_GET_ALARMOUTCFG 	0xF20420 						//��ȡ�������������
#define  SXC_SET_ALARMOUTCFG 	0xF20421 						//���ÿ������������

#define SXC_GET_ALARMINCFG	0xE20410					 	//��ȡ�����������
#define SXC_SET_ALARMINCFG	0xE20411 						 //���ñ����������

//-----------------------------------------------------------------------------------------------------��������
#define SXC_GET_CONNECT_PAMA  0xE0C000						//��ȡ���������豸����
#define SXC_SET_CONNECT_PAMA  0xE0C001						//���ô��������豸����
#define SXC_DOOR_ALMIN_V2	 0xE300A0						// �Ŵű������

#define SXC_DEV_NOALIVE 		0xE20502 						//�豸������
//---------------------------------------------------------------------------- ��ȡ�๦�ܷ���������״̬
#define	SXC_GET_ICC_DEV_PARAM		0xE70C00

//----------------------------------------------------------------�豸��Ϣ
#define   DEV_ID_VERSION     1.0	
#define   DEV_SERIAL_Num     1234567
#define   DEV_SOFT_VERSION     1.0
#define   DEV_HARD_VERSION     1.0
#define   DEV_COMM_VERSION     1.0
#define   DEV_COM_NUM           7

#define SXC_REBOOT 			0xF30900 			//����

#define SXC_SET_TIMECFG 		0xF20501 			//�����豸ʱ��

#define SXC_PARA_RESTORE 		0xF30901 			//�ָ�Ĭ�ϲ���

#ifdef __WINDOWS_
#include <winsock2.h>
#define MSG_NOSIGNAL 0
#endif

/*---------------�ṹ����-----------------------------------------------------------------------------*/

//-------------------------------------------------------------------------------------�ͻ��˷�������ͷ��ʽ
typedef struct
{
    int  version;   							//����ͨѶЭ��汾��ĿǰΪ0x01
    int  length;	       							// �������ݰ��ĳ���
    int checkSum;							// У���
    int cmdWord;							// �ͻ�������
    int status;								// ��Ϊ����ͷʱΪ0������ͷʱΪ״̬
    int usrID;								// �û�ID
    char  clientMAC[6];						// �ͻ���MAC��ַ
    char  savestate;		       				// ���ڲ������ò�����0��ȷ�ϣ�1�����档
    // ��������������λΪ����λ
    char  spare[1];							//����
} INTER_SXC_HEAD;

//---------------------------------------------------------------------------------------�豸�˷�������ͷ��ʽ,weilei.
typedef struct
{
    int version;
    int length;
    int checkSum;
    int requestID;   //
    int status;      							//��Ϊ����ͷʱΪ0������ͷʱΪ״̬
    char  clientMAC[6];
    char  spare[6];
}INTER_SXD_HEAD,*LPINTER_SXD_HEAD;

//-------------------------------------------------------------------------------------------�ͻ��˵�¼�������
typedef struct
{
    char   usrName[16];
    char   reserved1[16];
    char   usrPwd[32];
}SX_INTER_SXC_LOGIN,*LPSX_INTER_SXC_LOGIN;

//-----------------------------------------------------------------------------------------�豸�˵�¼�������
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
    char   		reserved[16];					// ����
    char   		usrPwd[32];					// MD5����
    int 		UserId;   						// �û�ID,������ͻ��˵��û�ID

    //�豸���к�=�豸�ͺ�+��������+��ˮ��+��������
    char 		sSerialNumber[SERIALNO_LEN];	// �������к� 32�ֽ�
    char  		macAddr[6]; 
    char			channelNums; 
    char 		byStartChan; 			   		// ��ʼͨ���� 
    char 		byAlarmInPortNum; 			// �豸����������� 
    char 		byAlarmOutPortNum; 			// �豸�����������
    char 		res[30]; 
}SX_INTER_SXD_LOGIN_V1,*LPSX_INTER_SXD_LOGIN_V1; 
//-----------------------------------------------------------------------------------------�豸�˵�¼�����
typedef struct
{
    INTER_SXD_HEAD device_send_head;
    SX_INTER_SXD_LOGIN_V1 device_send_data;
}SX_INTER_SXD_LOGIN_SEND_V1,*LPSX_INTER_SXC_LOGIN_SEND_V1;

// --------------------------------------------------------------------�豸����ʱ��������
typedef struct
{
    int pathNO;								// ��ʼͨ���š�
    int pathSum;							// ͨ��������128.
    int alarmInNum;							// �������������
    int alarmOutNum;						// �������������
    int deviceType;							// �豸���͡�1:dvs;2:dvr 2:mms.
    char  mmsMAC[6];						// �豸��MAC��ַ
}SX_INTER_MMSINFO;


//----------------------------------------------------------------------�ͻ��˵�¼ʱ�豸���صĲ�����(�豸����ָ��)
typedef struct
{
    int usrID;
    SX_INTER_MMSINFO mmsInfo;
}SX_INTER_SXD_LOGIN_RET, *LPSX_INTER_SXD_LOGIN_RET;
//----------------------------------------------------------------------�ͻ��˵�½ȷ�����˳���½
typedef struct
{
    INTER_SXD_HEAD sxdHeader;	
    int usrID;
    SX_INTER_MMSINFO dvsInfo;
}INTER_SXD_LOGIN_RET,*LP_INTER_SXD_LOGIN_RET;

//----------------------------------------------------------------------�������ӵ�socket�ͽ��ܵ����ݰ�ͷ
typedef struct ck_fd{
    INTER_SXC_HEAD Inter_sxc_head;
    int connect_fd;
    int client_ip;

}LISTEN_ACQUIRED_PARA;

//	--------------------------------------------------------------------socket�׽��ֵ�����
typedef enum tag_sock_type
{
    TCP_SOCK,
    UDP_SOCK,
    BRD_SOCK,
    E_SOCK_TYPE_DEFAULT_PATCH
}E_SOCK_TYPE;


//-----------------------------------------------------------------------��ʼ����socket����
typedef struct tag_SOCK_ATTR{
    int reused;//1����ʾ�������ã�0����ʾĬ��
    int noblocked;//1����ʾ�������� 0����ʾĬ��
    int stf;											//>0��ʾ�����˷��ͳ�ʱʱ�䣬0����ʾĬ��
    //struct timeval send_timeout;
    int rtf;											//>0��ʾ�����˽��ճ�ʱʱ�䣬0����ʾĬ��
    //struct timeval recv_timeout;
    int reserved;

}SOCK_ATTR;

//-------------------------------------------------------------------------�û���Ϣ
typedef struct userlist{
    int user_id;
    int sockfd_client;
    //char server_mac[36];
    struct userlist *next;
}user_info;

// -------------------------------------------------------------------------�ͻ�����̨�������������
typedef struct  {
    long ptzID;								// ��̨ID��4���ֽ�
    long ptzCommand;						// ��̨�������4���ֽ�
    long ptzBit;								// �ò�����ʾ�����ٶȣ�4���ֽ�
}SX_INTER_PTZ_REQ, *LPSX_INTER_PTZ_REQ;

// --------------------------------------------------------------------------���ƾ����л��ṹ��
typedef struct
{
    long  matrixID;							// �豸ID��4���ֽ�
    long  nValueIn;							// ֵ���룬4���ֽ�
    long  nValueOut;							// ֵ�����4���ֽ�
}SX_INTER_MATRIX_REQ, *LPSX_INTER_MATRIX_REQ;


//--------------------------------------------------------------------------- �����Ž���������
typedef enum
{
    DOORCTL_CLOSE=0,					     		// �ر�
    DOORCTL_OPEN ,					  	        // ��
    DOORCTL_CMD_NUM,
}DOORCTL_CMD_TYPE;
//---------------------------------------------------------------------------- �����Ž��ṹ��
typedef struct
{
    char byDevNo;						    // �Ž�����������id����Χ��1,2,3��255
    char byDoorNo;				 		    // ��ID����Χ��1,2,3��255
    char byCmdType;					    // �����������͡�
    // ���ö�����ͣ�DOORCTL_CMD_TYPE
    char byRsv[1];
}SXC_INTER_DOOR_CTL_CMD;

// -----------------------------------------------------------------------------�Ŵ�״̬
typedef enum
{
    DOOR_CONTACT_OPEN_CIRCUIT = 0,	             // ��·
    DOOR_CONTACT_SHORT_CIRCUIT,	             // ��·
    DOOR_CONTACT_OPEN_SHORT_CIRCUIT, 	     // ��·�Ͷ�·�����
    DOOR_CONTACT_OPEN_SHORT_CIRCUT_NO,    // ��·�Ͷ�·�������
}SXC_DOOR_CONTACT_CHECK_MODE;
//------------------------------------------------------------------------------- �Ž�����
typedef struct
{
    char byDevNo;						     // �Ž�����������id����Χ��1,2,3��255
    char byDoorNo;			                             // ��ID����Χ��1,2,3��255
    char byDoorLock;			                     // ������0-������1-���ա�
    char byDelayCloseDoor1;	                             // �Ž������ٿ���ʱ����ʱ��
    char byDelayCloseDoor2;                               // �ⲿ�ٿ���ʱ����ʱ��
    char byDoorContMode;	                             // SXC_DOOR_CONTACT_CHECK_MODE
    char byReserved[2];		                             // ����
}SX_INTER_DOOR_PARA_REQ, *LP_INTER_DOOR_PARA_REQ;

// -------------------------------------------------------------------------------�Ž���ǰ��״̬
typedef struct
{
    char byDevNo;							// �Ž�����������id����Χ��1,2,3��255
    char byDoorNo;							// ��ID����Χ��1,2,3��255
    char byDoorStatus;						// �ŵ�ǰ״̬��e.g.: �򿪣��ر�
    char byDoorContactDetectionMode;			//�Ŵż�ⷽʽ����(��·)(Ĭ��)
    char byDoorLockRelayStatus;				// �����̵���״̬����(����) 
    char resv[3];							// ����

}INTER_DOOR_STATUS,*LP_INTER_DOOR_STATUS;

//--------------------------------------------------------------------------------���͵�ǰ�Ž�����
typedef struct 
{
    INTER_SXD_HEAD sxdHeader;
    SX_INTER_DOOR_PARA_REQ sxddoorpara;
}INTER_SXD_DOOR_PARA_SEND;

//--------------------------------------------------------------------------------���͵�ǰ�Ž�״̬
typedef struct 
{
    INTER_SXD_HEAD sxdHeader;
    INTER_DOOR_STATUS sxddoorstate;
}INTER_SXD_DOOR_STATUS_SEND;

//-------------------------------------------------------------------------------- ״̬��ѯ��������
typedef enum
{
    ALARMCTL_DEV_RUNNING_STATE=0,		// �豸����״̬
    ALARMCTL_DEV_PATROL_CHECK,			// Ѳ��
    ALARMCTL_DEV_WORK_STATE,			// ��������ѯ����
    ALARMCTL_DEV_STATE_CMD_NUM,	
}ALARMCTL_DEV_STATE_CMD_TYPE;

// -------------------------------------------------------------------------------   ״̬����
typedef enum 
{
    ALARMCTL_DEV_CLOSED=0,				// �豸�ر�
    ALARMCTL_DEV_OPENED,				// �豸����
} ALARMCTL_DEV_STATE;

typedef enum
{
    ALARMCTL_DEV_DISARMED = 0,			// δ����
    ALARMCTL_DEV_ARMED ,					// �Ѳ���
} ALARMCTL_DEV_DEFENCE_STATE;

typedef struct 
{
    int byAlarmState;						//��������״̬���ĸ����������ˣ�
    int byBypassState;						//������·״̬���ĸ���������·�ˣ�
    int byDefenceAlarmType;					//�������͵ı���(��ͨ��������������)
} ALARMCTL_DEV_ALARM_STATE;

//-----------------------------------------------------------------------------------��ȡ����������״̬
typedef struct
{
    char byDevNo;		            				// ������������id
    char byCmdType;	          				  // ��ѯ��������:Ѳ��
    char byDevState;                        			  //ȡֵ��ALARMCTL_DEV_STATE
    char byDevDefenceState;       				 //ȡֵ��ALARMCTL_DEV_DEFENCE_STATE
    char byRsv[4];			    				 // ����
    ALARMCTL_DEV_ALARM_STATE  byAlarmState;
} SXC_INTER_ALARMCTL_DEV_GET_STATE, *LP_SXC_INTER_ALARMCTL_DEV_GET_STATE;

//-----------------------------------------------------------------------------------���͵�ǰ������״̬
typedef struct 
{
    INTER_SXD_HEAD sxdHeader;
    SXC_INTER_ALARMCTL_DEV_GET_STATE sxdalarmstate;
} INTER_SXD_ALARM_STATUS_SEND;

//------------------------------------------------------------------------------------����������������������
typedef enum
{
    ALARMCTL_DEV_OPEN=0,					// ������
    ALARMCTL_DEV_CLOSE,					// �ر�����
    ALARMCTL_DEV_ARMING,        				 //��������
    ALARMCTL_DEV_DISARMING,				// ��������
    ALARMCTL_DEV_FIRE_POLICE,			// ��������
    ALARMCTL_DEV_RESET,					// ��λ����
    ALAEMCTL_DEV_OTHER_DISARMING,		// ��·����
    ALARMCTL_DEV_CMD_NUM,

} ALARMCTL_DEV_CMD_TYPE;

//--------------------------------------------------------------------------------------��������������
typedef struct
{
    char byDevNo;							// ��������������id
    char byAreaNo;							// ��������������id
    short wGuardNo;						// ��������������id��1-16
    char byCmdType;						// ������������:��+�ر�+ ����+ ����+ ����+��·
    char byRsv[3];	
} SXC_INTER_ALARMCTL_DEV_CTL_CMD;

//-------------------------------------------------------------------------------------- ������������������
typedef struct 
{
    char byAlarmHostID;						// ��������ID
    char byEnterDelay;						// ������ʱʱ��10-150s
    char byOutDelay;						// �����ʱʱ��10-300s
    char byAlarmDelay;						// ������ʱʱ��2���ӣ�5���ӣ�10���ӣ�15���ӣ�							//  30����
}SXC_INTER_ALARMCTL_SET_PARAM_REQ;

//---------------------------------------------------------------------------------------�����������
typedef struct
{	
    INTER_SXD_HEAD sxdHeader;
    int  alarm_state;	
}SXC_INTER_ALARM_OUT_STATE;

//---------------------------------------------------------------------------------------�����������
typedef struct
{
    int channel;   //��������˿ڣ���0��ʼ����ʾ����������������˿ڣ�0xFF��ʾ����ȫ�������
}SXC_INTER_ALARM_OUT_CTRL;

// ---------------------------------------------------------------------------------------�������������
typedef struct
{ 
    //��ʼʱ�� 
    char byStartHour;						// ʱ
    char byStartMin; 						// ����
    //����ʱ�� 
    char byStopHour; 						// ʱ
    char byStopMin; 							// ����
}INTER_SCHEDTIME,*LPINTER_SCHEDTIME;

typedef struct
{ 
    char byAlarminNo;						// ���������ͨ���š�
    char byAlarmInDelay;						// ������ʱ����Χ�� 0-60S
    char byAlarmType; 						// ���������͡�0������,1������  
    char byAlarmInHandle; 					// �Ƿ��� 
    char sAlarmInName[16];					// ���ơ�
    char byRsv[16];							// ����
    // ����ʽ��Ĭ�ϴ���ֵ���������ϴ���������Ӧ�ı������ͨ��
    //INTER_HANDLEEXCEPTION struAlarmHandleType; 

    // ����ʱ�䡣7��4��ʱ��Σ�ÿ��ĵ�һ��ʱ���00��00 ~23:59
    INTER_SCHEDTIME struAlarmTime[7][4];	 

    int dwPtzPathNo;						// ����������ͼ��ͨ���ţ�0~31��

    // 0xff����ͼ��ȱʡ����1��1
    char byPresetNo; 						// ���õ���̨Ԥ�úţ�1-128��Ч��

    // 0xff��ʾ������Ԥ�õ㡣
    int dwRecordChan; 						// ����������¼��ͨ����λ��

    // bit0��ͨ��1��bit1��ͨ��2��
    // bit2ͨ��3��bit3��ͨ��4����bit
}INTER_MMS_ALARMINCFG,*LPINTER_MMS_ALARMINCFG;



//---------------------------------------------------------------------------------------�������������
typedef struct
{ 
    char byAlarmoutNo;					// ���������ͨ���š�
    char sAlarmOutName[16];				// ���� 16�ֽ� 
    char reserved[16];					// ���� 
    int dwAlarmOutDelay;				// �����ʱ(-1��0xf����Ϊ���ޣ� 
    // �ֶ��ر�)   [0~600S]   5S��
    // ����������������ӳ�ʱ��

    // �����������ʱ���;7��4��ʱ��Σ�����һ��ʾ��һ�죬
    // ÿ��ĵ�һ��ʱ���00��00 ~23:59��������Ϊȫ��ʱ���͵�
    // һ��ʱ��� 00��00-24��00��ʾȫ�죬
    // ����3��ʱ�����00��00-00��00	
    INTER_SCHEDTIME 	struAlarmOutTime[MAX_DAYS][MAX_TIMESEGMENT];
}INTER_ALARMOUTCFG,*LPINTER_ALARMOUTCFG;


//---------------------------------------------------------------------------------------����ģʽ��������info
typedef struct
{
    int client_id;
    char mode;
    char user_table_idx;
    char *name;
    char *pwd;
    int ip;
} T_ACTIVE_CONTEXT;

//---------------------------------------------------------------------------------------�������
typedef struct
{ 
    int  dwSize;
    int dwDeviceIP;					//�豸 IP��ַ, Ĭ��ֵ��192.168.10.64
    //�޸ĺ�������Ч 
    int dwDeviceIPMask;				//�豸 IP��ַ����, Ĭ��ֵ��255.255.255.0
    //�޸ĺ�������Ч 
    int dwNetInterface;				//����ӿڲ�����
    short wDevicePort;					// tcp�˿ں� ��Ĭ��Ϊ10000
    char byMACAddr[6]; 				// �����ַ��ֻ������ʾ 6�ֽ�, ������
    int dwGatewayIP; 					//���ص�ַĬ��ֵ��192.168.10.100
    int dwManageHostIP;				// Զ�̹���������ַ������ģʽ��������
    // ��IP��ַ, Ĭ��ֵ��122.200.70.151
    short wManageHostPort; 				// Զ�̹��������˿ںţ�Ĭ��Ϊ10000 
    short wRev;
    int dwCataSvrIP1;					// Ŀ¼������ip1�˿ڹ̶�Ϊ1008��
    // Ŀ¼������IP��������
    // ��Զ��ϵͳ����Э��_��꡷
    int dwCataSvrIP2;					// Ŀ¼������ip2 �˿ڹ̶�Ϊ1008��
    // WEB����˿ڹ̶�Ϊ80
    char PPPOEUserName[16];			// PPPOE�û���
    char reserved[16];					// ����
    char PPPOEPassWord[16];			// PPPOE����
    char reserved2[16];				// ����
    int    PPPOEIP;						// �����裬��ȡ����				
    char PPPOEUSEDTYPE;				// 0-��ʹ�� 1-�������� 2-��������
    //�����������ϡ�����������ʶ�� 
    // 3-��������
    int  MultiCastAddr;
    char byNetMode;					// ��������0�������� 1��PPPOE  2��CDMA
    // ����������Ϊ0ʱPPPOEUSEDTYPE ����ѡ��
    // ǿ��Ϊ 1-�������ߡ�����������Ϊ1: 								
    // PPPOEPassWord PPPOEPassName�����ã�
    // ���಻������
    char active_mode_name[16];		// ����ģʽ-��֤�û��� 
    char reserved3[16];					// ����
    char active_mode_pwd[16];			// ����ģʽ-��֤�û����� 
    char reserved4[32];					// ����
}INTER_NETCFG,*LPINTER_NETCFG;

//---------------------------------------------------------------------------------------�������+�޸ı�־λ:0δ���� 1 ������
typedef struct
{
    int flag;
    INTER_NETCFG para;
}INTER_NETCFG_FILE,*LPINTER_NETCFG_FILE;

//-------------------------------------------------------------------------------------��������������ݰ�
typedef struct{
    INTER_SXD_HEAD sxdHeader;
    INTER_NETCFG  para_net_send;
}INTER_NETCFG_SEND,*LPINTER_NETCFG_SEND;

//---------------------------------------------------------------------------------------�豸����
typedef enum  
{
    LINK_DEV_UNDEFAULT = 0,
    LINK_DEV_MATRIX, 					// ����
    LINK_DEV_ALARM_CTL,				// ����������
    LINK_DEV_DOOR_CLT,				// �Ž�������
    LINK_DEV_CRADLE_HEAD,			// ��̨
    LINK_DEV_TYPE_NUM, 
}SERIAL_LINK_DEV_TYPE;

// --------------------------------------------------------------------------------------RS232�������� 
typedef struct
{ 
    int dwBaudRate;						// ������
    char byDataBit; 						// ����λ�� 0-5λ��1-6λ��2-7λ��3-8λ 
    char byStopBit;						// ֹͣλ��0-1λ��1-2λ
    char byParity; 						// У�顣0-��У�飬1-��У�飬2-żУ�� 
    char byFlowcontrol;					// 0-�ޣ�1-�����أ�2-Ӳ���� 
    int dwRsv[2];
}RS232_ATTR;

// ---------------------------------------------------------------------------------------����Э������
typedef enum  
{
    MATRIX_PROTOCOL_UNKNOW=0, 
    MATRIX_PROTOCOL_PELCO,        
    MATRIX_PROTOCOL_AD,
    MATRIX_PROTOCOL_SX,
    MATRIX_PROTOCOL_NUM, 
}MATRIX_PROTOCOL_TYPE;

//------------------------------------------------------------------------------------------����������Э������
typedef enum  
{
    ALARM_CTL_PROTOCOL_UNKNOW=0,        
    ALARM_CTL_PROTOCOL_CK2316,        
    ALARM_CTL_PROTOCOL_VISTA120,        
    ALARM_CTL_PROTOCOL_NUM, 
}ALARM_CLT_PROTOCOL_TYPE;

//------------------------------------------------------------------------------------------�Ž�������Э������
typedef enum  
{
    DOOR_CTL_PROTOCOL_UNKNOW=0,        
    DOOR_CTL_PROTOCOL_COSON,        
    DOOR_CTL_PROTOCOL_WESTINGHOUSE,        
    DOOR_CTL_PROTOCOL_NUM, 
}DOOR_PROTOCOL_TYPE;

// ------------------------------------------------------------------------------------------�豸ѡ����
typedef struct
{
    // 0:�����豸 SERIAL_LINK_DEV_TYPE;
    char byDevType;		

    // 1:���� 2:���������� 3:�Ž���������4���ƾ�// ������
    char byDevNo;// �豸�� 

    char byDevProtocol;	// �豸Э�飬�������ö������
    // e.g. ���Ϊ�����豸��Ϊ1��
    // Э��ʹ�MATRIX_PROTOCOL_TYPE
    // ��ѡ������������
}SERIAL_LINK;

//------------------------------------------------------------------------------------------- �������Ͷ���
typedef struct
{
    char byPathNo;					// ���ں�
    char byRsv[3];					// ����
    SERIAL_LINK  serialLink;			// ѡ�������豸
    RS232_ATTR  serialAttr;			// �����豸�Ĵ�������
    int dwRsv[1];
}SERIAL_PAMATER;
//-------------------------------------------------------------------------------------------�����͵Ĵ��ڲ���
typedef struct
{
    INTER_SXD_HEAD sxdHeader;
    SERIAL_PAMATER com_para;
}SERIAL_PAMATER_SEND;

//-------------------------------------------------------------------------------------------- �Ž���������

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

// ---------------------------------------------------------------------------------------------�Ž�����
typedef struct
{
    char byDevNo;						// �Ž�����������id����Χ��1,2,3��255
    char byDoorNo;						// ��ID����Χ��1,2,3��255
    char byDoorAlmType;					// SXC_DOOR_SYS_ALARM_TYPE�Ž�
    char byRsv [9];						//����
}SXC_DOOR_SYS_ALM_INFO;

//-------------------------------------------------------------------------------------------- ������������
typedef struct
{
    char byAlarmCtlID;					// ������������ID
    char byHostAlmType;					// �������ͣ�0-��ͨ������1-�������� 02  �ޱ�����
    char byRsv[2];						// ����
    int dwAlarmID;						// ������
    ;									// ����
}SXC_ALARM_HOST_INFO;

//---------------------------------------------------------------------------------------------- ����������
typedef struct
{
    int byChannelID;						// ������ͨ���� 
    int byStatus;						// ��������״̬ 0-�仯��1-δ�仯
    char byRservered[4];					// ����
}SXC_ALRAM_SWITCH_IN;

typedef  union tag_alarm_ctl{
    SXC_DOOR_SYS_ALM_INFO sDoorSysAlm;
    SXC_ALARM_HOST_INFO   sAlarmHost;
    SXC_ALRAM_SWITCH_IN   sSwitchIn;
}alarmCtl_S;

//----------------------------------------------------------------------------------------------- �����ϴ���������
typedef struct
{
    char Year;							// �ꡣ��2000��Ĳ�ֵ����Χ00~99��
    // ���磺09����ʾ2009�꣬
    char Month;							// �¡���Χ��һ��12���·�
    char Day;							// �ա� �����·���ȷ������Χ��1~31
    // 2�·���Ҫ�����������������
    char Hour; 							// ʱ����Χ�� 0~23
    char Minute;						// �֡���Χ�� 0~59
    char Second;						// �롣��Χ�� 0~59
    char byDevID;						// �豸ID
    char byAlmType;						// �Ž���������������������
    alarmCtl_S alarmCtl ;
    char byReserved[4];					// ����
}INTER_DOOR_ALMOUT, *LP_INTER_DOOR_ALMOUT;
//------------------------------------------------------------------------------------------------�����ϴ����ݰ�
typedef struct
{
    INTER_SXD_HEAD sxdHeader;
    INTER_DOOR_ALMOUT alarm_para;	
}INTER_DOOR_ALMOUT_SEND;

//-------------------------------------------------------------------------------------------------������������ 
typedef struct 
{
    char   byAlarmInType;				// ��������ı������͡�ȡֵ1-9
    int      byAlarmLinkOutChan;   			  // �������������ͨ���š���8λ��ĳһλΪ1��ʾ������ͨ����
}ALARM_LINKAGE_PARA;
//-------------------------------------------------------------------------------------------------�������ò�������
typedef struct
{
    INTER_SXD_HEAD sxdHeader;
    ALARM_LINKAGE_PARA alarm_linkage_para;
}ALARM_LINKAGE_PARA_SEND;
//�����������ʱ������ 
typedef struct 
{
    char  byAlarmOutChan;     // ���������ͨ���š�ȡֵ0-8��0��ʾ��������ͨ���ı����������ʱ�䣻1-8��ʾ��Ӧͨ���ı����������ʱ�䣻���Ϊ��ȡʱ��ͨ���Ų���Ϊ0�����ʱ��Ϊ0ʱ0�֣����ʾ��ͨ���ı���������ó�����
    char  byHour;           		  // �����������ʱ�䣨Сʱ�����Ϊ24Сʱ
    char  byMinute;	         	 // �����������ʱ�䣨���ӣ�
    char  byReserved;
}ALARM_OUTPUT_DURATION_CFG_V1;

//-------------------------------------------------------------------------------------------------�����������ʱ�䷢��
typedef struct
{
    INTER_SXD_HEAD sxdHeader;
    ALARM_OUTPUT_DURATION_CFG_V1 linkage_time;
}ALARM_OUTPUT_DURATION_CFG_SEND;

//---------------------------------------------------------------------------------------------------������������͵�ַ����
typedef struct
{
    char byPassword[4];  					 //����������ģ����̵��û����룬4λ���붼Ϊ0-9֮�������
}SXC_ALARMCTL_PWD_PARA;

typedef struct
{
    char byAddress; 						  //����������ģ����̵ĵ�ַ��0-15��
}SXC_ALARMCTL_ADDR_PARA;


typedef struct
{ 
    //��ʼʱ�� 
    char byStartHour;					// ʱ
    char byStartMin; 					// ����
    //����ʱ�� 
    char byStopHour; 					// ʱ
    char byStopMin; 						// ����
} SX_INTER_SCHEDTIME,*LPSX_INTER_SCHEDTIME;

//---------------------------------------------------------------------------------------- �������������
typedef struct
{ 
    char byAlarminNo;					// ���������ͨ���š�
    char sAlarmInName[16];				// ���ơ�
    char reserved[16];		   			 //����
    char byAlarmType; 					// ���������͡�0������,1������
    char byAlarmInHandle; 				// �Ƿ��� 
    //SX_INTER_HANDLEEXCEPTION struAlarmHandleType;	//����ʽ  Ĭ�ϴ���ֵ���������ϴ���������Ӧ�ı������ͨ��
    SX_INTER_SCHEDTIME struAlarmTime[7][4]; // ����ʱ�� 7��4��ʱ��� ÿ��ĵ�һ��ʱ���00��00 ~23:59	 
    int  byPtzPathNo;					// ����������ͼ��ͨ���ţ�0~31��
    // 0xff����ͼ��ȱʡ����1��1
    char byPresetNo; 					// ���õ���̨Ԥ�úţ�1-128��Ч��
    // 0xff��ʾ������Ԥ�õ㡣
    int dwRecordChan; 					// ����������¼��ͨ����λ��
    // bit0��ͨ��1��bit1��ͨ��2��
    // bit2ͨ��3��bit3��ͨ��4����bit
    char byAlarmInDelay;					// ������ʱ����Χ�� 0-60S
} SX_INTER_ALARMINCFG,*LPSX_INTER_ALARMINCFG;

//-------------------------------------------------------------------------------------------- �豸��������
typedef struct 
{
    char byDevID;					// �豸ID
    char byRsv[3];					// ����
    char byDevSerialNum[128];		// �豸���к�
    char bySoftVer;  					// ����汾��
    char byHardVer;  				// Ӳ���汾��
    char byCommProVer;				// ͨ��Э��汾
    char bySerialNum;				// ���ڸ���
    char  rsv2[8];
} SX_DEVICE_WORKING_STATUS, LP_SX_DEVICE_WORKING_STATUS;
typedef struct 
{
    INTER_SXD_HEAD sxdHeader;
    SX_DEVICE_WORKING_STATUS para;
} SX_DEVICE_WORKING_STATUS_SEND, LP_SX_DEVICE_WORKING_STATUS_SEND;

typedef struct
{ 
    char  Year;	// �ꡣ��2000��Ĳ�ֵ����Χ00~99��
    // ���磺09����ʾ2009�꣬
    char  Month;	// �¡���Χ��һ��12���·�
    char  Day;	// �ա� �����·���ȷ������Χ��1~31
    // 2�·���Ҫ�����������������
    char  Hour; 	// ʱ����Χ�� 0~23
    char  Minute;	// �֡���Χ�� 0~59
    char  Second;	// �롣��Χ�� 0~59
    char	rev[2];	// ������
}INTER_TIME,*LPINTER_TIME;

/*---------------��������-----------------------------------------------------------------*/
extern int server_listen_fd ,server_connect_fd ;
INTER_NETCFG para_net;//�����������
extern char *net_para_file_path;
extern int if_have_net_client_connect;
extern user_info *head;
extern  FILE *fp_dev_config_file;

/*---------------��������-----------------------------------------------------------------*/
void *pthread_server_passive(void *arg);     //����ͨ��ģʽ�߳�
void *pthread_server_active(void *arg);        //����ģʽͨ���߳�
void *pthread_handshake(void *arg);            //˫�������߳�
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

extern int log_in_reply(char *recv_para,INTER_SXC_HEAD *data_head,int client_fd);//��½����ظ�

extern int login_apply(void );                                                                           //����ģʽ��¼����

extern int main_service_ctl(void);

extern user_info *link_insert(int fd);

extern  void link_print(user_info *head);

extern  user_info * link_delete(int fd);

extern int cradle_ctrl_reply(char *recv_para,INTER_SXC_HEAD *data_head,int fd);

extern int MATRIX_ctrl_change(char *recv_para,INTER_SXC_HEAD *data_head,int fd);

extern int MATRIX_ctrl_cancel_singal(char *recv_para,INTER_SXC_HEAD *data_head,int fd);

extern int MATRIX_ctrl_cancel_all(char *recv_para,INTER_SXC_HEAD *data_head,int fd);

extern int entrance_open_close_ctrl(char *recv_para,INTER_SXC_HEAD *data_head,int fd);//�Ž�����

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

