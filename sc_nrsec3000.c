#include "sc_nrsec3000.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "log.h"
#include "security_device.h"


void nrsec3000_usage_help(void)
{
    printf("\nCommands:\n"
    "    test NRSEC3000 chip test\n"
    );
}

int nrsec30000_command(int argc, char **argv)
{
   if (argc < 2) {
      nrsec3000_usage_help();
      return 1;
   }

   if (strncmp(argv[1], "test", 5) == 0) {
      nrsec3000_test();
   }
   else {
      nrsec3000_usage_help();
   }

   return 0;
}
