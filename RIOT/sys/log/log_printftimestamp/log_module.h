
#ifndef LOG_MODULE_H
#define LOG_MODULE_H

#include "log.h"

#ifdef __cplusplus
extern "C" {
#endif

int log_set_level(log_level_t level);
void log_write(log_level_t level, const char* fmt, ...);
void log_hex(log_level_t level, int numBytes, const char* hexData) ;

#define LOG_DUMP(pdata, len) log_hex(HEX_DUMP, (len), (pdata))

#ifdef __cplusplus
}
#endif
/**@}*/
#endif /* LOG_MODULE_H */
