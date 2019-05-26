#include "heart_beat.h"
#include "frame_encode.h"
#include "circular_task.h"
#include "comm.h"
#include "log.h"

#include "periph/rtt.h"
#include "board.h"
#include "daq.h"

void heart_beat_handler(void)
{
    uint16_t length = 0;
    uint8_t data[MAX_RSP_FRAME_LEN] = { 0 };
    length = frame_heart_beat_encode(data, DEVICEOK, rtt_get_counter());
    LOG_INFO("Try to send heartbeat to master.");
    msg_circular_packet_send(data, length, HEART_BEAT);
}

uint32_t is_overflow_happended(uint32_t interval)
{
    static uint32_t overflow = 0;

    if (overflow >= interval) {
        overflow = 0;
        return SD_TRUE;
    }
    else {
        overflow++;
        return SD_FALSE;
    }
}
static int g_wdt_flag = 1;

void update_wdt_flag(int flag)
{
    g_wdt_flag = flag;
    LOG_INFO("Update watchdog flag as %d", g_wdt_flag);
}

void set_fpga_utc_time(uint32_t timestamp)
{
   daq_fsmc_fpga_time_set(timestamp);
}

void heart_beat_service(void)
{
    if (g_wdt_flag) {
        IWDG_Feed();
    } else {
        LOG_WARN("Watchdog flag is %d, stop feed dog.", g_wdt_flag);
    }

    if (SD_TRUE == is_overflow_happended(200)) {
        heart_beat_handler();
        set_fpga_utc_time(rtt_get_counter());
    }
}
