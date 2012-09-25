#ifndef _GLOABL_H
#define _GLOABL_H

#define YES 1
#define NO  0

#define MATRIX_UART_DEVICE              UART_DEVICE_ttyS2
#define ENTRANCE_GUARD_UART_DEVICE      UART_DEVICE_ttyS1
#define CK2316_UART_DEVICE              UART_DEVICE_ttyS3
#define CRADLE_HEAD_UART_DEVICE         UART_DEVICE_ttyS6

#define UART_DEV_CONFIG_FILE            "dev_config_file.config"

#define MAX_UART_DEV_NUM                7

void init_serial_dev_parameter(void);
int set_serial_dev_parameter(FILE *fp_dev_config_file);

#endif
