/*
 * sc_device_cfg_test.c
 *
 *  Created on: Mar 19, 2018
 *      Author: chenzy
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>

#include "sc_device_cfg_test.h"
#include "frame_common.h"
#include "upgrade_from_flash.h"
#include "env_cfg.h"


void env_cfg_usage(void)
{
	printf("\r\nusage: device env cfg commands:\r\n");
    printf("\t -h, --help\r\n");
    printf("\t\t print this help message\r\n");
    printf("\t -s, --show\r\n");
    printf("\t\t show device configuration\r\n");
    printf("\t -i, --id=<device_id>\r\n");
    printf("\t\t set new device ID\r\n");
    printf("\t -v, --version=<version>\r\n");
	printf("\t\t set new device version\r\n");
    printf("\t -I, --Interval=<interval>\r\n");
	printf("\t\t set device data interval\r\n");
    printf("\t -t, --threshold=<threshold>\r\n");
	printf("\t\t set device all channel threshold\r\n");
    printf("\t -r, --reboot\r\n");
	printf("\t\t reboot system\r\n");
}

void test_set_device_id(uint16_t device_id)
{
	printf("Try to set device_id = %d.\r\n", device_id);
	if(cfg_get_device_id()!= device_id)
	{
		cfg_set_device_id(device_id);
		update_device_cfg();
	}
}

void test_set_device_data_interval(uint32_t interval)
{
	printf("Try to set data interval = %ld.\r\n", interval);
	cfg_set_device_data_interval(interval);
	update_device_cfg();
}

void test_reboot(void)
{
	printf("Try to reboot system now!\r\n");
	soft_reset();
}

int test_set_device_cfg(int argc, char **argv)
{
	int opt = 0;
	uint16_t device_id = 0;
	uint32_t interval = 0;

	static const struct option long_opts[] = {
			{ "help", no_argument, NULL, 'h' },
			{ "show", no_argument, NULL, 's' },
			{ "id", required_argument, NULL, 'i' },
			{ "version", required_argument, NULL, 'v' },
			{ "Interval", required_argument, NULL, 'I' },
			{ "threshold", required_argument, NULL, 't' },
			{ "reboot", no_argument, NULL, 'r' },
			{ NULL, 0, NULL, 0 },
	};

    if (argc < 2 || strlen(argv[1]) < 2) {
    	env_cfg_usage();
        return 1;
    }

    while ((opt = getopt_long(argc, argv, "hsi:v:I:r", long_opts, NULL))!= -1) {
    	switch (opt) {
    	case 'h':
    		env_cfg_usage();
			break;
    	case 's':
    		display_device_cfg();
    		break;
    	case 'i':
    		device_id = (uint16_t)atoi(optarg);
			test_set_device_id(device_id);
    		break;
    	case 'v':
			cfg_set_device_version(optarg);
			update_device_cfg();
			break;
    	case 'I':
    		interval = (uint32_t)atoi(optarg);
			test_set_device_data_interval(interval);
    		break;
    	case 'r':
    		test_reboot();
    		break;
    	default:
			if (!optarg) {
				printf("optarg is NULL\r\n");
				break;
			}
			printf("unknown command %s!\r\n", optarg);
			break;
    	}
    }
    return 0;
}
