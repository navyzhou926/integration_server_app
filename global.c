#include <stdio.h>
#include "global.h"
#include "entrance_guard.h"
#include "ck2316_alarm.h"
#include "matrix_control.h"
#include "cradle_head_control.h"

#define DEBUG
#ifdef DEBUG
#define printf_debug(fmt, arg...) printf(fmt, ##arg)
#else
#define printf_debug(fmt, arg...) do{}while(0)
#endif

void init_entrance_guard_serial_pamater(FILE *fp_dev_config_file)
{
    int temp = 0;
    #if 0
    if ((fscanf(fp_dev_config_file, "%d ",&entrance_guard_data.entrance_guard_serial_pamater.byPathNo)) != 1)
        entrance_guard_data.entrance_guard_serial_pamater.byPathNo = ENTRANCE_GUARD_SERIAL_PORT;
    if ((fscanf(fp_dev_config_file, "%d ",&entrance_guard_data.entrance_guard_serial_pamater.serialAttr.dwBaudRate)) != 1)
        entrance_guard_data.entrance_guard_serial_pamater.serialAttr.dwBaudRate = ENTRANCE_GUARD_BOARD;
    if ((fscanf(fp_dev_config_file, "%d ",&entrance_guard_data.entrance_guard_serial_pamater.serialAttr.byDataBit)) != 1)
        entrance_guard_data.entrance_guard_serial_pamater.serialAttr.byDataBit = ENTRANCE_GUARD_UART_DATA_BIT;
    if ((fscanf(fp_dev_config_file, "%d ",&entrance_guard_data.entrance_guard_serial_pamater.serialAttr.byStopBit)) != 1)
        entrance_guard_data.entrance_guard_serial_pamater.serialAttr.byStopBit = ENTRANCE_GUARD_UART_STOP_BIT;
    if ((fscanf(fp_dev_config_file, "%d ",&entrance_guard_data.entrance_guard_serial_pamater.serialAttr.byParity)) != 1)
        entrance_guard_data.entrance_guard_serial_pamater.serialAttr.byParity = ENTRANCE_GUARD_UART_CHECK_BIT;
    if ((fscanf(fp_dev_config_file, "%d\n",&entrance_guard_data.entrance_guard_serial_pamater.serialAttr.byFlowcontrol)) != 1)
        entrance_guard_data.entrance_guard_serial_pamater.serialAttr.byFlowcontrol = 0;
    #endif
    if ((fscanf(fp_dev_config_file, "%d ",&temp)) == 1)
        entrance_guard_data.entrance_guard_serial_pamater.byPathNo = temp;
    if ((fscanf(fp_dev_config_file, "%d ",&temp)) == 1)
        entrance_guard_data.entrance_guard_serial_pamater.serialAttr.dwBaudRate = temp;
    if ((fscanf(fp_dev_config_file, "%d ",&temp)) == 1)
        entrance_guard_data.entrance_guard_serial_pamater.serialAttr.byDataBit = temp;
    if ((fscanf(fp_dev_config_file, "%d ",&temp)) == 1)
        entrance_guard_data.entrance_guard_serial_pamater.serialAttr.byStopBit = temp;
    if ((fscanf(fp_dev_config_file, "%d ",&temp)) == 1)
        entrance_guard_data.entrance_guard_serial_pamater.serialAttr.byParity = temp;
    if ((fscanf(fp_dev_config_file, "%d\n",&temp)) == 1)
        entrance_guard_data.entrance_guard_serial_pamater.serialAttr.byFlowcontrol = temp;
}

void init_matrix_control_serial_pamater(FILE *fp_dev_config_file)
{
    int temp = 0;
    #if 0
    if ((fscanf(fp_dev_config_file, "%d ",&matrix_control_data.matrix_control_serial_pamater.byPathNo)) != 1)
        matrix_control_data.matrix_control_serial_pamater.byPathNo = MATRIX_SERIAL_PORT;
    if ((fscanf(fp_dev_config_file, "%d ",&matrix_control_data.matrix_control_serial_pamater.serialAttr.dwBaudRate)) != 1)
        matrix_control_data.matrix_control_serial_pamater.serialAttr.dwBaudRate = MATRIX_UART_BOARD;
    if ((fscanf(fp_dev_config_file, "%d ",&matrix_control_data.matrix_control_serial_pamater.serialAttr.byDataBit)) != 1)
        matrix_control_data.matrix_control_serial_pamater.serialAttr.byDataBit = MATRIX_UART_DATA_BIT;
    if ((fscanf(fp_dev_config_file, "%d ",&matrix_control_data.matrix_control_serial_pamater.serialAttr.byStopBit)) != 1)
        matrix_control_data.matrix_control_serial_pamater.serialAttr.byStopBit = MATRIX_UART_STOP_BIT;
    if ((fscanf(fp_dev_config_file, "%d ",&matrix_control_data.matrix_control_serial_pamater.serialAttr.byParity)) != 1)
        matrix_control_data.matrix_control_serial_pamater.serialAttr.byParity = MATRIX_UART_CHECK_BIT;
    if ((fscanf(fp_dev_config_file, "%d\n",&matrix_control_data.matrix_control_serial_pamater.serialAttr.byFlowcontrol)) != 1)
        matrix_control_data.matrix_control_serial_pamater.serialAttr.byFlowcontrol = 0;
    #endif
    if ((fscanf(fp_dev_config_file, "%d ",&temp)) == 1)
        matrix_control_data.matrix_control_serial_pamater.byPathNo = temp;
    if ((fscanf(fp_dev_config_file, "%d ",&temp)) == 1)
        matrix_control_data.matrix_control_serial_pamater.serialAttr.dwBaudRate = temp;
    if ((fscanf(fp_dev_config_file, "%d ",&temp)) == 1)
        matrix_control_data.matrix_control_serial_pamater.serialAttr.byDataBit = temp;
    if ((fscanf(fp_dev_config_file, "%d ",&temp)) == 1)
        matrix_control_data.matrix_control_serial_pamater.serialAttr.byStopBit = temp;
    if ((fscanf(fp_dev_config_file, "%d ",&temp)) == 1)
        matrix_control_data.matrix_control_serial_pamater.serialAttr.byParity = temp;
    if ((fscanf(fp_dev_config_file, "%d\n",&temp)) == 1)
        matrix_control_data.matrix_control_serial_pamater.serialAttr.byFlowcontrol = temp;
}

void init_cradle_head_control_serial_pamater(FILE *fp_dev_config_file)
{
    int temp = 0;
    #if 0
    if ((fscanf(fp_dev_config_file, "%d ",&cradle_head_control_data.cradle_head_control_serial_pamater.byPathNo)) != 1)
        cradle_head_control_data.cradle_head_control_serial_pamater.byPathNo = CRADLE_HEAD_SERIAL_PORT;
    if ((fscanf(fp_dev_config_file, "%d ",&cradle_head_control_data.cradle_head_control_serial_pamater.serialAttr.dwBaudRate)) != 1)
        cradle_head_control_data.cradle_head_control_serial_pamater.serialAttr.dwBaudRate = CRADLE_HEAD_UART_BOARD;
    if ((fscanf(fp_dev_config_file, "%d ",&cradle_head_control_data.cradle_head_control_serial_pamater.serialAttr.byDataBit)) != 1)
        cradle_head_control_data.cradle_head_control_serial_pamater.serialAttr.byDataBit = CRADLE_HEAD_UART_DATA_BIT;
    if ((fscanf(fp_dev_config_file, "%d ",&cradle_head_control_data.cradle_head_control_serial_pamater.serialAttr.byStopBit)) != 1)
        cradle_head_control_data.cradle_head_control_serial_pamater.serialAttr.byStopBit = CRADLE_HEAD_UART_STOP_BIT;
    if ((fscanf(fp_dev_config_file, "%d ",&cradle_head_control_data.cradle_head_control_serial_pamater.serialAttr.byParity)) != 1)
        cradle_head_control_data.cradle_head_control_serial_pamater.serialAttr.byParity = CRADLE_HEAD_UART_CHECK_BIT;
    if ((fscanf(fp_dev_config_file, "%d\n",&cradle_head_control_data.cradle_head_control_serial_pamater.serialAttr.byFlowcontrol)) != 1)
        cradle_head_control_data.cradle_head_control_serial_pamater.serialAttr.byFlowcontrol = 0;
    #endif
    if ((fscanf(fp_dev_config_file, "%d ",&temp)) == 1)
        cradle_head_control_data.cradle_head_control_serial_pamater.byPathNo = temp;
    if ((fscanf(fp_dev_config_file, "%d ",&temp)) == 1)
        cradle_head_control_data.cradle_head_control_serial_pamater.serialAttr.dwBaudRate = temp;
    if ((fscanf(fp_dev_config_file, "%d ",&temp)) == 1)
        cradle_head_control_data.cradle_head_control_serial_pamater.serialAttr.byDataBit = temp;
    if ((fscanf(fp_dev_config_file, "%d ",&temp)) == 1)
        cradle_head_control_data.cradle_head_control_serial_pamater.serialAttr.byStopBit = temp;
    if ((fscanf(fp_dev_config_file, "%d ",&temp)) == 1)
        cradle_head_control_data.cradle_head_control_serial_pamater.serialAttr.byParity = temp;
    if ((fscanf(fp_dev_config_file, "%d\n",&temp)) == 1)
        cradle_head_control_data.cradle_head_control_serial_pamater.serialAttr.byFlowcontrol = temp;
}

void init_ck2316_alarm_serial_pamater(FILE *fp_dev_config_file)
{
    int temp;

    #if 0
    if ((fscanf(fp_dev_config_file, "%d ",&ck2316_alarm_data.ck2316_alarm_serial_pamater.byPathNo)) != 1)
        ck2316_alarm_data.ck2316_alarm_serial_pamater.byPathNo = CK2316_SERIAL_PORT;
    if ((fscanf(fp_dev_config_file, "%d ",&ck2316_alarm_data.ck2316_alarm_serial_pamater.serialAttr.dwBaudRate)) != 1)
        ck2316_alarm_data.ck2316_alarm_serial_pamater.serialAttr.dwBaudRate = CK2316_UART_BOARD;
    if ((fscanf(fp_dev_config_file, "%d ",&ck2316_alarm_data.ck2316_alarm_serial_pamater.serialAttr.byDataBit)) != 1)
        ck2316_alarm_data.ck2316_alarm_serial_pamater.serialAttr.byDataBit = CK2316_UART_DATA_BIT;
    if ((fscanf(fp_dev_config_file, "%d ",&ck2316_alarm_data.ck2316_alarm_serial_pamater.serialAttr.byStopBit)) != 1)
        ck2316_alarm_data.ck2316_alarm_serial_pamater.serialAttr.byStopBit = CK2316_UART_STOP_BIT;
    if ((fscanf(fp_dev_config_file, "%d ",&ck2316_alarm_data.ck2316_alarm_serial_pamater.serialAttr.byParity)) != 1)
        ck2316_alarm_data.ck2316_alarm_serial_pamater.serialAttr.byParity = CK2316_UART_CHECK_BIT;
    if ((fscanf(fp_dev_config_file, "%d",&ck2316_alarm_data.ck2316_alarm_serial_pamater.serialAttr.byFlowcontrol)) != 1)
        ck2316_alarm_data.ck2316_alarm_serial_pamater.serialAttr.byFlowcontrol = 0;
    #endif
    if ((fscanf(fp_dev_config_file, "%d ",&temp)) == 1)
        ck2316_alarm_data.ck2316_alarm_serial_pamater.byPathNo = temp;
    if ((fscanf(fp_dev_config_file, "%d ",&temp)) == 1)
        ck2316_alarm_data.ck2316_alarm_serial_pamater.serialAttr.dwBaudRate = temp;
    if ((fscanf(fp_dev_config_file, "%d ",&temp)) == 1)
        ck2316_alarm_data.ck2316_alarm_serial_pamater.serialAttr.byDataBit = temp;
    if ((fscanf(fp_dev_config_file, "%d ",&temp)) == 1)
        ck2316_alarm_data.ck2316_alarm_serial_pamater.serialAttr.byStopBit = temp;
    if ((fscanf(fp_dev_config_file, "%d ",&temp)) == 1)
        ck2316_alarm_data.ck2316_alarm_serial_pamater.serialAttr.byParity = temp;
    if ((fscanf(fp_dev_config_file, "%d",&temp)) == 1)
        ck2316_alarm_data.ck2316_alarm_serial_pamater.serialAttr.byFlowcontrol = temp;
}

int set_serial_dev_parameter(FILE *fp_dev_config_file)
{
    fseek(fp_dev_config_file, 0, SEEK_SET);
    printf_debug("\n\nEntrance guard attribution:\t%d %d %d %d %d %d\n\n",entrance_guard_data.entrance_guard_serial_pamater.byPathNo, entrance_guard_data.entrance_guard_serial_pamater.serialAttr.dwBaudRate, entrance_guard_data.entrance_guard_serial_pamater.serialAttr.byDataBit, entrance_guard_data.entrance_guard_serial_pamater.serialAttr.byStopBit, entrance_guard_data.entrance_guard_serial_pamater.serialAttr.byParity, entrance_guard_data.entrance_guard_serial_pamater.serialAttr.byFlowcontrol);
    fprintf(fp_dev_config_file, "%d %d %d %d %d %d\n",entrance_guard_data.entrance_guard_serial_pamater.byPathNo, entrance_guard_data.entrance_guard_serial_pamater.serialAttr.dwBaudRate, entrance_guard_data.entrance_guard_serial_pamater.serialAttr.byDataBit, entrance_guard_data.entrance_guard_serial_pamater.serialAttr.byStopBit, entrance_guard_data.entrance_guard_serial_pamater.serialAttr.byParity, entrance_guard_data.entrance_guard_serial_pamater.serialAttr.byFlowcontrol);

    printf_debug("Matrix control attribution:\t%d %d %d %d %d %d\n\n",matrix_control_data.matrix_control_serial_pamater.byPathNo, matrix_control_data.matrix_control_serial_pamater.serialAttr.dwBaudRate, matrix_control_data.matrix_control_serial_pamater.serialAttr.byDataBit, matrix_control_data.matrix_control_serial_pamater.serialAttr.byStopBit, matrix_control_data.matrix_control_serial_pamater.serialAttr.byParity, matrix_control_data.matrix_control_serial_pamater.serialAttr.byFlowcontrol);
    fprintf(fp_dev_config_file, "%d %d %d %d %d %d\n",matrix_control_data.matrix_control_serial_pamater.byPathNo, matrix_control_data.matrix_control_serial_pamater.serialAttr.dwBaudRate, matrix_control_data.matrix_control_serial_pamater.serialAttr.byDataBit, matrix_control_data.matrix_control_serial_pamater.serialAttr.byStopBit, matrix_control_data.matrix_control_serial_pamater.serialAttr.byParity, matrix_control_data.matrix_control_serial_pamater.serialAttr.byFlowcontrol);

    printf_debug("Cradle head attribution:\t%d %d %d %d %d %d\n\n",cradle_head_control_data.cradle_head_control_serial_pamater.byPathNo, cradle_head_control_data.cradle_head_control_serial_pamater.serialAttr.dwBaudRate, cradle_head_control_data.cradle_head_control_serial_pamater.serialAttr.byDataBit, cradle_head_control_data.cradle_head_control_serial_pamater.serialAttr.byStopBit, cradle_head_control_data.cradle_head_control_serial_pamater.serialAttr.byParity, cradle_head_control_data.cradle_head_control_serial_pamater.serialAttr.byFlowcontrol);
    fprintf(fp_dev_config_file, "%d %d %d %d %d %d\n",cradle_head_control_data.cradle_head_control_serial_pamater.byPathNo, cradle_head_control_data.cradle_head_control_serial_pamater.serialAttr.dwBaudRate, cradle_head_control_data.cradle_head_control_serial_pamater.serialAttr.byDataBit, cradle_head_control_data.cradle_head_control_serial_pamater.serialAttr.byStopBit, cradle_head_control_data.cradle_head_control_serial_pamater.serialAttr.byParity, cradle_head_control_data.cradle_head_control_serial_pamater.serialAttr.byFlowcontrol);

    printf_debug("Ck2316 alarm attribution:\t%d %d %d %d %d %d\n\n",ck2316_alarm_data.ck2316_alarm_serial_pamater.byPathNo, ck2316_alarm_data.ck2316_alarm_serial_pamater.serialAttr.dwBaudRate, ck2316_alarm_data.ck2316_alarm_serial_pamater.serialAttr.byDataBit, ck2316_alarm_data.ck2316_alarm_serial_pamater.serialAttr.byStopBit, ck2316_alarm_data.ck2316_alarm_serial_pamater.serialAttr.byParity, ck2316_alarm_data.ck2316_alarm_serial_pamater.serialAttr.byFlowcontrol);
    fprintf(fp_dev_config_file, "%d %d %d %d %d %d",ck2316_alarm_data.ck2316_alarm_serial_pamater.byPathNo, ck2316_alarm_data.ck2316_alarm_serial_pamater.serialAttr.dwBaudRate, ck2316_alarm_data.ck2316_alarm_serial_pamater.serialAttr.byDataBit, ck2316_alarm_data.ck2316_alarm_serial_pamater.serialAttr.byStopBit, ck2316_alarm_data.ck2316_alarm_serial_pamater.serialAttr.byParity, ck2316_alarm_data.ck2316_alarm_serial_pamater.serialAttr.byFlowcontrol);

    fflush(fp_dev_config_file);

    return 0;
}

void init_serial_dev_parameter(void)
{
    FILE *fp_dev_config_file;

    if ((fp_dev_config_file = fopen(UART_DEV_CONFIG_FILE, "a+")) == NULL)
    {
        printf("FUNC[%s] LINE[%d]\tOpen %s error!\n",__FUNCTION__, __LINE__, UART_DEV_CONFIG_FILE);
        exit(1);
    }
    init_entrance_guard_serial_pamater(fp_dev_config_file);
    init_matrix_control_serial_pamater(fp_dev_config_file);
    init_cradle_head_control_serial_pamater(fp_dev_config_file);
    init_ck2316_alarm_serial_pamater(fp_dev_config_file);
    fclose(fp_dev_config_file);

    if ((fp_dev_config_file = fopen(UART_DEV_CONFIG_FILE, "w+")) == NULL)
    {
        printf("FUNC[%s] LINE[%d]\tOpen %s error!\n",__FUNCTION__, __LINE__, UART_DEV_CONFIG_FILE);
        exit(1);
    }
    set_serial_dev_parameter(fp_dev_config_file);
}
