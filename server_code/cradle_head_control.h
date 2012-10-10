#ifndef _CRADLE_HEAD_CONTROL_H
#define _CRADLE_HEAD_CONTROL_H


#define CRADLE_HEAD_CONTROL_NO_VALID_COMMAND         0xff
#if 0
//��̨�������--����
#define PTZ_UP                  0   //��SS�ٶ�����
#define PTZ_DOWN                1   //��SS�ٶ�����
#define PTZ_LEFT                2   //��SS�ٶ�����
#define PTZ_RIGHT               3 //��SS�ٶ�����
#define PTZ_APERTURE_BIG        4  //��Ȧ���ڣ����
#define PTZ_APERTURE_SMALL      5   //��Ȧ���ڣ���С
#define PTZ_FOCUS_IN            6  //�۽���������ǰ��
#define PTZ_FOCUS_OUT           7   //�۽�Զ��������
#define PTZ_ZOOM_IN             8   //�䱶��(��д, �Ŵ�)��������
#define PTZ_ZOOM_OUT            9  //�䱶��(���, ��С)�� �����С
#define PTZ_AUTO_ZOOM           10 //��ʼ�Զ����ࣨ�Զ����ʣ�
#define PTZ_AUTO_FOCUS          11  //���Զ�����
#define PTZ_AUTO_APERTURE       12  //���Զ���Ȧ
#define PTZ_SET_PTZBIT          13 //����Ԥ�õ�
#define PTZ_CLE_PTZBIT          14  //����Ԥ�õ�
#define PTZ_STOP_ALL            15  //ֹͣ��������������ͷ����̨������
#define AUX_PWRON1              16  //��������1��
#define AUX_PWROFF1             17 //��������1��
#define AUX_PWRON2              18 //��������2��
#define AUX_PWROFF2             19  //��������2��
#endif
//��̨�������--Э������
#define Palco_D   1
#define Palco_P   2
#define YAAN      3
#define Philips   4     //�����ָ�����ͨѶЭ�飨��Ҫ485������ת������
#define TOTA120   5     //TOTA������ͨѶЭ��
#define RV800     6     //����������ͨѶЭ��
#define S1601     7     //����Э��
#define CLT_168   8     //��ǧ���Э��
#define TD_500    9     //��ǧ�������ͨѶЭ��
#define V1200     10    //����V1200�����������ž���ͨѶЭ��
#define ZION      11    //�������� CBR2400
#define ANT       12    //����Э��
#define CBC       13    //�ձ�CBC������ͨѶЭ��
#define CS850A    14    //����CS850A������ͨѶЭ��
#define CONCORD   15    //2000b��ά��B01������concord���ܿ���ͨѶЭ��
#define HD600     16    //CCVE��·���ϵͳͨѶЭ��
#define SAMSUNG   17    //����641��643������ͨѶЭ��
#define PIH       18    //����PIH717������ͨѶЭ��
#define MG_CS160  19    //����B01ͨѶЭ��
#define WISDOM    20    //WISDOM������ͨѶЭ��
#define NEOCOM    21    //�Ϳ�ķЭ��


#define CRADLE_HEAD_SERIAL_PORT          2
#define CRADLE_HEAD_UART_BOARD           9600
#define CRADLE_HEAD_UART_DATA_BIT        3  //8bit
#define CRADLE_HEAD_UART_CHECK_BIT       0
#define CRADLE_HEAD_UART_STOP_BIT        0

typedef struct 
{
    int current_fd;
    unsigned int setup_command_set;         //�洢��������(��16λ��ÿһλ��ʾһ������) 
    unsigned char cradle_head_protocol_type;     //��̨Э������
    unsigned char cradle_head_address;      //��̨��ַ(1-255)
    unsigned char cradle_head_move_speed;   //��̨�ƶ��ٶ�(1-255)
    unsigned char cradle_head_preset_point; //��̨Ԥ�õ�(1-32, 33-64)
    SERIAL_PAMATER cradle_head_control_serial_pamater;
}cradle_head_control_arg;

cradle_head_control_arg cradle_head_control_data;

void *pthread_cradle_head_control(void *arg);

#endif

