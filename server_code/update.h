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
������ͷ �� 
*/

typedef enum tag_protocal_type 
{
	UPDATE_REQUEST, //��������
	UPDATE_START,    //������ʼ
	UPDATE_DATA,     //��������
	UPDATE_CTL,      //��������
	UPDATE_REFRESH_PROGE,      //��������
	UPDATE_STATE_UP,      //����״̬�ϴ�
	UPDATE_STATE_GET,      //����״̬��ȡ
	
	MAX_UPDATE_TYPE
}E_UPDATE_PROTOCOL_TYPE;

typedef struct tag_update_protocol_head
{
	BYTE byType;           //����
	BYTE byRsv[3];         //���
	DWORD dwResult;        //ָ��������豸����ʱʹ�ã�����μ�������
	DWORD dwLen;           //���ݳ���
}T_UPDATE_PROTOCOL_HEAD, *LP_T_UPDATE_PROTOCOL_HEAD;


/*
�������� �� ������SX_DEVICE_VERSION_INFO
					
SDK��Device��Я������
Device- SDK��Я��������
*/

/*
�豸���� �� 
*/
typedef enum tag_device_type
{
    DEVICE_DVS, //DVS
    DEVICE_EVR, //EVR   
    DEVICE_AV,  //��Ƶ����������

    MAX_DEVICE_TYPE
}E_DEVICE_TYPE;

/*
��Ƶ�����������ӳ������� �� 
*/

typedef enum tag_sx_av_packet_type
{
//���ص�Ԫ
	PACKET_MAIN_UNIT_TYPE_START = 0, //�����ʾ����
	PROG_MAIN_UNIT, //������
	//------�������������ڴ�����--<����˵�����Ŀ���ʽ>
	PACKET_MAIN_UNIT_TYPE_END, //�����ʾ����
	
//������Ԫ
	PACKET_ALARM_UNIT_TYPE_START = 0xE0, //�����ʾ����
	PROG_ALARM_UNIT,  //������
	PACKET_ALARM_UNIT_TYPE_END, //�����ʾ����
	
}SX_AV_PACKET_TYPE;

typedef struct tag_sub_prog_attr
{
	BYTE byType;            //����--SX_AV_PACKET_TYPE
	BYTE byRsv[3];          //�ֽڶ��뱣����
	DWORD dwTime;           //�ӳ�������ʱ��-time_t	
	DWORD dwSize;           //�ӳ����С	
	DWORD dwVersion;  	    //�汾��
}SX_SUB_PROG_ATTR_INFO, *LP_SX_SUB_PROG_ATTR_INFO;

typedef struct tag_previous_update_state
{ 	
	BYTE dwPausedFlag;    //�����жϱ�ʾ
	BYTE dwRsv[3];         //������-������
	DWORD dwFileID;        //�ж������ļ���Id�ţ�����ƥ�� 
	DWORD dwTime;          //�ж������ļ����������ڣ�����ƥ��
	DWORD dwUpdatedSize;  //�Ѿ����յ����ļ��Ĵ�С
}SX_PREVIOUS_UPDATE_STATE, *LP_SX_PREVIOUS_UPDATE_STATE;

/*
 typedef struct tag_device_version_info
{ 	
	DWORD dwDeviceType;  	//�豸����
	DWORD dwHardVersion;    //Ӳ���汾
	DWORD dwSoftVersion;    //����汾
	
	SX_PREVIOUS_UPDATE_STATE tPreviousState;  	
	DWORD dwSubProgSum;  	//�豸��ǰ�ж��ٸ��ӳ���
	
	//�ɱ䳤�ȿ�������Ϣ dwLibSum * sizeof(SX_SUB_PROG_ATTR_INFO)
}SX_DEVICE_VERSION_INFO, *LP_SX_DEVICE_LIB_INFO;
*/


/*
��ʼ����
							������SX_UPDATE_ATTR
SDK��Device�� Я������
Device- SDK��  ��Я������
*/

/*
�������� �� 
*/
typedef enum tag_update_type
{
    RESET_UPDATE, //���´���
    IMPLEMENT_UPDATE, //�ϵ㴫��

    MAX_UPDATE_TYPE_DUMMY
}E_UPDATE_TYPE;


typedef struct tag_update_file_cxt
{
	DWORD dwFileID;       //����������ID�ţ���������û��ϵ㴫��
	DWORD dwTime;         //�����ļ������ڣ��û��ϵ㴫��
	DWORD dwSize;            //�������ݵĴ�С
	DWORD dwSubProgSum;     //�ӳ������Ŀ
	//�����ӳ�������ݡ���СΪdwSize  * sizeof(SX_SUB_PROG_ATTR_INFO)
}SX_UPDATE_FILE_CXT, *LP_SX_UPDATE_FILE_CXT;


typedef struct tag_update_attr
{
	BYTE byType;            //����---E_UPDATE_TYPE
	BYTE byRsv[3];          //�ֽڶ��뱣����

	SX_UPDATE_FILE_CXT  tFileCxt;//���Ϊ����������ģʽ����˽ṹ��Ч

}SX_UPDATE_ATTR, *LP_SX_UPDATE_ATTR;


/*
��������
������SX_UPDATE_DATA
SDK��Device�� Я������
Device- SDK��  ��Я������
*/

typedef struct tag_update_date
{
	DWORD dwPackId;   //�����ݰ��ı�ţ���1��ʼ 
	DWORD dwPackLen;  //�����ݰ��ĳ���
	
//���ݰ�����, ����Ϊ dwPackLen

}SX_UPDATE_DATA, *LP_SX_UPDATE_DATA;

/*
��������
������SX_UPDATE_CTL
*/

typedef enum tag_update_ctl_type
{
    CANCELED_UPDATE,  //ȡ������
    PAUSED_TRANSFER,  //��ͣ����

    MAX_UPDATE_CTL_TYPE
}E_UPDATE_CTL_TYPE;

typedef struct tag_update_ctl
{
	DWORD dwPackNum;   //�����ݰ��ı�ţ���1��ʼ 
	
}SX_UPDATE_CTL, *LP_SX_UPDATE_CTL;

/*
��������
������SX_UPDATE_PROGRESS
*/

typedef struct tag_update_progress
{
	DWORD dwRefreshSize;   //���µ��ֽ���
	BYTE  byPct;             //���µİٷֱ�
	BYTE  byRsv[3];          //����

}SX_UPDATE_PROGRESS, *LP_SX_UPDATE_PROGRESS;


/*
����״̬�ϴ�
������SX_UPDATE_STATE
SDK��Device�� ��Я������
Device- SDK��  Я������
*/

typedef struct tag_update_state
{
	DWORD dwState;   
}SX_UPDATE_STATE, *LP_SX_UPDATE_STATE;


/*
����״̬��ȡ
������SX_UPDATE_STATE 
SDK��Device�� ��Я������
Device- SDK��  Я������
*/

/*
�����붨��
*/
#define AV_UPDATE_NO_RIGHT       0xEE000001 //��Ȩ��
#define AV_UPDATE_DATA_ERR       0xEE000002 //���ݴ���
#define AV_UPDATE_VER_ERR        0xEE000003 //�汾����
#define AV_UPDATE_REFRESH_ERR    0xEE000004 //�汾���´���
#define AV_MEMORY_LACK           0xEE000005 //�ڴ治��
#define AV_DOWNLOAD_FAIL         0xEE000006 //���ذ汾ʧ��
#define AV_UP_STATE_ERR          0xEE000007 //��ǰ״̬�����������	
#define AV_UP_FLASH_ERR          0xEE000008 //flash�����쳣	


/*
�ļ�ͷ
���ܣ��������У��
*/


typedef struct tag_update_head
{
	BYTE tag[4];   //�ļ���tagֵ-��s' 'x' 's' 'j'
	DWORD dwDeviceType;   //��������Ӧ���豸����
	DWORD dwFileID;       //����������ID�ţ���������û��ϵ㴫��
	DWORD dwTime;         //�����ļ������ڣ��û��ϵ㴫��
	DWORD size;    //��������С������������ͷ
	BYTE md5[32];  //�������ݵ�md5У��,������ͷ��ʼ����
	DWORD dwDataHeadOffset; //����ͷ��ƫ�Ƶ�ַ����ǰΪsizeof(SX_UPDATE_FILE_HEAD)
	DWORD dwHardVersion;    //�ʺϵ�Ӳ��ƽ̨�汾��
	DWORD dwSDKVersion;     //�ð汾��Ӧ��SDK�汾��	
	BYTE  byRev[4];          //������
}SX_UPDATE_FILE_HEAD, *LP_SX_UPDATE_FILE_HEAD;

/*
����ͷ
���ܣ����ݵ���������
*/


typedef struct tag_sub_packet_head
{
	DWORD dwOffset; //�����ļ��е�ƫ�Ƶ�ַ
	SX_SUB_PROG_ATTR_INFO attr;
}SX_SUB_PACKET_HEAD, *LP_SX_SUB_PACKET_HEAD;


/*
������������ͷ
���ܣ�ƥ��
*/

typedef struct tag_update_date_head
{
	DWORD dwSoftVersion;     //���������汾��
	DWORD dwUpdateLibSum;  	 //Ҫ�����Ŀ������
	
	//�ɱ��Ӱ���ͷ dwLibSum * sizeof(SX_SUB_PACKET_HEAD)

}SX_AV_UPDATE_DATA_HEAD, *LP_SX_AV_UPDATE_DATA_HEAD;

typedef struct
{
    DWORD dwHardVersion;   	//Ӳ���汾�š���16λ�����汾,��16λ�Ǵΰ汾 ��ǰ�汾 0x00010002��v1.2�����ͻ�����ʾΪv1.2����ʽ
    DWORD dwSoftVersion;  	
    DWORD dwRev;       				//����������汾��չ
    DWORD dwARMVersion;             //ARM����汾���汾�����ʽͬ������汾����
    DWORD dwEncodeDSPVersion;       //����DSP����汾���汾�����ʽͬ������汾����
    DWORD dwDecodeDSPVersion;       //����DSP����汾���汾�����ʽͬ������汾����
    BYTE INSoftVersion [12];       		
    //�ڲ�����汾=ARM�汾+����DSP����汾+����DSP����汾
    	DWORD dwSoftBuildDate;	//�����������,0xYYYYMMDD ���꣨��߶����ֽڣ������£��θ�һ���ֽڣ������գ����һ���ֽڣ�����ʮ������ʾ
    DWORD language; 					//����
    DWORD byAlarmOutPortNum; 		   	//DVS�����������
    DWORD TransFlag; 		   			
    //�������ݱ�־λ����λ��bit0��ARM�����־λ��0����ʾ���ݲ����䣬1����ʾ���ݴ��䣻bit1������DSP�����־λ��0����ʾ���ݲ����䣬1����ʾ���ݴ��䣻bit2������DSP�����־λ��0����ʾ���ݲ����䣬1����ʾ���ݴ���*/
    DWORD total_length;   				//�����ļ��ܳ���
    DWORD ARM_start_Addr;   		   	//ARM��ʼ��ַ
    DWORD ARM_length;   				//ARM�����ļ�����
    BYTE ARM_MD5[32];					//ARM�������ݵ�MD5����ֵ������У���������ݵ�������
    DWORD EncodeDSP_start_Addr;   		//����DSP��ʼ��ַ
    DWORD EncodeDSP_length;   			//����DSP�����ļ�����
    BYTE EncodeDSP_MD5[32];			    //����DSP�������ݵ�MD5����ֵ������У���������ݵ�������
    DWORD DecodeDSP_start_Addr;   		//����DSP��ʼ��ַ
    DWORD DecodeDSP_length;   			//����DSP�����ļ�����
    BYTE DecodeDSP_MD5[32];			    //����DSP�������ݵ�MD5����ֵ������У���������ݵ�������
}SX_AV_UPDATE_VERIFY_HEAD, *LP_SX_AV_UPDATE_VERIFY_HEAD;


