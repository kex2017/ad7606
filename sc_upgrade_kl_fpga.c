/*
 * sc_update_FPGA_ops.c
 *
 *  Created on: Jun 22, 2018
 *      Author: chenzy
 */

#include "sc_upgrade_kl_fpga.h"

#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>

#include "x_delay.h"
#include "board.h"
#include "soft_spi.h"
#include "fatfs/ff.h"
#include "log.h"
#include "hashes/md5.h"
#include "upgrade_from_sd_card.h"
#include "daq.h"
#include "ext_fpga_img_upgrade.h"
#include "mass_fpga_handler.h"

void flash_usage(void)
{
    printf("usage: FPGA flash commands:\r\n");
    printf("\t -h, --help\r\n");
    printf("\t\t print this help message\r\n");
    printf("\t -i, --id\r\n");
    printf("\t\t read FPGA flash id\r\n");
    printf("\t -r [addr], --read [addr]\r\n");
    printf("\t\t read FPGA flash 256 bytes at addr\r\n");
    printf("\t -w [addr], --write [addr]\r\n");
    printf("\t\t write 20 bytes[0~9] to FPGA flash addr\r\n");
    printf("\t -e, --erase\r\n");
    printf("\t\t erase FPGA flash chip\r\n");
    printf("\t -u <filename>, --upgrade\r\n");
    printf("\t\t upgrade ext FPGA\r\n");
    printf("\t -f <len>, --flash <len>\r\n");
    printf("\t\t calc FPGA flash md5sum form 0 to [len]\r\n");
    printf("\t -s <filename>, --sdcard <filename>\r\n");
    printf("\t\t calc file md5sum from sd card\r\n");
    printf("\t -a , --append\r\n");
    printf("\t\t append content to sd card file\r\n");
	printf("\t -x <filename>, --extract\r\n");
	printf("\t\t uncompress FPGA file in sdcard\r\n");
	printf("\t -n , --scandir\r\n");
	printf("\t\t scan SD dir file\r\n");
	printf("\t -d <filename> , --delfile\r\n");
	printf("\t\t delete SD file\r\n");
}

int upgrade_kl_fpga_command(int argc, char **argv)
{
    int opt = 0, len;
    char data[20] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19};
    uint32_t addr = 0;
    uint8_t md5sum[33] = {0};
    char *filename = NULL;

    static const struct option long_opts[] = {
            { "help", no_argument, NULL, 'h' },
            { "id", no_argument, NULL, 'i' },
            { "read", required_argument, NULL, 'r' },
            { "write", required_argument, NULL, 'w' },
            { "erase", no_argument, NULL, 'e' },
            { "upgrade", required_argument, NULL, 'u' },
            { "flash", required_argument, NULL, 'f' },
            { "sdcard", required_argument, NULL, 's' },
            { "append", no_argument, NULL, 'a' },
			{ "extract", required_argument, NULL, 'x' },
			{ "scandir", no_argument, NULL, 'n' },
			{ "delfile", required_argument, NULL, 'd' },
            { NULL, 0, NULL, 0 },
    };

    if (argc < 2 || strlen(argv[1]) < 2) {
        flash_usage();
        return 1;
    }

    while((opt = getopt_long(argc, argv, "hir:w:eu:f:s:ax:nd:", long_opts, NULL)) != -1) {
        switch (opt) {
        case 'h':
            flash_usage();
            break;
        case 'i':
        	read_kl_fpga_flash_id();
            break;
        case 'r':
            addr = optarg ? atoi(optarg) : 0;
            printf("Try to read 256 data from addr:%ld\r\n", addr);
            read_fpga_flash_bytes(addr, 256);
            break;
        case 'w':
            addr = optarg ? atoi(optarg) : 0;
            printf("Try to write 20 data to addr:%ld\r\n", addr);
            write_fpga_flash_bytes(addr, (uint8_t *)data, 20);
            break;
        case 'e':
        	printf("Try to erase fpga flash with whole chip\r\n");
        	erase_fpga_flash();
            break;
        case 'u':
        	filename = optarg;
        	printf("Try to upgrade fpga\r\n");
        	upgrade_ext_fpga(filename);
            break;
        case 'n':
			scan_sd_card_files();
			break;
		case 'd':
			filename = optarg;
			delete_sd_card_file(filename);
			break;
        case 's':
            filename = optarg;
            sd_read_file((char*)filename);
            break;
        case 'f':
            len = atoi(optarg);
            calc_ext_fpga_flash_md5sum(len, md5sum);
            break;
		case 'x':
			//压缩的文件为FPGA3_P，解压文件名手动输入
			filename = optarg;
			uncompress_file((char*)"FPGA3_P", filename);
			break;
        case 'a':
            sd_append_test();
            break;
        default:
            /* once option without valid value, opt will be set as '?' and optarg is NULL,
             * so will need to handle here to avoid segmentation fault.*/
            if (!optarg) {
                printf("optarg is NULL\r\n");
                break;
            }
            printf("unknown command %s!\r\n", optarg);
            break;
        }
    }
    optind = 1;
    return 0;
}

