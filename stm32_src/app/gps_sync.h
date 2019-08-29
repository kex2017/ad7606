#ifndef GPS_SYNC_H_
#define GPS_SYNC_H_

#include "minmea.h"
#include "kernel_types.h"

typedef struct gps_t
{
   float latitude;
   float longitude;
   int height;
}GPS;

#define GY25_FRAME_LEN (8)
#define GY25_FRAME_HEADER 0xAA
#define GY25_FRAME_ENDER 0x55

typedef struct gy25_t {
    float course_angle; //航向角
    float pitch_angle; //俯仰角
    float roll_angle;  //横滚角
} GY25;

GY25* get_gy25_dip_angle(void);
void gy25_read_dip_angle(void);
int update_rtc_time(int retry_count);

kernel_pid_t gps_service_init(void);
void init_device_irq(void);

#endif//GPS_SYNC_H_
