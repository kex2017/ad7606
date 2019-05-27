#include "gps_sync.h"

#include <string.h>
#include "x_delay.h"
#include "periph_cpu.h"
#include "gps.h"
#include "periph/rtt.h"
#include "log.h"
#include "thread.h"
#include "env_cfg.h"

#define GPS_INTERVAL  50U
#define GPS_GET_TIME_INTERVAL 200U
#define GPS_GET_COORDINATES_INTERVAL 2U
#define GPS_RX_BUFSIZE    (512)

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

   gps_init();

   while(!gps_get_time())
   {
       delay_ms(10);
   }


   while(!gps_get_coordinates())
   {
       delay_ms(10);
   }


   while (1)
   {
		delay_s(GPS_INTERVAL);
		while (!gps_get_time()) {
			delay_ms(10);
		}
	}


   return NULL;
}

kernel_pid_t gps_service_pid;

#define GPS_THREAD_PRIORITY	(8)
static char gps_service_thread_stack[THREAD_STACKSIZE_MAIN];
kernel_pid_t gps_service_init(void)
{
	gps_service_pid = thread_create(gps_service_thread_stack,
			sizeof(gps_service_thread_stack),
			GPS_THREAD_PRIORITY, THREAD_CREATE_STACKTEST, gps_handler, NULL,
			"gps_service");
	return gps_service_pid;

}
