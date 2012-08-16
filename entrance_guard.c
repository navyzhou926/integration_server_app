#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include "global.h"
#include "net.h"
#include "uart.h"
#include "entrance_guard.h"

#define DEBUG
#ifdef DEBUG
#define printf_debug(fmt, arg...) printf(fmt, ##arg)
#else
#define printf_debug(fmt, arg...) do{}while(0)
#endif

pthread_mutex_t entrance_guard_mutex = PTHREAD_MUTEX_INITIALIZER;

entrance_guard_arg entrance_guard_data = {NO, IS_OPENED, IS_CLOSED, IS_OPENED, 0, 0, 5, 5, NO, NO, 0, 0};

//host code
unsigned char search_entrance_guard_code[] = {0xfa,0xfa,0xfa,0x2a,0x01,0x00,0x02,0xcc,0x57};
unsigned char open_door_code[] = {0xfa,0xfa,0xfa,0x2a,0x03,0x00,0x63,0x20,0xc2,0x6e,0x1b};
unsigned char close_door_code[] = {0xfa,0xfa,0xfa,0x2a,0x03,0x00,0x63,0x20,0xc3,0x7e,0x3a};


/* init setup */
//门磁  常开+短路检测模式+扫描间隔为0秒
unsigned char door_contact_default_open_setup_code[] = {0xfa,0xfa,0xfa,0x2a,0x0b,0x00,0x24,0x00,0x08,0x89,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x9b,0xb9};
//门磁  常闭+短路检测模式+扫描间隔为0秒 (推荐)
unsigned char door_contact_default_close_setup_code[] = {0xfa,0xfa,0xfa,0x2a,0x0b,0x00,0x24,0x00,0x08,0xc9,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x6d,0x69};
//出门按钮 常开+不产生消息+无检测模式+扫描间隔0秒+联动对象：***门锁 (推荐+默认) 
unsigned char door_button_default_setup_code_1[] = {0xfa,0xfa,0xfa,0x2a,0x0b,0x00,0x24,0x01,0x08,0x80,0x00,0x42,0x00,0xb0,0x00,0x00,0x00,0xbd,0x74};
//出门按钮 常开+普通消息+无检测模式+扫描间隔0秒+联动对象：***门锁 
unsigned char door_button_default_setup_code_2[] = {0xfa,0xfa,0xfa,0x2a,0x0b,0x00,0x24,0x01,0x08,0x88,0x00,0x42,0x00,0xb0,0x00,0x00,0x00,0xa3,0xae};
//门锁  输出点状态---常开+普通消息+连续输出+5秒+无联动对象（默认）
unsigned char door_lock_default_open_setup_code[] = {0xfa,0xfa,0xfa,0x2a,0x0b,0x00,0x24,0x10,0x08,0x8a,0x05,0x00,0x00,0x00,0x00,0x00,0x00,0xbe,0xfd};
//门锁  输出点状态---常闭+普通消息+连续输出+5秒+无联动对象
unsigned char door_lock_default_close_setup_code[] = {0xfa,0xfa,0xfa,0x2a,0x0b,0x00,0x24,0x10,0x08,0xca,0x05,0x00,0x00,0x00,0x00,0x00,0x00,0x48,0x2d};
//客服端或NetKing软件设置门开保持时间 (1-255秒) (1-9,10,15,20,30,40,50,60,70,80,90,100,255)
unsigned char client_control_door_hold_time_code[CLIENT_CONTROL_DOOR_HOLD_TIME_COUNT][27] = {{0xfa,0xfa,0xfa,0x2a,0x13,0x00,0x26,0x20,0x10,0x00,0x00,0x02,0x01,0x1e,0x00,0x00,0x68,0x10,0x12,0x34,0x56,0x78,0x07,0x00,0x01,0x73,0x94},{0xfa,0xfa,0xfa,0x2a,0x13,0x00,0x26,0x20,0x10,0x00,0x00,0x02,0x02,0x1e,0x00,0x00,0x68,0x10,0x12,0x34,0x56,0x78,0x07,0x00,0x01,0xbc,0x31},{0xfa,0xfa,0xfa,0x2a,0x13,0x00,0x26,0x20,0x10,0x00,0x00,0x02,0x03,0x1e,0x00,0x00,0x68,0x10,0x12,0x34,0x56,0x78,0x07,0x00,0x01,0xf9,0x52},{0xfa,0xfa,0xfa,0x2a,0x13,0x00,0x26,0x20,0x10,0x00,0x00,0x02,0x04,0x1e,0x00,0x00,0x68,0x10,0x12,0x34,0x56,0x78,0x07,0x00,0x01,0x33,0x5a},{0xfa,0xfa,0xfa,0x2a,0x13,0x00,0x26,0x20,0x10,0x00,0x00,0x02,0x05,0x1e,0x00,0x00,0x68,0x10,0x12,0x34,0x56,0x78,0x07,0x00,0x01,0x76,0x39},{0xfa,0xfa,0xfa,0x2a,0x13,0x00,0x26,0x20,0x10,0x00,0x00,0x02,0x06,0x1e,0x00,0x00,0x68,0x10,0x12,0x34,0x56,0x78,0x07,0x00,0x01,0xb9,0x9c},{0xfa,0xfa,0xfa,0x2a,0x13,0x00,0x26,0x20,0x10,0x00,0x00,0x02,0x07,0x1e,0x00,0x00,0x68,0x10,0x12,0x34,0x56,0x78,0x07,0x00,0x01,0xfc,0xff},{0xfa,0xfa,0xfa,0x2a,0x13,0x00,0x26,0x20,0x10,0x00,0x00,0x02,0x08,0x1e,0x00,0x00,0x68,0x10,0x12,0x34,0x56,0x78,0x07,0x00,0x01,0x3d,0xad},{0xfa,0xfa,0xfa,0x2a,0x13,0x00,0x26,0x20,0x10,0x00,0x00,0x02,0x09,0x1e,0x00,0x00,0x68,0x10,0x12,0x34,0x56,0x78,0x07,0x00,0x01,0x78,0xce},{0xfa,0xfa,0xfa,0x2a,0x13,0x00,0x26,0x20,0x10,0x00,0x00,0x02,0x0a,0x1e,0x00,0x00,0x68,0x10,0x12,0x34,0x56,0x78,0x07,0x00,0x01,0xb7,0x6b},{0xfa,0xfa,0xfa,0x2a,0x13,0x00,0x26,0x20,0x10,0x00,0x00,0x02,0x0f,0x1e,0x00,0x00,0x68,0x10,0x12,0x34,0x56,0x78,0x07,0x00,0x01,0xf7,0xa5},{0xfa,0xfa,0xfa,0x2a,0x13,0x00,0x26,0x20,0x10,0x00,0x00,0x02,0x14,0x1e,0x00,0x00,0x68,0x10,0x12,0x34,0x56,0x78,0x07,0x00,0x01,0x25,0xee},{0xfa,0xfa,0xfa,0x2a,0x13,0x00,0x26,0x20,0x10,0x00,0x00,0x02,0x1e,0x1e,0x00,0x00,0x68,0x10,0x12,0x34,0x56,0x78,0x07,0x00,0x01,0xa4,0x72},{0xfa,0xfa,0xfa,0x2a,0x13,0x00,0x26,0x20,0x10,0x00,0x00,0x02,0x28,0x1e,0x00,0x00,0x68,0x10,0x12,0x34,0x56,0x78,0x07,0x00,0x01,0x10,0xc5},{0xfa,0xfa,0xfa,0x2a,0x13,0x00,0x26,0x20,0x10,0x00,0x00,0x02,0x32,0x1e,0x00,0x00,0x68,0x10,0x12,0x34,0x56,0x78,0x07,0x00,0x01,0x87,0xed},{0xfa,0xfa,0xfa,0x2a,0x13,0x00,0x26,0x20,0x10,0x00,0x00,0x02,0x3c,0x1e,0x00,0x00,0x68,0x10,0x12,0x34,0x56,0x78,0x07,0x00,0x01,0x03,0xdc},{0xfa,0xfa,0xfa,0x2a,0x13,0x00,0x26,0x20,0x10,0x00,0x00,0x02,0x46,0x1e,0x00,0x00,0x68,0x10,0x12,0x34,0x56,0x78,0x07,0x00,0x01,0xe3,0x4c},{0xfa,0xfa,0xfa,0x2a,0x13,0x00,0x26,0x20,0x10,0x00,0x00,0x02,0x50,0x1e,0x00,0x00,0x68,0x10,0x12,0x34,0x56,0x78,0x07,0x00,0x01,0x7a,0x93},{0xfa,0xfa,0xfa,0x2a,0x13,0x00,0x26,0x20,0x10,0x00,0x00,0x02,0x5a,0x1e,0x00,0x00,0x68,0x10,0x12,0x34,0x56,0x78,0x07,0x00,0x01,0xfb,0x0f},{0xfa,0xfa,0xfa,0x2a,0x13,0x00,0x26,0x20,0x10,0x00,0x00,0x02,0x64,0x1e,0x00,0x00,0x68,0x10,0x12,0x34,0x56,0x78,0x07,0x00,0x01,0x44,0xe2},{0xfa,0xfa,0xfa,0x2a,0x13,0x00,0x26,0x20,0x10,0x00,0x00,0x02,0xff,0x1e,0x00,0x00,0x68,0x10,0x12,0x34,0x56,0x78,0x07,0x00,0x01,0x23,0x09}};

//按开门按钮，门开保持时间(0-255秒)(1-9,10,15,20,30,40,50,100,255)
unsigned char door_button_control_door_hold_time_code[BUTTON_CONTROL_DOOR_HOLD_TIME_COUNT][19] = {{0xfa,0xfa,0xfa,0x2a,0x0b,0x00,0x24,0x10,0x08,0x8a,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x7f,0x3b},{0xfa,0xfa,0xfa,0x2a,0x0b,0x00,0x24,0x10,0x08,0x8a,0x02,0x00,0x00,0x00,0x00,0x00,0x00,0xa7,0xb9},{0xfa,0xfa,0xfa,0x2a,0x0b,0x00,0x24,0x10,0x08,0x8a,0x03,0x00,0x00,0x00,0x00,0x00,0x00,0x1f,0xd8},{0xfa,0xfa,0xfa,0x2a,0x0b,0x00,0x24,0x10,0x08,0x8a,0x04,0x00,0x00,0x00,0x00,0x00,0x00,0x06,0x9c},{0xfa,0xfa,0xfa,0x2a,0x0b,0x00,0x24,0x10,0x08,0x8a,0x05,0x00,0x00,0x00,0x00,0x00,0x00,0xbe,0xfd},{0xfa,0xfa,0xfa,0x2a,0x0b,0x00,0x24,0x10,0x08,0x8a,0x06,0x00,0x00,0x00,0x00,0x00,0x00,0x66,0x7f},{0xfa,0xfa,0xfa,0x2a,0x0b,0x00,0x24,0x10,0x08,0x8a,0x07,0x00,0x00,0x00,0x00,0x00,0x00,0xde,0x1e},{0xfa,0xfa,0xfa,0x2a,0x0b,0x00,0x24,0x10,0x08,0x8a,0x08,0x00,0x00,0x00,0x00,0x00,0x00,0x54,0xf7},{0xfa,0xfa,0xfa,0x2a,0x0b,0x00,0x24,0x10,0x08,0x8a,0x09,0x00,0x00,0x00,0x00,0x00,0x00,0xec,0x96},{0xfa,0xfa,0xfa,0x2a,0x0b,0x00,0x24,0x10,0x08,0x8a,0x0a,0x00,0x00,0x00,0x00,0x00,0x00,0x34,0x14},{0xfa,0xfa,0xfa,0x2a,0x0b,0x00,0x24,0x10,0x08,0x8a,0x0f,0x00,0x00,0x00,0x00,0x00,0x00,0x4d,0xb3},{0xfa,0xfa,0xfa,0x2a,0x0b,0x00,0x24,0x10,0x08,0x8a,0x14,0x00,0x00,0x00,0x00,0x00,0x00,0x31,0xe7},{0xfa,0xfa,0xfa,0x2a,0x0b,0x00,0x24,0x10,0x08,0x8a,0x1e,0x00,0x00,0x00,0x00,0x00,0x00,0xc2,0xa9},{0xfa,0xfa,0xfa,0x2a,0x0b,0x00,0x24,0x10,0x08,0x8a,0x28,0x00,0x00,0x00,0x00,0x00,0x00,0x3a,0x01},{0xfa,0xfa,0xfa,0x2a,0x0b,0x00,0x24,0x10,0x08,0x8a,0x32,0x00,0x00,0x00,0x00,0x00,0x00,0xfe,0x34},{0xfa,0xfa,0xfa,0x2a,0x0b,0x00,0x24,0x10,0x08,0x8a,0x64,0x00,0x00,0x00,0x00,0x00,0x00,0xb5,0x86},{0xfa,0xfa,0xfa,0x2a,0x0b,0x00,0x24,0x10,0x08,0x8a,0xff,0x00,0x00,0x00,0x00,0x00,0x00,0x62,0x2b}};


unsigned char handshake_base_code[] = {0xfa,0xfa,0xfa,0x2a,0x03,0x00,0x11,0x01,0x00,0x00,0x00};
signed char handshake_variable_code[][2] = {{0x14,0xaa},{0x04,0x8b},{0x34,0xe8},{0x24,0xc9},{0x54,0x2e},{0x44,0x0f},{0x74,0x6c},{0x64,0x4d},{0x95,0xa2},{0x85,0x83},{0xb5,0xe0},{0xa5,0xc1},{0xd5,0x26},{0xc5,0x07},{0xf5,0x64},{0xe5,0x45},{0x06,0x9b},{0x16,0xba},{0x26,0xd9},{0x36,0xf8},{0x46,0x1f},{0x56,0x3e},{0x66,0x5d},{0x76,0x7c},{0x87,0x93},{0x97,0xb2},{0xa7,0xd1},{0xb7,0xf0},{0xc7,0x17},{0xd7,0x36},{0xe7,0x55},{0xf7,0x74},{0x30,0xc8},{0x20,0xe9},{0x10,0x8a},{0x00,0xab},{0x70,0x4c},{0x60,0x6d},{0x50,0x0e},{0x40,0x2f},{0xb1,0xc0},{0xa1,0xe1},{0x91,0x82},{0x81,0xa3},{0xf1,0x44},{0xe1,0x65},{0xd1,0x06},{0xc1,0x27},{0x22,0xf9},{0x32,0xd8},{0x02,0xbb},{0x12,0x9a},{0x62,0x7d},{0x72,0x5c},{0x42,0x3f},{0x52,0x1e},{0xa3,0xf1},{0xb3,0xd0},{0x83,0xb3},{0x93,0x92},{0xe3,0x75},{0xf3,0x54},{0xc3,0x37},{0xd3,0x16},{0x5c,0x6e},{0x4c,0x4f},{0x7c,0x2c},{0x6c,0x0d},{0x1c,0xea},{0x0c,0xcb},{0x3c,0xa8},{0x2c,0x89},{0xdd,0x66},{0xcd,0x47},{0xfd,0x24},{0xed,0x05},{0x9d,0xe2},{0x8d,0xc3},{0xbd,0xa0},{0xad,0x81},{0x4e,0x5f},{0x5e,0x7e},{0x6e,0x1d},{0x7e,0x3c},{0x0e,0xdb},{0x1e,0xfa},{0x2e,0x99},{0x3e,0xb8},{0xcf,0x57},{0xdf,0x76},{0xef,0x15},{0xff,0x34},{0x8f,0xd3},{0x9f,0xf2},{0xaf,0x91},{0xbf,0xb0},{0x78,0x0c},{0x68,0x2d},{0x58,0x4e},{0x48,0x6f},{0x38,0x88},{0x28,0xa9},{0x18,0xca},{0x08,0xeb},{0xf9,0x04},{0xe9,0x25},{0xd9,0x46},{0xc9,0x67},{0xb9,0x80},{0xa9,0xa1},{0x99,0xc2},{0x89,0xe3},{0x6a,0x3d},{0x7a,0x1c},{0x4a,0x7f},{0x5a,0x5e},{0x2a,0xb9},{0x3a,0x98},{0x0a,0xfb},{0x1a,0xda},{0xeb,0x35},{0xfb,0x14},{0xcb,0x77},{0xdb,0x56},{0xab,0xb1},{0xbb,0x90},{0x8b,0xf3},{0x9b,0xd2},{0x85,0x22},{0x95,0x03},{0xa5,0x60},{0xb5,0x41},{0xc5,0xa6},{0xd5,0x87},{0xe5,0xe4},{0xf5,0xc5},{0x04,0x2a},{0x14,0x0b},{0x24,0x68},{0x34,0x49},{0x44,0xae},{0x54,0x8f},{0x64,0xec},{0x74,0xcd},{0x97,0x13},{0x87,0x32},{0xb7,0x51},{0xa7,0x70},{0xd7,0x97},{0xc7,0xb6},{0xf7,0xd5},{0xe7,0xf4},{0x16,0x1b},{0x06,0x3a},{0x36,0x59},{0x26,0x78},{0x56,0x9f},{0x46,0xbe},{0x76,0xdd},{0x66,0xfc},{0xa1,0x40},{0xb1,0x61},{0x81,0x02},{0x91,0x23},{0xe1,0xc4},{0xf1,0xe5},{0xc1,0x86},{0xd1,0xa7},{0x20,0x48},{0x30,0x69},{0x00,0x0a},{0x10,0x2b},{0x60,0xcc},{0x70,0xed},{0x40,0x8e},{0x50,0xaf},{0xb3,0x71},{0xa3,0x50},{0x93,0x33},{0x83,0x12},{0xf3,0xf5},{0xe3,0xd4},{0xd3,0xb7},{0xc3,0x96},{0x32,0x79},{0x22,0x58},{0x12,0x3b},{0x02,0x1a},{0x72,0xfd},{0x62,0xdc},{0x52,0xbf},{0x42,0x9e},{0xcd,0xe6},{0xdd,0xc7},{0xed,0xa4},{0xfd,0x85},{0x8d,0x62},{0x9d,0x43},{0xad,0x20},{0xbd,0x01},{0x4c,0xee},{0x5c,0xcf},{0x6c,0xac},{0x7c,0x8d},{0x0c,0x6a},{0x1c,0x4b},{0x2c,0x28},{0x3c,0x09},{0xdf,0xd7},{0xcf,0xf6},{0xff,0x95},{0xef,0xb4},{0x9f,0x53},{0x8f,0x72},{0xbf,0x11},{0xaf,0x30},{0x5e,0xdf},{0x4e,0xfe},{0x7e,0x9d},{0x6e,0xbc},{0x1e,0x5b},{0x0e,0x7a},{0x3e,0x19},{0x2e,0x38},{0xe9,0x84},{0xf9,0xa5},{0xc9,0xc6},{0xd9,0xe7},{0xa9,0x00},{0xb9,0x21},{0x89,0x42},{0x99,0x63},{0x68,0x8c},{0x78,0xad},{0x48,0xce},{0x58,0xef},{0x28,0x08},{0x38,0x29},{0x08,0x4a},{0x18,0x6b},{0xfb,0xb5},{0xeb,0x94},{0xdb,0xf7},{0xcb,0xd6},{0xbb,0x31},{0xab,0x10},{0x9b,0x73},{0x8b,0x52},{0x7a,0xbd},{0x6a,0x9c},{0x5a,0xff},{0x4a,0xde},{0x3a,0x39},{0x2a,0x18},{0x1a,0x7b},{0x0a,0x5a}};

//entrance guard code
unsigned char return_search_entrance_guard_code[] = {0xfa,0xfa,0xfa,0x2a,0x15,0x00,0x02,0x41,0x43,0x4d,0x36,0x38,0x31,0x30,0x2d,0x31,0x2d,0x30,0x31,0x31,0x37,0x30,0x37,0x30,0x38,0x32,0x39,0x6e,0xc5};
unsigned char return_setup_success_code[] = {0xfa,0xfa,0xfa,0x2a,0x01,0x00,0x81,0x6d,0xbc};
unsigned char return_handshake_code_1[] = {0xfa,0xfa,0xfa,0x2a,0x04,0x00,0x85,0x00,0x00,0x03,0x3b,0xc9};
unsigned char return_handshake_code_2[] = {0xfa,0xfa,0xfa,0x2a,0x04,0x00,0x85,0x00,0x00,0x00,0x0b,0xaa};

//报警上传数据  匹配码
unsigned char return_alarm_upload_code[] = {0xfa,0xfa,0xfa,0x2a,0x11,0x00,0x11};
unsigned char return_alarm_upload_code_1[] = {0xfa,0xfa,0xfa,0x2a,0x7d,0x00,0x11};
//门禁报警上传---开关按钮开门数据
unsigned char return_open_door_alarm_upload_code[] = {0xfa,0xfa,0xfa,0x2a,0x11,0x00,0x11,0x00,0x00,0x03,0x00,0x0c,0x10,0x00,0x00,0x00,0x00,0x0c};
//门禁报警上传---5s后自动关门数据
unsigned char return_close_door_alarm_upload_code[] = {0xfa,0xfa,0xfa,0x2a,0x11,0x00,0x11,0x00,0x00,0x03,0x00,0x0d,0x10,0x00,0x00,0x00,0x00,0x0c};
//门禁报警上传---门磁短路数据+普通消息
unsigned char return_door_contact_close_alarm_upload_code_1[] = {0xfa,0xfa,0xfa,0x2a,0x11,0x00,0x11,0x00,0x00,0x03,0x00,0x88,0x00,0x00,0x00,0x00,0x00,0x0c};
//门禁报警上传---门磁短路数据+报警消息
unsigned char return_door_contact_close_alarm_upload_code_2[] = {0xfa,0xfa,0xfa,0x2a,0x11,0x00,0x11,0x00,0x00,0x00,0x00,0x88,0x00,0x00,0x00,0x00,0x00,0x0c};
//门禁报警上传---门磁开路数据+普通消息（默认）
unsigned char return_door_contact_open_alarm_upload_code_1[] = {0xfa,0xfa,0xfa,0x2a,0x11,0x00,0x11,0x00,0x00,0x03,0x00,0x93,0x00,0x00,0x00,0x00,0x00,0x0c};
//门禁报警上传---门磁开路数据+报警消息
unsigned char return_door_contact_open_alarm_upload_code_2[] = {0xfa,0xfa,0xfa,0x2a,0x11,0x00,0x11,0x00,0x00,0x00,0x00,0x93,0x00,0x00,0x00,0x00,0x00,0x0c};

int search_entrance_guard(int *com_fd);
int entrance_guard_handshake_and_setup(int *com_fd, FILE *fp_normal_message_file, FILE *fp_alarm_message_file, FILE *fp_message_count_file, FILE *fp_config_file);
void print_string(char *string, unsigned char *buffer, unsigned int len);

void *pthread_entrance_guard(void *arg)
{
    int com_fd;
    int recv_ret = 0;

    FILE *fp_normal_message_file;
    FILE *fp_alarm_message_file;
    FILE *fp_message_count_file;
    FILE *fp_config_file;

    com_fd = InitCom(UART_DEVICE_ttyS0, ENTRANCE_GUARD_BOARD_RATE);
    //com_fd = InitCom(UART_DEVICE_ttyUSB0, BOARD_RATE);
    if (com_fd == -1) 
    {
        printf("FUNC[%s] LINE[%d]\tInit uart failed!\n",__FUNCTION__, __LINE__);
        exit(1);
    }

    //打开存储普通消息的文件
    if ((fp_normal_message_file = fopen(ENTRANCE_GUARD_NORMAL_MESSAGE_FILE, "a+")) == NULL)
    {
        printf("FUNC[%s] LINE[%d]\tOpen %s error!\n",__FUNCTION__, __LINE__, ENTRANCE_GUARD_NORMAL_MESSAGE_FILE);
        exit(1);
    }
    //打开存储报警消息的文件
    if ((fp_alarm_message_file = fopen(ENTRANCE_GUARD_ALARM_MESSAGE_FILE, "a+")) == NULL)
    {
        printf("FUNC[%s] LINE[%d]\tOpen %s error!\n",__FUNCTION__, __LINE__, ENTRANCE_GUARD_ALARM_MESSAGE_FILE);
        exit(1);
    }

    //打开存储普通消息和报警消息计数的文件
    if ((fp_message_count_file = fopen(ENTRANCE_GUARD_MESSAGE_COUNT_FILE, "a+")) == NULL)
    {
        printf("FUNC[%s] LINE[%d]\tOpen %s error!\n",__FUNCTION__, __LINE__, ENTRANCE_GUARD_ALARM_MESSAGE_FILE);
        exit(1);
    }
    fseek(fp_message_count_file, 0, SEEK_SET);
    //if ((fscanf(fp_message_count_file, "%03d %03d\n",&entrance_guard_data.current_normal_message_num, &entrance_guard_data.current_alarm_message_num)) != 2)
    if ((fscanf(fp_message_count_file, "%03d ",&entrance_guard_data.current_normal_message_num)) != 1)
    {
        entrance_guard_data.current_normal_message_num = 0;
    }
    if ((fscanf(fp_message_count_file, "%03d\n",&entrance_guard_data.current_alarm_message_num)) != 1)
    {
        entrance_guard_data.current_alarm_message_num = 0;
    }
    fclose(fp_message_count_file);
    if ((fp_message_count_file = fopen(ENTRANCE_GUARD_MESSAGE_COUNT_FILE, "w+")) == NULL)
    {
        printf("FUNC[%s] LINE[%d]\tOpen %s error!\n",__FUNCTION__, __LINE__, ENTRANCE_GUARD_ALARM_MESSAGE_FILE);
        exit(1);
    }
    fseek(fp_message_count_file, 0, SEEK_SET);
    fprintf(fp_message_count_file, "%03d %03d\n",entrance_guard_data.current_normal_message_num, entrance_guard_data.current_alarm_message_num);
    fflush(fp_message_count_file);

    //打开存储门禁控制器一些参数的文件
    //一共存储5个参数，从上到下顺序为：
    //door_lock_relay_status_setup (1:常开, 0:常闭)   
    //door_contact_detection_mode_setup (1:开路，0:短路)
    //door_status (1:开门，0:关门)
    //client_set_door_hold_time (0-255s)
    //button_set_door_hold_time (0-255s)
    if ((fp_config_file = fopen(ENTRANCE_GUARD_CONFIG_FILE, "a+")) == NULL)
    {
        printf("FUNC[%s] LINE[%d]\tOpen %s error!\n",__FUNCTION__, __LINE__, ENTRANCE_GUARD_CONFIG_FILE);
        exit(1);
    }
    fseek(fp_config_file, 0, SEEK_SET);
    if ((fscanf(fp_config_file, "%d ",&entrance_guard_data.door_lock_relay_status_setup)) != 1)
    {
        entrance_guard_data.door_lock_relay_status_setup = IS_OPENED;
    }
    if ((fscanf(fp_config_file, "%d ",&entrance_guard_data.door_contact_detection_mode_setup)) != 1)
    {
        entrance_guard_data.door_contact_detection_mode_setup = IS_CLOSED;
    }
    if ((fscanf(fp_config_file, "%d ",&entrance_guard_data.door_status)) != 1)
    {
        entrance_guard_data.door_status = IS_OPENED;
    }
    if ((fscanf(fp_config_file, "%03d ",&entrance_guard_data.client_set_door_hold_time)) != 1)
    {
        entrance_guard_data.client_set_door_hold_time = 5;
    }
    if ((fscanf(fp_config_file, "%03d ",&entrance_guard_data.button_set_door_hold_time)) != 1)
    {
        entrance_guard_data.button_set_door_hold_time = 5;
    }
    fclose(fp_config_file);
    if ((fp_config_file = fopen(ENTRANCE_GUARD_CONFIG_FILE, "w+")) == NULL)
    {
        printf("FUNC[%s] LINE[%d]\tOpen %s error!\n",__FUNCTION__, __LINE__, ENTRANCE_GUARD_CONFIG_FILE);
        exit(1);
    }
    printf_debug("\n\nEntrance guard initial status info: %d %d %d %03d %03d\n\n",entrance_guard_data.door_lock_relay_status_setup, entrance_guard_data.door_contact_detection_mode_setup, entrance_guard_data.door_status, entrance_guard_data.client_set_door_hold_time, entrance_guard_data.button_set_door_hold_time);
    fprintf(fp_config_file, "%d %d %d %03d %03d",entrance_guard_data.door_lock_relay_status_setup, entrance_guard_data.door_contact_detection_mode_setup, entrance_guard_data.door_status, entrance_guard_data.client_set_door_hold_time, entrance_guard_data.button_set_door_hold_time);
    fflush(fp_config_file);

SEARCH_ENTRANCE_GUARD:
#if 0
    // Send search code //FA FA FA 2A 01 00 02 CC 57
    usleep(100000); //100ms
    if (search_entrance_guard(&com_fd) != 1)
    {
        exit(1);
    }
    usleep(300000); //300ms
#endif

    recv_ret = entrance_guard_handshake_and_setup(&com_fd, fp_normal_message_file, fp_alarm_message_file, fp_message_count_file, fp_config_file);
    if (recv_ret == -1) 
    {
        sleep(30);
        goto SEARCH_ENTRANCE_GUARD;
        //exit(1);
    }
    else if (recv_ret == 0) 
    {
        goto SEARCH_ENTRANCE_GUARD;
    }

    return (void *)1;
}

int entrance_guard_handshake_and_setup(int *com_fd, FILE *fp_normal_message_file, FILE *fp_alarm_message_file, FILE *fp_message_count_file, FILE *fp_config_file)
{
    time_t tm;
    struct tm *t;
    int i = 0;
    int recv_ret = 0;
    unsigned int recv_size = 0;
    unsigned char recv_buffer[512] = {0};
    unsigned char *send_buffer = NULL;
    unsigned char send_size = 0;

    //entrance_guard_data.current_handshake_num = 0xbe;

    while (1) 
    {
        usleep(1000000);
        memset(recv_buffer, 0, HANDSHAKE_SETUP_RECV_SIZE);

        //printf_debug("entrance_guard_data.setup_command_set = %d\n",entrance_guard_data.setup_command_set);
        switch(entrance_guard_data.setup_command_set)
        {
            case ENTRANCE_GUARD_NO_VALID_COMMAND:  //没有合法的命令，则发送握手码   
                handshake_base_code[8] = entrance_guard_data.current_handshake_num;
                handshake_base_code[9] = handshake_variable_code[handshake_base_code[8]][0];
                handshake_base_code[10] = handshake_variable_code[handshake_base_code[8]][1];
                send_buffer = handshake_base_code;
                send_size = HANDSHAKE_SEND_SIZE;
#ifdef DEBUG 
                //printf_debug("handshake_base_code: %x %x %x\n",handshake_base_code[8], handshake_base_code[9], handshake_base_code[10]);
                //print_string("handshake_base_code: ", handshake_base_code, 11);
#endif
                break;
            case ENTRANCE_GUARD_OPEN_DOOR:         //门状态设置(开门)
                send_buffer = open_door_code;
                send_size = OPEN_DOOR_SEND_SIZE;
                break;
            case ENTRANCE_GUARD_CLOSE_DOOR:        //门状态设置(关门) 
                send_buffer = close_door_code;
                send_size = CLOSE_DOOR_SEND_SIZE;
                break;
            case ENTRANCE_GUARD_RELAY_OPEN_DOOR:   //门锁继电器状态设置(常开)(默认)
                send_buffer = door_lock_default_open_setup_code;
                send_size = DOOR_LOCK_DEFAULT_OPEN_SETUP_SEND_SIZE;
                break;
            case ENTRANCE_GUARD_RELAY_CLOSE_DOOR:  //门锁继电器状态设置(常闭)
                send_buffer = door_lock_default_close_setup_code;
                send_size = DOOR_LOCK_DEFAULT_CLOSE_SETUP_SEND_SIZE;
                break;
            case ENTRANCE_GUARD_CLIENT_SET_DOOR_HOLD_TIME: //客户端设置门的保持时间(1-255秒)
                for (i = 0; i < CLIENT_CONTROL_DOOR_HOLD_TIME_COUNT; i++) 
                {
                    if (client_control_door_hold_time_code[i][12] == entrance_guard_data.client_set_door_hold_time) 
                    {
                        send_buffer = client_control_door_hold_time_code[i];
                        send_size = CLIENT_CONTROL_DOOR_HOLD_TIME_SEND_SIZE; 
                        entrance_guard_data.if_set_door_hold_time_is_valid = YES;
                        break;
                    }
                }
                if (entrance_guard_data.if_set_door_hold_time_is_valid == NO) 
                {
                    send_buffer = handshake_base_code;
                    send_size = HANDSHAKE_SEND_SIZE;
                    entrance_guard_data.setup_command_set = ENTRANCE_GUARD_NO_VALID_COMMAND;
                    printf("FUNC[%s] LINE[%d]\tSet door hold time is not valid!\n",__FUNCTION__, __LINE__);
                    //navy 网络发送设置门保持时间不合法反馈包
                }
                entrance_guard_data.if_set_door_hold_time_is_valid = NO;
                break;
            case ENTRANCE_GUARD_BUTTON_SET_DOOR_HOLD_TIME: //开门按钮设置门的保持时间(1-255秒)
                for (i = 0; i < BUTTON_CONTROL_DOOR_HOLD_TIME_COUNT; i++) 
                {
                    if (door_button_control_door_hold_time_code[i][10] == entrance_guard_data.button_set_door_hold_time) 
                    {
                        send_buffer = door_button_control_door_hold_time_code[i];
                        send_size = BUTTON_CONTROL_DOOR_HOLD_TIME_SEND_SIZE; 
                        entrance_guard_data.if_set_door_hold_time_is_valid = YES;
                        break;
                    }
                }
                if (entrance_guard_data.if_set_door_hold_time_is_valid == NO) 
                {
                    send_buffer = handshake_base_code;
                    send_size = HANDSHAKE_SEND_SIZE;
                    entrance_guard_data.setup_command_set = ENTRANCE_GUARD_NO_VALID_COMMAND;
                    printf("FUNC[%s] LINE[%d]\tSet door hold time is not valid!\n",__FUNCTION__, __LINE__);
                    //navy 网络发送设置门保持时间不合法反馈包
                }
                entrance_guard_data.if_set_door_hold_time_is_valid = NO;
                break;
            //case ENTRANCE_GUARD_GET_NORMAL_MESSAGE: //获取普通消息
                //break;
            //case ENTRANCE_GUARD_GET_ALARM_MESSAGE:  //获取报警消息
                //break;
            case ENTRANCE_GUARD_DOOR_CONTACT_NORMALLY_OPEN: //门磁状态设置(常开)
                send_buffer = door_contact_default_open_setup_code;
                send_size = DOOR_CONTACT_DEFAULT_OPEN_SETUP_SEND_SIZE;
                break;
            case ENTRANCE_GUARD_DOOR_CONTACT_NORMALLY_CLOSE: //门磁状态设置(常闭)(默认)
                send_buffer = door_contact_default_close_setup_code;
                send_size = DOOR_CONTACT_DEFAULT_CLOSE_SETUP_SEND_SIZE;
                break;
            default:
                break;
        }
        //print_string("send_data: ", send_buffer, send_size);

        //pthread_mutex_lock(&entrance_guard_mutex);
        if (SendDataToCom(*com_fd, send_buffer, send_size) == -1)
        {
            printf("FUNC[%s] LINE[%d]\tSend data to com error!\n",__FUNCTION__, __LINE__);
            return -1;
        }

        recv_size = HANDSHAKE_SETUP_RECV_SIZE;
        recv_ret = RecvDataFromCom(*com_fd, recv_buffer, &recv_size, RECV_TIMEOUT, MATCH_HANDSHAKE_TIMEOUT);
        switch (recv_ret)
        {
            case 0:
            #if 0
#ifdef DEBUG 
                printf_debug("FUNC[%s] LINE[%d]\trecv_size = %d\n",__FUNCTION__, __LINE__,recv_size);
                print_string("recv_data: ", recv_buffer, recv_size);
#endif
                usleep(500000);
                break;
            #endif
            case -1:
                if (recv_size == 0) 
                {
                    if (entrance_guard_data.setup_command_set) 
                    {
                        printf("FUNC[%s] LINE[%d]\tTimeout, setup_command_set = 0x%x!\n",__FUNCTION__, __LINE__, entrance_guard_data.setup_command_set);
                    }
                    else
                    {
                        printf("FUNC[%s] LINE[%d]\tTimeout, ditn't receive entrance guard handshake code!\n",__FUNCTION__, __LINE__);
                    }
                    entrance_guard_data.setup_command_set = ENTRANCE_GUARD_NO_VALID_COMMAND;
                    entrance_guard_data.if_entrance_guard_alive = NO;
                    entrance_guard_data.if_has_delete_offline_alarm = NO;
                #if 1
                    usleep(100000);
                #else
                    recv_size = HANDSHAKE_SETUP_RECV_SIZE;
                    recv_ret = RecvDataFromCom(*com_fd, recv_buffer, &recv_size, 500, 300000);
                #endif
                    break;
                }
                else
                {
#ifdef DEBUG 
                    //printf_debug("FUNC[%s] LINE[%d]\trecv_size = %d\n",__FUNCTION__, __LINE__,recv_size);
                    //print_string("recv_data: \t", recv_buffer, recv_size);
#endif
                    if ((strncmp(return_handshake_code_1, recv_buffer, 12) == 0) || (strncmp(return_handshake_code_2, recv_buffer, 12) == 0)) 
                    {
                        printf_debug("FUNC[%s] LINE[%d]\tHandshake code: %x %x %x\n",__FUNCTION__, __LINE__, handshake_base_code[8], handshake_base_code[9], handshake_base_code[10]);
                        entrance_guard_data.if_has_delete_offline_alarm = YES;
                    }
                    else if (strncmp(return_setup_success_code, recv_buffer, 9) == 0) 
                    {
                        switch(entrance_guard_data.setup_command_set)
                        {
                        //navy 
                            case ENTRANCE_GUARD_NO_VALID_COMMAND:  //没有合法的命令，则发送握手码   
                            //navy 网络发送消息“无效命令”
                                break;
                            case ENTRANCE_GUARD_OPEN_DOOR:         //门状态设置(开门)
                                if (entrance_guard_data.door_status == IS_OPENED) 
                                {
                                    printf_debug("FUNC[%s] LINE[%d]\tRepeat open the door!\n",__FUNCTION__, __LINE__);
                                    //navy 网络发送消息“重复打开门”
                                    
                                }
                                else
                                {
                                    printf_debug("FUNC[%s] LINE[%d]\tThe door is opened!\n",__FUNCTION__, __LINE__);
                                    entrance_guard_data.door_status = IS_OPENED;

                                    //navy 网络发送消息“成功打开门”
                                }
                                break;
                            case ENTRANCE_GUARD_CLOSE_DOOR:        //门状态设置(关门) 
                                if (entrance_guard_data.door_status == IS_CLOSED) 
                                {
                                    printf_debug("FUNC[%s] LINE[%d]\tRepeat close the door!\n",__FUNCTION__, __LINE__);
                                    //navy 网络发送消息“重复关闭门”
                                    
                                }
                                else
                                {
                                    printf_debug("FUNC[%s] LINE[%d]\tThe door is closed!\n",__FUNCTION__, __LINE__);
                                    entrance_guard_data.door_status = IS_CLOSED;
                                    //navy 网络发送消息“成功关闭门”
                                }
                                break;
                            case ENTRANCE_GUARD_RELAY_OPEN_DOOR:   //门锁继电器状态设置(常开)(默认)
                                    entrance_guard_data.door_lock_relay_status_setup = IS_OPENED;
                                printf_debug("FUNC[%s] LINE[%d]\tThe door lock relay is setup to normally open!\n",__FUNCTION__, __LINE__);
                                break;
                            case ENTRANCE_GUARD_RELAY_CLOSE_DOOR:  //门锁继电器状态设置(常闭)
                                    entrance_guard_data.door_lock_relay_status_setup = IS_CLOSED;
                                printf_debug("FUNC[%s] LINE[%d]\tThe door lock relay is setup to normally close!\n",__FUNCTION__, __LINE__);
                                break;
                            case ENTRANCE_GUARD_CLIENT_SET_DOOR_HOLD_TIME: //客户端设置门的保持时间(1-255秒)
                                printf_debug("FUNC[%s] LINE[%d]\tClient set door hold time is %d seconds!\n",__FUNCTION__, __LINE__, entrance_guard_data.client_set_door_hold_time);
                                break;
                            case ENTRANCE_GUARD_BUTTON_SET_DOOR_HOLD_TIME: //开门按钮设置门的保持时间(1-255秒)
                                printf_debug("FUNC[%s] LINE[%d]\tButton set door hold time is %d seconds!\n",__FUNCTION__, __LINE__, entrance_guard_data.button_set_door_hold_time);
                                break;
                            //case ENTRANCE_GUARD_GET_NORMAL_MESSAGE: //获取普通消息
                                //break;
                            //case ENTRANCE_GUARD_GET_ALARM_MESSAGE:  //获取报警消息
                                //break;
                            case ENTRANCE_GUARD_DOOR_CONTACT_NORMALLY_OPEN: //门磁状态设置(常开)
                                entrance_guard_data.door_contact_detection_mode_setup = IS_OPENED;
                                printf_debug("FUNC[%s] LINE[%d]\tThe door contact is setup to normally open!\n",__FUNCTION__, __LINE__);
                                break;
                            case ENTRANCE_GUARD_DOOR_CONTACT_NORMALLY_CLOSE: //门磁状态设置(常闭)(默认)
                                entrance_guard_data.door_contact_detection_mode_setup = IS_CLOSED;
                                printf_debug("FUNC[%s] LINE[%d]\tThe door contact is setup to normally close!\n",__FUNCTION__, __LINE__);
                                break;
                            default:
                                break;
                        }
                        entrance_guard_data.setup_command_set = ENTRANCE_GUARD_NO_VALID_COMMAND;
                        fseek(fp_config_file, 0, SEEK_SET);
                        printf_debug("\n\n\nentrance guard status info: %d %d %d %03d %03d\n\n\n",entrance_guard_data.door_lock_relay_status_setup, entrance_guard_data.door_contact_detection_mode_setup, entrance_guard_data.door_status, entrance_guard_data.client_set_door_hold_time, entrance_guard_data.button_set_door_hold_time);
                        fprintf(fp_config_file, "%d %d %d %03d %03d",entrance_guard_data.door_lock_relay_status_setup, entrance_guard_data.door_contact_detection_mode_setup, entrance_guard_data.door_status, entrance_guard_data.client_set_door_hold_time, entrance_guard_data.button_set_door_hold_time);
                        fflush(fp_config_file);
                    }
                    //else if(strncmp(return_alarm_upload_code, recv_buffer, 7) == 0)
                    else if(strncmp(return_alarm_upload_code, recv_buffer, 4) == 0)
                    {
                        //从接受到的数据中获取握手码基值
                        entrance_guard_data.current_handshake_num = recv_buffer[10];
#ifdef DEBUG 
                        printf_debug("FUNC[%s] LINE[%d]\trecv_size = %d\n",__FUNCTION__, __LINE__,recv_size);
                        print_string("recv_data: ", recv_buffer, recv_size);
#endif
                        if (entrance_guard_data.if_has_delete_offline_alarm == YES) 
                        {
                            tm = time(NULL);
                            t = localtime(&tm);

                            if (entrance_guard_data.current_normal_message_num >= MAX_NORMAL_MESSAGE_NUM) 
                            {
                                entrance_guard_data.current_normal_message_num = 0;
                                fclose(fp_normal_message_file);
                                if (remove(ENTRANCE_GUARD_NORMAL_MESSAGE_FILE) < 0)
                                {
                                    printf("FUNC[%s] LINE[%d]\tFailed to delete %s!\n",__FUNCTION__, __LINE__, ENTRANCE_GUARD_NORMAL_MESSAGE_FILE);
                                }
                                else
                                {
                                    printf_debug("FUNC[%s] LINE[%d]\tSucceed to delete %s!\n",__FUNCTION__, __LINE__, ENTRANCE_GUARD_NORMAL_MESSAGE_FILE);
                                }

                                if ((fp_normal_message_file = fopen(ENTRANCE_GUARD_NORMAL_MESSAGE_FILE, "w+")) == NULL)
                                {
                                    printf("FUNC[%s] LINE[%d]\tOpen %s error!\n",__FUNCTION__, __LINE__, ENTRANCE_GUARD_NORMAL_MESSAGE_FILE);
                                    //exit(1);
                                    return -1;
                                }
                            }
                            if (entrance_guard_data.current_alarm_message_num >= MAX_ALARM_MESSAGE_NUM) 
                            {
                                entrance_guard_data.current_alarm_message_num = 0;
                                fclose(fp_alarm_message_file);
                                if (remove(ENTRANCE_GUARD_ALARM_MESSAGE_FILE) < 0)
                                {
                                    printf("FUNC[%s] LINE[%d]\tFailed to delete %s!\n",__FUNCTION__, __LINE__, ENTRANCE_GUARD_ALARM_MESSAGE_FILE);
                                }
                                else
                                {
                                    printf_debug("FUNC[%s] LINE[%d]\tSucceed to delete %s!\n",__FUNCTION__, __LINE__, ENTRANCE_GUARD_ALARM_MESSAGE_FILE);
                                }
                                if ((fp_alarm_message_file = fopen(ENTRANCE_GUARD_ALARM_MESSAGE_FILE, "w+")) == NULL)
                                {
                                    printf("FUNC[%s] LINE[%d]\tOpen %s error!\n",__FUNCTION__, __LINE__, ENTRANCE_GUARD_ALARM_MESSAGE_FILE);
                                    //exit(1);
                                    return -1;
                                }
                            }

                            fseek(fp_message_count_file, 0, SEEK_SET);
                            switch(recv_buffer[11])
                            {
                                //存储格式 "2012-07-13 16:10:30 1" (1表示门锁打开，2表示门锁关闭，3表示合法打开门磁，4表示关闭门磁，5表示非法打开门磁)
                                //case 0x0c:  //按下开门按钮，门锁打开
                                case VALID_OPEN_DOOR_LOCK:
                                    fprintf(fp_normal_message_file, "%04d-%02d-%02d %02d:%02d:%02d %02d\n",t->tm_year + 1900, t->tm_mon + 1, t->tm_mday, t->tm_hour, t->tm_min, t->tm_sec, 1);
                                    fprintf(fp_message_count_file, "%03d %03d\n",++entrance_guard_data.current_normal_message_num, entrance_guard_data.current_alarm_message_num);
                                    printf_debug("FUNC[%s] LINE[%d]\t%04d-%02d-%02d %02d:%02d:%02d Door lock open message\n",__FUNCTION__, __LINE__,t->tm_year + 1900, t->tm_mon + 1, t->tm_mday, t->tm_hour, t->tm_min, t->tm_sec);
                                    break;
                                //case 0x0d:  //门开保持时间(5秒)后，门自动关闭
                                case VALID_CLOSE_DOOR_LOCK:
                                    fprintf(fp_normal_message_file, "%04d-%02d-%02d %02d:%02d:%02d %02d\n",t->tm_year + 1900, t->tm_mon + 1, t->tm_mday, t->tm_hour, t->tm_min, t->tm_sec, 2);
                                    fprintf(fp_message_count_file, "%03d %03d\n",++entrance_guard_data.current_normal_message_num, entrance_guard_data.current_alarm_message_num);
                                    printf_debug("FUNC[%s] LINE[%d]\t%04d-%02d-%02d %02d:%02d:%02d Door lock close message\n",__FUNCTION__, __LINE__,t->tm_year + 1900, t->tm_mon + 1, t->tm_mday, t->tm_hour, t->tm_min, t->tm_sec);
                                    break;
                                //case 0x12:  //合法打开门磁(合法开门)
                                //case 0x10:  //合法打开门磁(合法开门)
                                case VALID_OPEN_DOOR_CONTACT_1:
                                case VALID_OPEN_DOOR_CONTACT_2:
                                    fprintf(fp_normal_message_file, "%04d-%02d-%02d %02d:%02d:%02d %02d\n",t->tm_year + 1900, t->tm_mon + 1, t->tm_mday, t->tm_hour, t->tm_min, t->tm_sec, 3);
                                    fprintf(fp_message_count_file, "%03d %03d\n",++entrance_guard_data.current_normal_message_num, entrance_guard_data.current_alarm_message_num);
                                    printf_debug("FUNC[%s] LINE[%d]\t%04d-%02d-%02d %02d:%02d:%02d Door contact normally open message\n",__FUNCTION__, __LINE__,t->tm_year + 1900, t->tm_mon + 1, t->tm_mday, t->tm_hour, t->tm_min, t->tm_sec);
                                    break;
                                //case 0x88:  //关闭门磁(关门)
                                case VALID_CLOSE_DOOR_CONTACT:
                                    fprintf(fp_normal_message_file, "%04d-%02d-%02d %02d:%02d:%02d %02d\n",t->tm_year + 1900, t->tm_mon + 1, t->tm_mday, t->tm_hour, t->tm_min, t->tm_sec, 4);
                                    fprintf(fp_message_count_file, "%03d %03d\n",++entrance_guard_data.current_normal_message_num, entrance_guard_data.current_alarm_message_num);
                                    printf_debug("FUNC[%s] LINE[%d]\t%04d-%02d-%02d %02d:%02d:%02d Door contact normally close message\n",__FUNCTION__, __LINE__,t->tm_year + 1900, t->tm_mon + 1, t->tm_mday, t->tm_hour, t->tm_min, t->tm_sec);
                                    break;
                                //case 0x93:  //非法打开门磁(非法开门)
                                //case 0x91:  //非法打开门磁(非法开门)
                                case INVALID_OPEN_DOOR_CONTACT_1:
                                case INVALID_OPEN_DOOR_CONTACT_2:
                                    fprintf(fp_alarm_message_file, "%04d-%02d-%02d %02d:%02d:%02d %02d\n",t->tm_year + 1900, t->tm_mon + 1, t->tm_mday, t->tm_hour, t->tm_min, t->tm_sec, 5);
                                    fflush(fp_alarm_message_file);
                                    fprintf(fp_message_count_file, "%03d %03d\n",entrance_guard_data.current_normal_message_num, ++entrance_guard_data.current_alarm_message_num);
                                    printf_debug("FUNC[%s] LINE[%d]\t%04d-%02d-%02d %02d:%02d:%02d Warning! Door contact abnormally open message upload\n",__FUNCTION__, __LINE__,t->tm_year + 1900, t->tm_mon + 1, t->tm_mday, t->tm_hour, t->tm_min, t->tm_sec);
                                    //网络发送给客户端 //navy
                                    break;
                                default:
                                    break;
                            }
                            fflush(fp_normal_message_file);
                            fflush(fp_message_count_file);

                            fseek(fp_message_count_file, 0, SEEK_SET);
                            if ((fscanf(fp_message_count_file, "%03d %03d\n",&entrance_guard_data.current_normal_message_num, &entrance_guard_data.current_alarm_message_num)) != 2)
                            {
                                entrance_guard_data.current_normal_message_num = 0;
                                entrance_guard_data.current_alarm_message_num = 0;
                            }
                            printf_debug("current_normal_message_num = %d \ncurrent_alarm_message_num = %d\n",entrance_guard_data.current_normal_message_num, entrance_guard_data.current_alarm_message_num);
                        }
                    }
                    entrance_guard_data.if_entrance_guard_alive = YES;
                }
                break;
            case -2:
                printf("FUNC[%s] LINE[%d]\tRecv data error!\n",__FUNCTION__, __LINE__);
                //navy send uart recv data error info to client
                //exit(1);
                //pthread_mutex_unlock(&entrance_guard_mutex);
                return -1;
            default: 
                break;
        }
        //pthread_mutex_unlock(&entrance_guard_mutex);
    }

    return 0;
}

void print_string(char *string, unsigned char *buffer, unsigned int len)
{
    int i = 0;

    printf("%s",string);
    for (i = 0; i < len; i++) 
    {
        printf("%02X ",buffer[i]);
    }
    printf("\n");
}

int send_entrance_guard_default_setup(int *com_fd)
{
    int recv_ret = 0;
    unsigned int recv_size = 0;
    unsigned char recv_buffer[256] = {0};
    usleep(150000);
    //if (SendDataToCom(*com_fd, door_contact_default_open_setup_code, 19) == -1)
    if (SendDataToCom(*com_fd, door_contact_default_close_setup_code, 19) == -1)
    {
        printf("FUNC[%s] LINE[%d]\tSend data to com error!\n",__FUNCTION__, __LINE__);
        return -1;
    }
    usleep(150000);
    if (SendDataToCom(*com_fd, door_button_default_setup_code_1, 19) == -1)
    {
        printf("FUNC[%s] LINE[%d]\tSend data to com error!\n",__FUNCTION__, __LINE__);
        return -1;
    }
    usleep(150000);
    if (SendDataToCom(*com_fd, door_lock_default_open_setup_code, 19) == -1)
    {
        printf("FUNC[%s] LINE[%d]\tSend data to com error!\n",__FUNCTION__, __LINE__);
        return -1;
    }
    recv_size = HANDSHAKE_SETUP_RECV_SIZE;
    if ((recv_ret = RecvDataFromCom(*com_fd, recv_buffer, &recv_size, RECV_TIMEOUT, ENTRANCE_GUARD_RECV_OVER_TIME)) == -2)
    {
        return -1;    
    }
    return 1;
}

int search_entrance_guard(int *com_fd)
{
    int recv_ret = 0;
    unsigned int recv_size = 0;
    unsigned char recv_buffer[64] = {0};

    while (1) 
    {
        if (SendDataToCom(*com_fd, search_entrance_guard_code, 9) == -1)
        {
            printf("FUNC[%s] LINE[%d]\tSend data to com error!\n",__FUNCTION__, __LINE__);
            return -1;
        }
        recv_size = SEARCH_ENTRANCE_GUARD_RECV_SIZE;
        recv_ret = RecvDataFromCom(*com_fd, recv_buffer, &recv_size, RECV_TIMEOUT, ENTRANCE_GUARD_RECV_OVER_TIME);
        switch (recv_ret)
        {
            case 0:
                if (strcmp(return_search_entrance_guard_code, recv_buffer) == 0) 
                {
                    printf("FUNC[%s] LINE[%d]\tEntrance guard is alive !\n",__FUNCTION__, __LINE__);
                    entrance_guard_data.if_entrance_guard_alive = YES;
                #if 1
                    return 1;
                #else
                    return send_entrance_guard_default_setup(com_fd); 
                #endif
                }
                else
                {
                    printf_debug("FUNC[%s] LINE[%d]\tReceived entrance guard data is not right!\n",__FUNCTION__, __LINE__);
                }
                break;
            case -1:
                if (recv_size == 0) 
                {
                    printf_debug("FUNC[%s] LINE[%d]\tTimeout, entrance guard is not alive !\n",__FUNCTION__, __LINE__);
                    entrance_guard_data.if_entrance_guard_alive = NO;
                }
                else
                {
                    printf_debug("FUNC[%s] LINE[%d]\tDidn't recv enough data ! recv_size = %d\n",__FUNCTION__, __LINE__, recv_size);
                }
                break;
            case -2:
                printf("FUNC[%s] LINE[%d]\tRecv data error!\n",__FUNCTION__, __LINE__);
                return -1;
            default: 
                break;
        }
        memset(recv_buffer, 0, SEARCH_ENTRANCE_GUARD_RECV_SIZE);
        sleep(3);
    }

    return -1;
}

