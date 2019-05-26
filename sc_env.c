#include "sc_env.h"

#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cfg.h"
#include "periph/rtt.h"
#include "time.h"
#include "gps_sync.h"
#include "temperature.h"

void usage_help(void)
{
    printf("\nOptions:\n"
    "    -h, --help\n"
    "        print this help message\n"
    "    -i <id>, --id=<id>\n"
    "        Set Device Number\n"
    "    -b <num>, --baudrate=<num>\n"
    "        Set Device uart2 baudrate\n"
    "    -W <id>, --longitude=<longitude>\n"
    "        Set Device longitude\n"
    "    -E <id>, --latitude=<latitude>\n"
    "        Set Device latitude\n"
    "    -H <id>, --height=<height>\n"
    "        Set Device height\n"
    "    -v <version>, --addr=<addr>\n"
    "        Set Hardware Version\n"
    "    -t <yyyymmddhhmiss>, --time=<yyyymmddhhmiss>\n"
    "        Set device timestamp\n"
	"    -s , --sram=\n"
	"        Test ext sram R/W\n"
	"    -T , --temp=\n"
	"        Test get device temperature R/W\n"
    );
}

void cfg_set_device_time(char *time)
{
	char time_str[14] = {0};
	char tmp[5] = {0};
    struct tm t;
    static time_t tim;

	if (strlen(time) != 14) {
		printf("Input timestamp length < 14(yyyymmddhhmmss)!\r\n");
		return;
	}

	memcpy(time_str, time, 14);
	memset(tmp, 0, 5);
	memcpy(tmp, time_str, 4);
	t.tm_year = atoi(tmp) - 1900;

	memset(tmp, 0, 5);
	memcpy(tmp, time_str + 4, 2);
	t.tm_mon = atoi(tmp) - 1;

	memset(tmp, 0, 5);
	memcpy(tmp, time_str + 6, 2);
	t.tm_mday = atoi(tmp);

	memset(tmp, 0, 5);
	memcpy(tmp, time_str + 8, 2);
	t.tm_hour = atoi(tmp) - 8;

	memset(tmp, 0, 5);
	memcpy(tmp, time_str + 10, 2);
	t.tm_min = atoi(tmp);

	memset(tmp, 0, 5);
	memcpy(tmp, time_str + 12, 2);
	t.tm_sec = atoi(tmp);

	tim = mktime(&t);
	rtt_set_counter((uint32_t)tim);
//	init_device_irq();
}

static uint16_t array1[1024] __attribute__((section(".big_data")));
static uint16_t array2[1024] __attribute__((section(".big_data")));

void display_array(uint16_t *array)
{
	int i = 0;

	for (i = 0; i < 1024; i++) {
		if((array[i] != array1[i]) || (array[i] != array2[i])) {
			printf("%04x %04x %04x %04x\r\n", i, array[i], array1[i], array2[i]);
		}
	}
}
uint16_t array0[1024] = {0};
void test_ext_sram(void)
{
	int i = 0;

	for (i = 0; i < 1024; i++) {
		array0[i] = i;
	}

	memcpy(array1, array0, sizeof(array0));
	memcpy(array2, array1, sizeof(array0));

	display_array(array0);
	if (!memcmp(array0, array1, sizeof(array0))) {
		printf("Ext SRAM R/W array1 OK!\r\n");
	} else {
		printf("Ext SRAM R/W array1 NOK!\r\n");
	}

	if (!memcmp(array0, array2, sizeof(array0))) {
		printf(" Ext SRAM R/W array2 OK!\r\n");
	} else {
		printf(" Ext SRAM R/W array2 NOK!\r\n");
	}
}

char version[32] = { 0 };
int setenv_command(int argc, char **argv)
{
    int opt = 0;
    uint16_t id = 0;
    uint32_t baudrate = 0;
    double longitude = 0.0, latitude = 0.0, height = 0.0;
    static const struct option long_opts[] = {
        { "help", no_argument, NULL, 'h' },
        { "id", required_argument, NULL, 'i' },
        { "baudrate", required_argument, NULL, 'b' },
        { "longitude", required_argument, NULL, 'W' },
        { "latitude", required_argument, NULL, 'E' },
        { "height", required_argument, NULL, 'H' },
        { "version", required_argument, NULL, 'v' },
		{ "time", required_argument, NULL, 't' },
		{ "sram", no_argument, NULL, 's' },
		{ "Temperature", no_argument, NULL, 'T' },
        {NULL,0,NULL,0},
    };

    if(argc < 2)
    {
        usage_help();
        return 1;
    }

    while((opt = getopt_long(argc, argv, "hi:b:W:E:H:v:t:sT", long_opts, NULL)) != -1)
    {
        switch (opt) {
        case 'h':
            usage_help();
            break;
        case 'i':
            id = (uint16_t)atoi(optarg);
            if (id == 0) {
                printf("Input error ID\r\n");
                return 1;
            }
            cfg_set_device_id(id);
            break;
        case 'b':
            baudrate = (uint32_t)atoi(optarg);
            if (baudrate == 0) {
                printf("Input error BaudRate\r\n");
                return 1;
            }
            cfg_set_device_uart2_bd(baudrate);
            break;
        case 'W':
            longitude = (double)atof(optarg);
            if (longitude == 0) {
                printf("Input error Longitude\r\n");
                return 1;
            }
            cfg_set_device_longitude(longitude);
            break;
        case 'E':
            latitude = (double)atof(optarg);
            if (latitude == 0) {
                printf("Input error Latitude\r\n");
                return 1;
            }
            cfg_set_device_latitude(latitude);
            break;
        case 'H':
            height = (double)atof(optarg);
            if (height == 0) {
                printf("Input error Height\r\n");
                return 1;
            }
            cfg_set_device_height(height);
            break;
        case 'v':
            if (8 < strlen(optarg))
            {
                printf("Input Version too long :%d\r\n",strlen(optarg));
                return 1;
            }
            cfg_set_device_version(optarg);
            break;
        case 't':
			cfg_set_device_time(optarg);
        	break;
        case 's':
        	test_ext_sram();
        	break;
        case 'T':
        	sc_get_device_temperature();
        	break;
        default:
            if(!optarg )
            {
                printf("optarg is NULL!\r\n");
                break;
            }
            printf("unkown command %s!\r\n",optarg);
            break;
        }
    }
    optind = 1;

    return 0;
}

int printenv_command(int argc, char **argv)
{
    (void)argc;
    (void)argv;
    print_device_cfg();
    return 0;
}

int saveenv_command(int argc, char **argv)
{
    (void)argc;
    (void)argv;
    update_device_cfg();
    return 0;
}
