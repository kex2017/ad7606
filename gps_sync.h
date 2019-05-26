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

int update_rtc_time(int retry_count);

kernel_pid_t gps_service_init(void);
#endif//GPS_SYNC_H_
