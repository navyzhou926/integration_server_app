#ifndef _NET_H
#define _NET_H

#include <sys/socket.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/select.h>
#include <time.h>

#define SERVER_PORT_TCP	    21203	

#define SERVER_ACCEPT_TIMEOUT_SECOND        120
#define SERVER_ACCEPT_TIMEOUT_MICROSECOND   0

#define SERVER_RECV_TIMEOUT_SECOND          6000
#define SERVER_RECV_TIMEOUT_MICROSECOND     0

#define NET_HANDSHAKE_DELAY_TIME            10

#define NET_RECV_BUFFER_SIZE           	    32

extern char ip[36];
extern char mac[36];
extern char net_recv_buffer[NET_RECV_BUFFER_SIZE + 1];
extern int if_disconnect_client;
extern pthread_mutex_t counter_mutex;

void *pthread_handshake(void *arg);
int get_local_ip(char *out_ip, char *out_mac);
int waiting_for_client_connect(int *server_sock_tcp, struct sockaddr_in *server_tcp, int *client_sock_tcp, struct sockaddr_in *client_tcp); 
int tcp_server_recv(int *server_sock_tcp, int *client_sock_tcp);


#endif
