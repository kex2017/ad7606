#include "gps_sync.h"
#include "fault_location_threads.h"

#include <string.h>
#include "x_delay.h"
#include "periph_cpu.h"
#include "gps.h"
#include "periph/rtt.h"
#include "log.h"
#include "thread.h"
#include "env_cfg.h"
#include "daq.h"
#include "periph/uart.h"
#include "isrpipe.h"

#define GPS_INTERVAL  300U
#define GPS_GET_TIME_INTERVAL 200U
#define GPS_GET_COORDINATES_INTERVAL 2U
#define GPS_RX_BUFSIZE    (512)


#define UART_GY25_DEV          UART_DEV(3)
#define UART_GY25_BAUDRATE     (115200)
#define GY25_GET_DIP_ANGLE_INTERVAL 2U


#define GY25_RX_BUFSIZE    (32)
static char _rx_buf_mem[GY25_RX_BUFSIZE];
isrpipe_t uart_gy25_isrpipe = ISRPIPE_INIT(_rx_buf_mem);

uint8_t read_dip_angle_frame[] = {0xA5, 0x51};//查询模式
uint8_t roll_angle_adjust_frame[] = {0xA5, 0x54};//校正模式， 校正俯仰横滚角 0 度， 需要保持水平时候发送
uint8_t course_angle_adjust_frame[] = {0xA5, 0x55};//校正模式，校正航向 0 度， 航向任意角度清零

static GY25 gy25;

void gy25_init(void)
{
    uart_init(UART_GY25_DEV, UART_GY25_BAUDRATE, (uart_rx_cb_t) isrpipe_write_one, &uart_gy25_isrpipe);
    NVIC_ClearPendingIRQ(UART4_IRQn);
    NVIC_DisableIRQ(UART4_IRQn);
}

int gy25_read(char* buffer, int count)
{
   (void) count;
   return isrpipe_read_timeout(&uart_gy25_isrpipe, buffer, GY25_RX_BUFSIZE, 100);
}

void gy25_enable(void)
{
   tsrb_init(&(uart_gy25_isrpipe.tsrb), _rx_buf_mem, GY25_RX_BUFSIZE);
   NVIC_ClearPendingIRQ(UART4_IRQn);
   NVIC_EnableIRQ(UART4_IRQn);
}

void gy25_disable(void)
{
   NVIC_ClearPendingIRQ(UART4_IRQn);
   NVIC_DisableIRQ(UART4_IRQn);
}

uint8_t parse_dip_angle(GY25 *gy25, char *buf)
{
    for(int i = 0; i < GY25_RX_BUFSIZE- GY25_FRAME_LEN; i++){
        if(GY25_FRAME_HEADER == buf[i] && GY25_FRAME_ENDER == buf[i+GY25_FRAME_LEN-1]){
            gy25->course_angle = (int16_t)(buf[i + 1] << 8 | buf[i + 2]) / 100.0;
            gy25->pitch_angle =  (int16_t)(buf[i + 3] << 8 | buf[i + 4]) / 100.0;
            gy25->roll_angle =   (int16_t)(buf[i + 5] << 8 | buf[i + 6]) / 100.0;

            LOG_INFO("get course_angle as %.2f, pitch_angle as %.2f, roll_angle as %.2f\r\n", \
                                                                     gy25->course_angle,\
                                                                     gy25->pitch_angle,\
                                                                     gy25->roll_angle);
            return 1;
        }
    }
    return 0;
}

void adjust_course_angle(void)
{
    uart_write(UART_GY25_DEV, course_angle_adjust_frame, sizeof(course_angle_adjust_frame));
}

void adjust_roll_angle(void)
{
    uart_write(UART_GY25_DEV, roll_angle_adjust_frame, sizeof(roll_angle_adjust_frame));
}

void gy25_read_dip_angle(void)
{
    char buffer[GY25_RX_BUFSIZE];
    memset(buffer, 0, GY25_RX_BUFSIZE);

    gy25_enable();
    uart_write(UART_GY25_DEV, read_dip_angle_frame, sizeof(read_dip_angle_frame));
    delay_s(GY25_GET_DIP_ANGLE_INTERVAL);
    gy25_read(buffer, GY25_RX_BUFSIZE);
    if (!parse_dip_angle(&gy25, buffer)) {
        LOG_ERROR("parse dip angle error!");
    }
    gy25_disable();
}

GY25* get_gy25_dip_angle(void)
{
    return &gy25;
}

static GPS gps;

int parse_gnrmc_msg(char* buf)
{
    struct minmea_sentence_rmc frame;
    struct tm t;
    static time_t tim;
    int len = 0;
    char *gnrmc_start_flag = 0;
    char *gnrmc_end_flag = 0;
    char gnrmc_buf[512] = { 0 };
    gnrmc_start_flag = strstr(buf, "$GNRMC");
    if (gnrmc_start_flag == NULL) {
        return 0;
    }
    else {
        gnrmc_end_flag = strstr(gnrmc_start_flag, "\n");
        len = gnrmc_end_flag - gnrmc_start_flag;
        if (len < 0) {
            return 0;
        }

        strncpy(gnrmc_buf, gnrmc_start_flag, len);

        if (minmea_parse_rmc(&frame, gnrmc_buf)) {

            if (frame.date.year == -1 && frame.date.month == -1 && frame.date.day == -1) {
                return 0;
            }
            else {
                t.tm_year = frame.date.year + 2000;
                t.tm_mon = frame.date.month - 1;
                t.tm_mday = frame.date.day;
                t.tm_hour = frame.time.hours;
                t.tm_min = frame.time.minutes;
                t.tm_sec = frame.time.seconds;
                t.tm_year -= 1900;
                tim = mktime(&t);
                rtt_set_counter((uint32_t)tim);
                LOG_DEBUG("%d-%02d-%02d %02d:%02d:%02d:%d\r\n", frame.date.year, frame.date.month, frame.date.day, frame.time.hours,
                       frame.time.minutes, frame.time.seconds, frame.time.microseconds);
                return 1;
            }
        }

    }
   return 0;
}

int parse_gngga_msg(GPS* gps, char *buf)
{
    struct minmea_sentence_gga frame;
    int len = 0;
    char *gngga_start_flag = 0;
    char *gngga_end_flag = 0;
    char gngga_buf[512] = { 0 };
    gngga_start_flag = strstr(buf, "$GNGGA");
    if (gngga_start_flag == NULL) {
        return 0;
    }
    else {
        gngga_end_flag = strstr(gngga_start_flag, "\n");
        len = gngga_end_flag - gngga_start_flag;
        if (len < 0) {
            return 0;
        }

        strncpy(gngga_buf, gngga_start_flag, len);

        if (minmea_parse_gga(&frame, gngga_buf)) {

            if (frame.latitude.value == 0 && frame.longitude.value == 0) {
                return 0;
            }
            else {
                gps->height = frame.height.value;
                gps->latitude = minmea_tocoord(&frame.latitude);
                gps->longitude = minmea_tocoord(&frame.longitude);
                cfg_set_device_longitude(gps->longitude);
                cfg_set_device_latitude(gps->latitude);
                cfg_set_device_height((double)(gps->height));
                LOG_DEBUG("gps :%" PRIu32 ", %" PRIu32 "\r\n", frame.latitude.value, frame.longitude.value);
                return 1;
            }
        }

    }
    return 0;
}

int gps_get_coordinates(void)
{
    int ret;
    char buffer[GPS_RX_BUFSIZE];
    memset(buffer,0,GPS_RX_BUFSIZE);

    gps_enable();
    delay_s(GPS_GET_COORDINATES_INTERVAL);
    gps_read(buffer, GPS_RX_BUFSIZE);
    ret = parse_gngga_msg(&gps, buffer);

    return ret;
}

int gps_get_time(void)
{
    int ret = 0;
    char buffer[GPS_RX_BUFSIZE];
    memset(buffer,0,GPS_RX_BUFSIZE);

    gps_enable();
    delay_ms(GPS_GET_TIME_INTERVAL);
    gps_read(buffer, GPS_RX_BUFSIZE);

    ret = parse_gnrmc_msg(buffer);

    return ret;
}

void *gps_handler(void *arg)
{
   /* ... */
   (void)arg;
   uint32_t gps_time = 0;
   gps_init();
   gy25_init();
   adjust_roll_angle();
   while(!gps_get_time())
   {
       delay_ms(10);
   }
    gps_time = rtt_get_counter();
    daq_spi_chan_set_fpga_utc(gps_time);

   while(!gps_get_coordinates())
   {
       delay_ms(10);
   }

    while (1) {
        adjust_course_angle();
        delay_s(GPS_INTERVAL);
        gy25_read_dip_angle();
        while (!gps_get_time()) {
            delay_ms(10);
        }

        gps_time = rtt_get_counter();
        daq_spi_chan_set_fpga_utc(gps_time);
    }

   return NULL;
}

kernel_pid_t gps_service_pid;
static char gps_service_thread_stack[THREAD_STACKSIZE_MAIN];
kernel_pid_t gps_service_init(void)
{
	gps_service_pid = thread_create(gps_service_thread_stack,
			sizeof(gps_service_thread_stack),
			GPS_THREAD_PRIORITY, THREAD_CREATE_STACKTEST, gps_handler, NULL,
			"gps_service");
	return gps_service_pid;

}
