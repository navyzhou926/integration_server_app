#ifndef _CK2316_ALARM_H
#define _CK2316_ALARM_H_H

#define CK2316_MAX_RECV_SIZE                32
#define CK2316_HANDSHAKE_RECV_SIZE          2

#define CK2316_RECV_TIMEOUT                 3000
#define CK2316_RECV_FINISH_TIMEOUT          15000

void *pthread_ck2316_alarm(void *arg);

#endif
