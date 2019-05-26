#ifndef HEART_BEAT_H_
#define HEART_BEAT_H_

#include "frame_common.h"

void update_wdt_flag(int flag);
void heart_beat_service(void);
void heart_beat_handler(void);

#endif
