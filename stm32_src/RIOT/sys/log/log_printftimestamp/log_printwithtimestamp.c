#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>

#include "log.h"

#define BUF_SIZE    (1024)
static char buf[BUF_SIZE];

log_level_t g_log_level = LOG_LEVEL;

#define cvt_nibble(a)   ((unsigned char) ((a) > 9 ? (a) + 'A' - 10 : (a) + '0'))

int hexdata_to_str(char *bcd_str, char ch)
{
   char hex;

   hex = (ch >> 4) & 0x0f;
   hex = cvt_nibble(hex);
   bcd_str[0] = hex;

   hex = ch & 0x0f;
   hex = cvt_nibble(hex);
   bcd_str[1] = hex;

   return 2;
}

int convertHexDataToStr(char *destHexStr, int destHexStrSize, int numBytes, const char* srcHexData)
{
   int i, offset = 0;
   char *buf = (char*)srcHexData;
   char *raw = destHexStr;

   if (destHexStrSize < numBytes * 3) {
      return 0;
   }

   for (i = 0; i < numBytes; i++) {
      offset += hexdata_to_str(raw + offset, buf[i]);
      raw[offset++] = ' ';
   }
   raw[offset] = '\0';

   return 1;
}

char* get_level_string(log_level_t level)
{
   switch (level) {
   case LOG_DEBUG:
      return "DEBUG";
   case LOG_WARNING:
      return "WARN";
   case LOG_ERROR:
      return "ERROR";
   case LOG_INFO:
      return "INFO";
   default:
      return "HEXDUMP";
   }
}

void _log(log_level_t level, const char *msg)
{
   time_t epoch_time;
   time(&epoch_time);
   struct tm *local_time = localtime(&epoch_time);
   printf("%04d-%02d-%02d %02d:%02d:%02d  %s: %s\r\n", local_time->tm_year - 100 + 2000, local_time->tm_mon + 1,
          local_time->tm_mday, local_time->tm_hour, local_time->tm_min, local_time->tm_sec, get_level_string(level),
          msg);

}

/**
 * @brief log_write overridden function
 *
 * This example function will only print the log's format string.
 * Use it where printf might be too heavy.
 *
 * @param[in] level (unused)
 * @param[in] format String that the function will print
 */
void log_write(log_level_t level, const char* fmt, ...)
{
   if (level > g_log_level) {
      return;
   }

   va_list ap;

   va_start(ap, fmt);

   if (fmt != NULL)
   vsnprintf(buf, sizeof(buf), fmt, ap);
   else
   buf[0] = '\0';

   _log(level, buf);
   va_end(ap);
}

void log_hex(log_level_t level, int numBytes, const char* hexData)
{
   if (level > g_log_level) {
      return;
   }
   int bufSize = numBytes * 3 + 2;

   if (bufSize > BUF_SIZE) {
      _log(level, "Buffer size is not enough to hold hex data");
      return;
   }
   if (0 == convertHexDataToStr(buf, BUF_SIZE, numBytes, hexData)) {
      _log(level, "Convert hex data to string error!");
      return;
   }

   _log(level, buf);
}

int log_set_level(log_level_t level)
{
   g_log_level = level;
   return 0;
}
