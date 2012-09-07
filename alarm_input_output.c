#include <stdio.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>
#include <pthread.h>
#include "global.h"
#include "net.h"
#include "alarm_input_output.h"
#include "entrance_guard.h"

#define DEBUG
#ifdef DEBUG
#define printf_debug(fmt, arg...) printf(fmt, ##arg)
#else
#define printf_debug(fmt, arg...) do{}while(0)
#endif

#define ALARM_GPIO_DEVICE_NAME     "/dev/gpio_alarm_in_out"

#define AT91_ALARM_IO1              0
#define AT91_ALARM_IO2              1
#define AT91_ALARM_IO3              2
#define AT91_ALARM_IO4              3
#define AT91_ALARM_IO5              4
#define AT91_ALARM_IO6              5
#define AT91_ALARM_IO7              6
#define AT91_ALARM_IO8              7
#define MAX_ALARM_IO_NUM            8
unsigned int const alarm_input_output_io_table[MAX_ALARM_IO_NUM] = {
    AT91_ALARM_IO1, 
    AT91_ALARM_IO2, 
    AT91_ALARM_IO3, 
    AT91_ALARM_IO4, 
    AT91_ALARM_IO5, 
    AT91_ALARM_IO6, 
    AT91_ALARM_IO7, 
    AT91_ALARM_IO8 
};


#define INPUT                       0
#define OUTPUT                      1

#define SET_IO_DATA_AS_LOW          0
#define SET_IO_DATA_AS_HIGH         1
#define GET_IO_DATA                 3
#define CHANGE_INPUT_OUTPUT_STATUS  4
#define MAX_COMMAND_NUM             4

#define ALARM_OUTPUT_NORMALLY_OPEN      SET_IO_DATA_AS_LOW
#define ALARM_OUTPUT_NORMALLY_CLOSE     SET_IO_DATA_AS_HIGH

#define ENABLE_ALARM_OUTPUT             ALARM_OUTPUT_NORMALLY_CLOSE
#define DISABLE_ALARM_OUTPUT            ALARM_OUTPUT_NORMALLY_OPEN 
#define GET_ALARM_INPUT_VALUE           GET_IO_DATA

#define ALARM_LINKAGE_OUTPUT_OBJECT_1           0x01
#define ALARM_LINKAGE_OUTPUT_OBJECT_2           0x02
#define ALARM_LINKAGE_OUTPUT_OBJECT_3           0x04
#define ALARM_LINKAGE_OUTPUT_OBJECT_4           0x08
#define ALARM_LINKAGE_OUTPUT_OBJECT_5           0x10
#define ALARM_LINKAGE_OUTPUT_OBJECT_6           0x20
#define ALARM_LINKAGE_OUTPUT_OBJECT_7           0x40
#define ALARM_LINKAGE_OUTPUT_OBJECT_8           0x80


#define DEFAULT_ALARM_DURATION                      5   //5分钟

alarm_input_output_arg alarm_input_output_data = {0, NO, ALARM_INPUT_OUTPUT_NORMAL_OPERATION, MAX_ALARM_LINKAGE_INPUT_OBJECT,  0, MAX_ALARM_INPUT_LINKAGE_OUTPUT_OBJECT, 0, {ALARM_LINKAGE_OUTPUT_OBJECT_1, ALARM_LINKAGE_OUTPUT_OBJECT_2, ALARM_LINKAGE_OUTPUT_OBJECT_3, ALARM_LINKAGE_OUTPUT_OBJECT_4, ALARM_LINKAGE_OUTPUT_OBJECT_5, ALARM_LINKAGE_OUTPUT_OBJECT_6, ALARM_LINKAGE_OUTPUT_OBJECT_7, ALARM_LINKAGE_OUTPUT_OBJECT_8, ALARM_LINKAGE_OUTPUT_OBJECT_1}, 0, DEFAULT_ALARM_DURATION};


FILE *fp_alarm_input_output_config_file = NULL;
int alarm_input_output_fd;
pthread_mutex_t alarm_output_mutex = PTHREAD_MUTEX_INITIALIZER;

extern int server_sock_tcp, client_sock_tcp;
extern int if_have_net_client_connect;

extern void print_string(char *string, unsigned char *buffer, unsigned int len);
int open_alarm_input_output_device(int *alarm_input_output_fd);
int change_alarm_input_output_status(int alarm_input_output_fd, int input_output_command);
int enable_and_disable_alarm_out(int alarm_out_fd, int alarm_output_io_pin, int enable_disable_command);

int alarm_input_output_setup(int *alarm_input_output_fd, FILE *fp_config_file, FILE *fp_timed_alarm_file, int *timed_alarm_count_array);
int get_alarm_input_value(int alarm_input_fd, int alarm_input_io_pin);
int get_alarm_input_data(int *alarm_input_output_fd, unsigned int *alarm_input_io_status, int *timed_alarm_count_array, int *temp_timed_alarm_count_array);
int start_alarm_output(int *alarm_input_output_fd);
void alarm_input_output_default_setup(int *temp_timed_alarm_count_array);


#define DELAY_N_SECOND              5
//#define DELAY_N_MINUTE_COUNT(n)     ((n)*(60/DELAY_N_SECOND))
#define CHANGE_MINUTE_TO_COUNT(n)     ((n)*(60/DELAY_N_SECOND))
#define CHANGE_COUNT_TO_MINUTE(n)     (((n)*DELAY_N_SECOND)/60)


void *pthread_timed_alarm(void *arg)
{
    int i = 0;
    int *timed_alarm_count_array = (int *)arg;

    while (1) 
    {
        if (alarm_input_output_data.real_time_alarm_output_objcet) 
        {
            for (i = 0; i < MAX_ALARM_INPUT_LINKAGE_OUTPUT_OBJECT; i++) 
            {
                if (timed_alarm_count_array[i] > 0)
                    timed_alarm_count_array[i]--;
                else
                {
                    if (alarm_input_output_data.real_time_alarm_output_objcet & (1<<i)) 
                    {
                        pthread_mutex_lock(&alarm_output_mutex);
                        alarm_input_output_data.real_time_alarm_output_objcet &= ~(1<<i);
                        if (start_alarm_output(&alarm_input_output_fd) < 0)
                        {
                            printf("FUNC[%s] LINE[%d]\tstart alarm output error\n",__FUNCTION__, __LINE__);
                            //return ret;
                        }
                        pthread_mutex_unlock(&alarm_output_mutex);
                    }               
                }
            }
            printf_debug("\nTimed alarm count: %04d %04d %04d %04d %04d %04d %04d %04d \n",timed_alarm_count_array[0], timed_alarm_count_array[1], timed_alarm_count_array[2], timed_alarm_count_array[3], timed_alarm_count_array[4], timed_alarm_count_array[5], timed_alarm_count_array[6], timed_alarm_count_array[7]);
            sleep(DELAY_N_SECOND);
        }
        else
        {
            printf("no alarm\n");
            sleep(1);
        }
    }

    return (void *)1;
}

void *pthread_alarm_input_output(void *arg)
{
    int ret = 0;
    int i = 0;
    int timed_alarm_count_array[MAX_ALARM_INPUT_LINKAGE_OUTPUT_OBJECT] = {0};
    FILE *fp_config_file = NULL;
    FILE *fp_timed_alarm_file = NULL;

    pthread_t tid_timed_alarm;

    while (1) 
    {
        ret = open_alarm_input_output_device(&alarm_input_output_fd);
        if (ret <= 0) 
        {
            printf("FUNC[%s] LINE[%d]\tOpen alarm input output device failed, ret = %d\n",__FUNCTION__, __LINE__,ret);
            alarm_input_output_data.if_alarm_input_output_device_is_ready = NO;
            sleep(20);
        }
        else
        {
            alarm_input_output_data.if_alarm_input_output_device_is_ready = YES;
            break;
        }
    }

try_again:

    if ((fp_config_file = fopen(ALARM_INPUT_OUTPUT_CONFIG_FILE, "a+")) == NULL)
    {
        printf("FUNC[%s] LINE[%d]\tOpen %s error!\n",__FUNCTION__, __LINE__, ALARM_INPUT_OUTPUT_CONFIG_FILE);
        exit(1);
    }
    fseek(fp_config_file, 0, SEEK_SET);
    for (i = 0; i < MAX_ALARM_LINKAGE_INPUT_OBJECT-1; i++) 
    {
        if ((fscanf(fp_config_file, "%03d ",&alarm_input_output_data.alarm_input_linkage_output_object[i])) != 1)
        {
            alarm_input_output_data.alarm_input_linkage_output_object[i] = (1 << i); 
        }
    }
    if ((fscanf(fp_config_file, "%03d ",&alarm_input_output_data.alarm_input_linkage_output_object[8])) != 1)
    {
        alarm_input_output_data.alarm_input_linkage_output_object[8] = ALARM_LINKAGE_OUTPUT_OBJECT_1; 
    }
    fclose(fp_config_file);
    if ((fp_config_file = fopen(ALARM_INPUT_OUTPUT_CONFIG_FILE, "w+")) == NULL)
    {
        printf("FUNC[%s] LINE[%d]\tOpen %s error!\n",__FUNCTION__, __LINE__, ALARM_INPUT_OUTPUT_CONFIG_FILE);
        exit(1);
    }
    printf_debug("\nAlarm linkage arg: 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X\n",alarm_input_output_data.alarm_input_linkage_output_object[0], alarm_input_output_data.alarm_input_linkage_output_object[1], alarm_input_output_data.alarm_input_linkage_output_object[2], alarm_input_output_data.alarm_input_linkage_output_object[3], alarm_input_output_data.alarm_input_linkage_output_object[4], alarm_input_output_data.alarm_input_linkage_output_object[5], alarm_input_output_data.alarm_input_linkage_output_object[6], alarm_input_output_data.alarm_input_linkage_output_object[7], alarm_input_output_data.alarm_input_linkage_output_object[8]);
    fprintf(fp_config_file, "%03d %03d %03d %03d %03d %03d %03d %03d %03d ",alarm_input_output_data.alarm_input_linkage_output_object[0], alarm_input_output_data.alarm_input_linkage_output_object[1], alarm_input_output_data.alarm_input_linkage_output_object[2], alarm_input_output_data.alarm_input_linkage_output_object[3], alarm_input_output_data.alarm_input_linkage_output_object[4], alarm_input_output_data.alarm_input_linkage_output_object[5], alarm_input_output_data.alarm_input_linkage_output_object[6], alarm_input_output_data.alarm_input_linkage_output_object[7], alarm_input_output_data.alarm_input_linkage_output_object[8]);
    fflush(fp_config_file);

    //存储定时报警时间的文件
    if ((fp_timed_alarm_file = fopen(ALARM_INPUT_OUTPUT_TIMED_ALARM_FILE, "a+")) == NULL)
    {
        printf("FUNC[%s] LINE[%d]\tOpen %s error!\n",__FUNCTION__, __LINE__, ALARM_INPUT_OUTPUT_TIMED_ALARM_FILE);
        exit(1);
    }
    fseek(fp_timed_alarm_file, 0, SEEK_SET);
    for (i = 0; i < MAX_ALARM_INPUT_LINKAGE_OUTPUT_OBJECT; i++) 
    {
        if ((fscanf(fp_timed_alarm_file, "%04d ",&timed_alarm_count_array[i])) != 1)
        {
            timed_alarm_count_array[i] = CHANGE_MINUTE_TO_COUNT(DEFAULT_ALARM_DURATION); 
        }
    }
    fclose(fp_timed_alarm_file);
    if ((fp_timed_alarm_file = fopen(ALARM_INPUT_OUTPUT_TIMED_ALARM_FILE, "w+")) == NULL)
    {
        printf("FUNC[%s] LINE[%d]\tOpen %s error!\n",__FUNCTION__, __LINE__, ALARM_INPUT_OUTPUT_TIMED_ALARM_FILE);
        exit(1);
    }
    printf_debug("\nTimed alarm count: %04d %04d %04d %04d %04d %04d %04d %04d \n",timed_alarm_count_array[0], timed_alarm_count_array[1], timed_alarm_count_array[2], timed_alarm_count_array[3], timed_alarm_count_array[4], timed_alarm_count_array[5], timed_alarm_count_array[6], timed_alarm_count_array[7]);

    fprintf(fp_timed_alarm_file, "%04d %04d %04d %04d %04d %04d %04d %04d ",timed_alarm_count_array[0], timed_alarm_count_array[1], timed_alarm_count_array[2], timed_alarm_count_array[3], timed_alarm_count_array[4], timed_alarm_count_array[5], timed_alarm_count_array[6], timed_alarm_count_array[7]);
    fflush(fp_timed_alarm_file);

    //创建报警定时线程
    if (pthread_create(&tid_timed_alarm, NULL, pthread_timed_alarm, timed_alarm_count_array) != 0) 
    {
        printf("FUNC[%s] LINE[%d]\tCan't create timed alarm thread !\n",__FUNCTION__, __LINE__);
        exit(1);
    }

    ret = alarm_input_output_setup(&alarm_input_output_fd, fp_config_file, fp_timed_alarm_file, timed_alarm_count_array);
    if (ret < 0) 
    {
        pthread_cancel(tid_timed_alarm);
        fclose(fp_config_file);
        fclose(fp_timed_alarm_file);
        sleep(10);
        printf("FUNC[%s] LINE[%d]\talarm_input_output_setup error! try it again.\n",__FUNCTION__, __LINE__);
        goto try_again;
    }

    return (void *)1;
}

int alarm_input_output_setup(int *alarm_input_output_fd, FILE *fp_config_file, FILE *fp_timed_alarm_file, int *timed_alarm_count_array)
{
    int i = 0;
    int ret = 0;
    unsigned int alarm_input_io_status; 
    int temp_timed_alarm_count_array[MAX_ALARM_INPUT_LINKAGE_OUTPUT_OBJECT];

    for (i = 0; i < MAX_ALARM_INPUT_LINKAGE_OUTPUT_OBJECT; i++) 
    {
        temp_timed_alarm_count_array[i] = timed_alarm_count_array[i];
    }

    while (1) 
    {
        switch(alarm_input_output_data.setup_command_set)
        {
            case ALARM_INPUT_OUTPUT_NORMAL_OPERATION:
                if (entrance_guard_data.if_has_entrance_guard_alarm == YES) 
                {
                    pthread_mutex_lock(&alarm_output_mutex);
                    alarm_input_output_data.real_time_alarm_output_objcet |= alarm_input_output_data.alarm_input_linkage_output_object[9];
                    for (i = 0; i < MAX_ALARM_INPUT_LINKAGE_OUTPUT_OBJECT; i++) 
                    {
                        if (alarm_input_output_data.alarm_input_linkage_output_object[9] & (1<<i)) 
                        {
                            timed_alarm_count_array[i] = temp_timed_alarm_count_array[i];
                        } 
                    }
                    if ((ret = start_alarm_output(alarm_input_output_fd)) < 0)
                    {
                        printf("FUNC[%s] LINE[%d]\tstart alarm output error, ret = %d\n",__FUNCTION__, __LINE__,ret);
                        return ret;
                    }
                    pthread_mutex_unlock(&alarm_output_mutex);
                    entrance_guard_data.if_has_entrance_guard_alarm = NO;
                }

                if ((get_alarm_input_data(alarm_input_output_fd, &alarm_input_io_status, timed_alarm_count_array, temp_timed_alarm_count_array)) < 0)
                {
                    printf("FUNC[%s] LINE[%d]\tget alarm input data error, ret = %d\n",__FUNCTION__, __LINE__,ret);
                    return ret;
                }

                if (alarm_input_io_status) 
                {
                    if ((ret = start_alarm_output(alarm_input_output_fd)) < 0)
                    {
                        printf("FUNC[%s] LINE[%d]\tstart alarm output error, ret = %d\n",__FUNCTION__, __LINE__,ret);
                        return ret;
                    }
                }
                usleep(10000);
                break;
            case ALARM_INPUT_OUTPUT_SET_AND_CANCEL_LINKAGE_ALARM:
                pthread_mutex_lock(&alarm_output_mutex);
                alarm_input_output_data.real_time_alarm_output_objcet = alarm_input_output_data.set_and_cancel_linkage_alarm_channel; 
                if ((ret = start_alarm_output(alarm_input_output_fd)) < 0)
                {
                    printf("FUNC[%s] LINE[%d]\tstart alarm output error, ret = %d\n",__FUNCTION__, __LINE__,ret);
                    return ret;
                }
                pthread_mutex_unlock(&alarm_output_mutex);
                //网络发送 成功取消报警输出
                printf_debug("FUNC[%s] LINE[%d]\tSucceed to set alarm output\n",__FUNCTION__, __LINE__);
                alarm_input_output_data.setup_command_set = ALARM_INPUT_OUTPUT_NORMAL_OPERATION;

                break;
            case ALARM_INPUT_OUTPUT_SET_LINKAGE_INFO:
                fseek(fp_config_file, 0, SEEK_SET);
                fprintf(fp_config_file, "%03d %03d %03d %03d %03d %03d %03d %03d %03d ",alarm_input_output_data.alarm_input_linkage_output_object[0], alarm_input_output_data.alarm_input_linkage_output_object[1], alarm_input_output_data.alarm_input_linkage_output_object[2], alarm_input_output_data.alarm_input_linkage_output_object[3], alarm_input_output_data.alarm_input_linkage_output_object[4], alarm_input_output_data.alarm_input_linkage_output_object[5], alarm_input_output_data.alarm_input_linkage_output_object[6], alarm_input_output_data.alarm_input_linkage_output_object[7], alarm_input_output_data.alarm_input_linkage_output_object[8]);
                fflush(fp_config_file);
                //网络发送 成功设置报警联动对应关系
                printf_debug("FUNC[%s] LINE[%d]\tSucceed to set alarm outlarm_input_output_data.alarm_linkage_output_objectput linkage\n",__FUNCTION__, __LINE__);
                alarm_input_output_data.setup_command_set = ALARM_INPUT_OUTPUT_NORMAL_OPERATION;
                break;
            case ALARM_INPUT_OUTPUT_RESTORE_TO_DEFAULT:
                alarm_input_output_default_setup(temp_timed_alarm_count_array);
                fseek(fp_config_file, 0, SEEK_SET);
                fprintf(fp_config_file, "%03d %03d %03d %03d %03d %03d %03d %03d %03d ",alarm_input_output_data.alarm_input_linkage_output_object[0], alarm_input_output_data.alarm_input_linkage_output_object[1], alarm_input_output_data.alarm_input_linkage_output_object[2], alarm_input_output_data.alarm_input_linkage_output_object[3], alarm_input_output_data.alarm_input_linkage_output_object[4], alarm_input_output_data.alarm_input_linkage_output_object[5], alarm_input_output_data.alarm_input_linkage_output_object[6], alarm_input_output_data.alarm_input_linkage_output_object[7], alarm_input_output_data.alarm_input_linkage_output_object[8]);
                fflush(fp_config_file);
                printf_debug("\nAlarm linkage arg: 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X\n",alarm_input_output_data.alarm_input_linkage_output_object[0], alarm_input_output_data.alarm_input_linkage_output_object[1], alarm_input_output_data.alarm_input_linkage_output_object[2], alarm_input_output_data.alarm_input_linkage_output_object[3], alarm_input_output_data.alarm_input_linkage_output_object[4], alarm_input_output_data.alarm_input_linkage_output_object[5], alarm_input_output_data.alarm_input_linkage_output_object[6], alarm_input_output_data.alarm_input_linkage_output_object[7], alarm_input_output_data.alarm_input_linkage_output_object[8]);

                fseek(fp_timed_alarm_file, 0, SEEK_SET);
                fprintf(fp_timed_alarm_file, "%04d %04d %04d %04d %04d %04d %04d %04d ",temp_timed_alarm_count_array[0], temp_timed_alarm_count_array[1], temp_timed_alarm_count_array[2], temp_timed_alarm_count_array[3], temp_timed_alarm_count_array[4], temp_timed_alarm_count_array[5], temp_timed_alarm_count_array[6], temp_timed_alarm_count_array[7]);
                fflush(fp_timed_alarm_file);
                printf_debug("\nTimed alarm count: %04d %04d %04d %04d %04d %04d %04d %04d \n",temp_timed_alarm_count_array[0], temp_timed_alarm_count_array[1], temp_timed_alarm_count_array[2], temp_timed_alarm_count_array[3], temp_timed_alarm_count_array[4], temp_timed_alarm_count_array[5], temp_timed_alarm_count_array[6], temp_timed_alarm_count_array[7]);
                //网络发送 成功恢复默认设置
                printf_debug("FUNC[%s] LINE[%d]\tAlarm input and output arg restore to default successfully\n",__FUNCTION__, __LINE__);
                alarm_input_output_data.setup_command_set = ALARM_INPUT_OUTPUT_NORMAL_OPERATION;
                break;
                //case ALARM_INPUT_OUTPUT_SET_LINKAGE_INFO:
                //break;
                //case ALARM_INPUT_OUTPUT_GET_LINKAGE_INFO:
                //break;
            case ALARM_INPUT_OUTPUT_SET_ALARM_DURATION:  //设置报警输出持续时间
                if (alarm_input_output_data.alarm_linkage_output_object == 0) 
                {
                    for (i = 0; i < MAX_ALARM_INPUT_LINKAGE_OUTPUT_OBJECT; i++) 
                    {
                        temp_timed_alarm_count_array[i] = alarm_input_output_data.alarm_linkage_output_object;
                    }
                }
                else
                {
                    temp_timed_alarm_count_array[alarm_input_output_data.alarm_linkage_output_object-1] = CHANGE_MINUTE_TO_COUNT(alarm_input_output_data.alarm_duration);
                }
                fseek(fp_timed_alarm_file, 0, SEEK_SET);
                fprintf(fp_timed_alarm_file, "%04d %04d %04d %04d %04d %04d %04d %04d ",temp_timed_alarm_count_array[0], temp_timed_alarm_count_array[1], temp_timed_alarm_count_array[2], temp_timed_alarm_count_array[3], temp_timed_alarm_count_array[4], temp_timed_alarm_count_array[5], temp_timed_alarm_count_array[6], temp_timed_alarm_count_array[7]);
                fflush(fp_timed_alarm_file);
                printf_debug("\nTimed alarm count: %04d %04d %04d %04d %04d %04d %04d %04d \n",temp_timed_alarm_count_array[0], temp_timed_alarm_count_array[1], temp_timed_alarm_count_array[2], temp_timed_alarm_count_array[3], temp_timed_alarm_count_array[4], temp_timed_alarm_count_array[5], temp_timed_alarm_count_array[6], temp_timed_alarm_count_array[7]);
                //网络发送 成功设置报警输出持续时间
                printf_debug("FUNC[%s] LINE[%d]\tSet alarm duration successfully\n",__FUNCTION__, __LINE__);
                alarm_input_output_data.setup_command_set = ALARM_INPUT_OUTPUT_NORMAL_OPERATION;
                break;
            case ALARM_INPUT_OUTPUT_GET_ALARM_DURATION:  //get报警输出持续时间
                alarm_input_output_data.alarm_duration = CHANGE_COUNT_TO_MINUTE(temp_timed_alarm_count_array[alarm_input_output_data.alarm_linkage_output_object-1]);
                //网络发送 成功get报警输出持续时间,then send data
                printf_debug("FUNC[%s] LINE[%d]\t alarm output channel %d duration is %02d:%02d:00\n",__FUNCTION__, __LINE__, alarm_input_output_data.alarm_linkage_output_object, alarm_input_output_data.alarm_duration/60, alarm_input_output_data.alarm_duration%60);
                alarm_input_output_data.setup_command_set = ALARM_INPUT_OUTPUT_NORMAL_OPERATION;
                break;
            default:
                break;
        }
    }

    return 1;
}

int get_alarm_input_data(int *alarm_input_output_fd, unsigned int *alarm_input_io_status, int *timed_alarm_count_array, int *temp_timed_alarm_count_array)
{
    int ret = 0;
    int i = 0, j = 0;
    time_t tm;
    struct tm *t;

    *alarm_input_io_status = 0;
    for (i = 0; i < MAX_ALARM_IO_NUM; i++) 
    {
        ret = get_alarm_input_value(*alarm_input_output_fd, alarm_input_output_io_table[i]);
        if (ret < 0) 
        {
            printf("FUNC[%s] LINE[%d]\tret = %d\n",__FUNCTION__, __LINE__,ret);
            return -1;
        }
        else if(ret == 0)
        {
            usleep(5000);
            if (get_alarm_input_value(*alarm_input_output_fd, alarm_input_output_io_table[i]) == 0) 
            {
                *alarm_input_io_status |= (1 << i);
                //printf_debug("FUNC[%s] LINE[%d]\tElectrical level: %d\n",__FUNCTION__, __LINE__,ret);
                pthread_mutex_lock(&alarm_output_mutex);
                alarm_input_output_data.real_time_alarm_output_objcet |= alarm_input_output_data.alarm_input_linkage_output_object[i];
                for (j = 0; j < MAX_ALARM_INPUT_LINKAGE_OUTPUT_OBJECT; j++) 
                {
                    if (alarm_input_output_data.alarm_input_linkage_output_object[i] & (1<<j)) 
                    {
                        timed_alarm_count_array[j] = temp_timed_alarm_count_array[j];
                    }
                }
                pthread_mutex_unlock(&alarm_output_mutex);
                if (if_have_net_client_connect == YES) 
                {
                    //navy 网络发送 报警上传
                    tm = time(NULL);
                    t = localtime(&tm);
                    printf_debug("FUNC[%s] LINE[%d]\t%04d-%02d-%02d %02d:%02d:%02d ,Input channel %d happen alarm\n",__FUNCTION__, __LINE__, t->tm_year+1900, t->tm_mon+1, t->tm_mday, t->tm_hour, t->tm_min, t->tm_sec, i+1);
                }
            }
        }
    }
    return 0;
}

int start_alarm_output(int *alarm_input_output_fd)
{
    int ret = 0;
    int i = 0;

    if ((ret = change_alarm_input_output_status(*alarm_input_output_fd, OUTPUT)) < 0) 
    {
        printf("FUNC[%s] LINE[%d]\tret = %d\n",__FUNCTION__, __LINE__,ret);
        return ret;
    }

    for (i = 0; i < MAX_ALARM_IO_NUM; i++) 
    {
        if ((ret = enable_and_disable_alarm_out(*alarm_input_output_fd, alarm_input_output_io_table[i], (alarm_input_output_data.real_time_alarm_output_objcet >> i) & 0x01)) < 0)
        {
            printf("FUNC[%s] LINE[%d]\tret = %d\n",__FUNCTION__, __LINE__,ret);
            return ret;
        }
    }

    if ((ret = change_alarm_input_output_status(*alarm_input_output_fd, INPUT)) < 0) 
    {
        printf("FUNC[%s] LINE[%d]\tret = %d\n",__FUNCTION__, __LINE__,ret);
        return ret;
    }

    return 0;
}

int open_alarm_input_output_device(int *alarm_input_output_fd)
{
    *alarm_input_output_fd = open(ALARM_GPIO_DEVICE_NAME, O_WRONLY | O_NONBLOCK);
    if (*alarm_input_output_fd < 0) 
    {
        printf("FUNC[%s] LINE[%d]\t\n",__FUNCTION__, __LINE__);
        perror(ALARM_GPIO_DEVICE_NAME);
        return -1;
    }
    return *alarm_input_output_fd;
}

int change_alarm_input_output_status(int alarm_input_output_fd, int input_output_command)
{
    int ret = 0;

    ret = ioctl(alarm_input_output_fd, CHANGE_INPUT_OUTPUT_STATUS, input_output_command);
    if (ret < 0) 
    {
        printf("FUNC[%s] LINE[%d]\tret = %d\n",__FUNCTION__, __LINE__,ret);
        perror(ALARM_GPIO_DEVICE_NAME);
        return ret;
    }

    return ret;
}

int enable_and_disable_alarm_out(int alarm_out_fd, int alarm_output_io_pin, int enable_disable_command)
{
    int ret = 0;

    if (alarm_output_io_pin < 0 || alarm_output_io_pin > MAX_ALARM_IO_NUM-1 || enable_disable_command > 2) 
    {
        return -1;
    }
    ret = ioctl(alarm_out_fd, enable_disable_command, alarm_output_io_pin);
    if (ret < 0) 
    {
        printf("FUNC[%s] LINE[%d]\tret = %d\n",__FUNCTION__, __LINE__,ret);
        perror(ALARM_GPIO_DEVICE_NAME);
        return ret;
    }

    return ret;
}

int get_alarm_input_value(int alarm_input_fd, int alarm_input_io_pin)
{
    int ret = 0;

    if (alarm_input_io_pin < 0 || alarm_input_io_pin > MAX_ALARM_IO_NUM-1) 
    {
        return -1;
    }
    ret = ioctl(alarm_input_fd, GET_ALARM_INPUT_VALUE, alarm_input_io_pin);
    if (ret < 0) 
    {
        perror(ALARM_GPIO_DEVICE_NAME);
        printf("FUNC[%s] LINE[%d]\tioctl error! ret = %d\n",__FUNCTION__, __LINE__, ret);
        return ret;
    }
    return ret;
}

void alarm_input_output_default_setup(int *temp_timed_alarm_count_array)
{
    int i = 0;

    alarm_input_output_data.setup_command_set = ALARM_INPUT_OUTPUT_NORMAL_OPERATION; 
    alarm_input_output_data.alarm_linkage_input_object = 0; 
    alarm_input_output_data.real_time_alarm_output_objcet = 0x00; 
    alarm_input_output_data.set_and_cancel_linkage_alarm_channel = 0x00; 
    for (i = 0; i < MAX_ALARM_LINKAGE_INPUT_OBJECT-1; i++) 
    {
        alarm_input_output_data.alarm_input_linkage_output_object[i] = (1 << i); 

        temp_timed_alarm_count_array[i] = CHANGE_MINUTE_TO_COUNT(DEFAULT_ALARM_DURATION);
    }
    alarm_input_output_data.alarm_input_linkage_output_object[MAX_ALARM_LINKAGE_INPUT_OBJECT-1] = ALARM_LINKAGE_OUTPUT_OBJECT_1; 
}

