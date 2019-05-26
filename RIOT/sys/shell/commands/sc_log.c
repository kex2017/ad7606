#include "log.h"
#include <stdio.h>
#include <string.h>

static int _log_usage(void)
{
    puts("usage: log <level>");
    puts("\t Set the log level to given level value");
    puts("levels:");
    puts("\tNONE");
    puts("\tERROR");
    puts("\tWARN");
    puts("\tINFO");
    puts("\tDEBUG");
    puts("\tALL");
    return 0;
}

int _log_handler(int argc, char **argv)
{
   if (argc < 2) {
      _log_usage();
       return 1;
   }
   else if (strncmp(argv[1], "NONE", 5) == 0) {
       return log_set_level(LOG_NONE);
   }
   else if (strncmp(argv[1], "ERROR", 5) == 0) {
       return log_set_level(LOG_ERROR);
   }
   else if (strncmp(argv[1], "WARN", 5) == 0) {
       return log_set_level(LOG_WARNING);
   }
   else if (strncmp(argv[1], "INFO", 5) == 0) {
       return log_set_level(LOG_INFO);
   }
   else if (strncmp(argv[1], "DEBUG", 5) == 0) {
       return log_set_level(LOG_DEBUG);
   }
   else if (strncmp(argv[1], "ALL", 5) == 0) {
       printf("Set Log Level to ALL\n");
       return log_set_level(LOG_ALL);
   }
   else {
       printf("unknown command: %s\n", argv[1]);
       return 1;
   }
   return 0;
}
