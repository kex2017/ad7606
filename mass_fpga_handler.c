/*
 * mass_fpga_handler.c
 *
 *  Created on: May 28, 2018
 *      Author: chenzy
 */

#include <stdio.h>
#include <string.h>
#include "log.h"
#include "fatfs/ff.h"

#include "heatshrink_encoder.h"
#include "heatshrink_decoder.h"
#include "heatshrink_config.h"

#include "upgrade_from_sd_card.h"

//static heatshrink_encoder hse;
static heatshrink_decoder hsd __attribute__((section(".big_data")));

#define OUTPUT_SIZE (2048)
static uint8_t out_buf[OUTPUT_SIZE] __attribute__((section(".big_data")));;
int decode_sink_handler(FIL *dst, uint8_t *comp_data, int read_len)
{
    static long total_len = 0;
    int sunk_len = 0;
    size_t writen_len;
    int result;
    size_t input_size, output_size;

    HSD_poll_res pres;

    total_len += read_len;
//    printf("Current decompressed len %ld\r\n", total_len);

    do {
        if (read_len > 0) {
            result = heatshrink_decoder_sink(&hsd, comp_data + sunk_len, read_len - sunk_len, &input_size);
            sunk_len += input_size;

//            printf("result:%d, read_len:%d, input_size:%d\r\n", result, read_len, input_size);
        }

        do {
            pres = heatshrink_decoder_poll(&hsd, out_buf, OUTPUT_SIZE, &output_size);
//            writen_len = write(dst, out_buf, output_size);
            result = f_write(dst, out_buf, output_size, (UINT*)&writen_len);
            if (result != FR_OK || writen_len != output_size) {
                printf("Write error result:%d,  expected:%d != actual:%d\r\n", result, writen_len, output_size);
            }
        } while (HSDR_POLL_MORE == pres);

        if (read_len == 0 && output_size == 0) {
            heatshrink_decoder_finish(&hsd);
            return 1; //finished
        }
    } while (sunk_len < read_len);

    return 0;
}

int decode_compressed_file(char *compressed_file, char *uncompressed_file)
{
    FIL src, dst;
    FRESULT result;
    uint8_t in_buf[256];
    int read_len;

    result = f_open(&src, compressed_file, FA_OPEN_EXISTING | FA_READ);
    if (result != FR_OK) {
        LOG_ERROR("Don't Find input File:%s, result:%d", compressed_file, result);
        return 1;
    }
    else {
        LOG_DEBUG("Open input File OK.");
    }

    result = f_open(&dst, uncompressed_file, FA_CREATE_ALWAYS | FA_WRITE);
    if (result != FR_OK) {
        LOG_ERROR("Don't Find output File: %s, result: %d", uncompressed_file, result);
        return 1;
    }
    else {
        LOG_DEBUG("Open output File OK.");
    }

    for (int i = 0; ; i++) {
        result = f_read(&src, (void*)in_buf, sizeof(in_buf), (UINT*)&read_len);
//        read_len = read(src_fd, in_buf, sizeof(in_buf));
        if (result == FR_OK && read_len == 0) {
            heatshrink_decoder_finish(&hsd);
        }
        if (decode_sink_handler(&dst, in_buf, read_len)) {
            printf("decode_sink_handler done\r\n");
            break;
        }
    }

    f_close(&src);
    f_close(&dst);
    return 0;
}

int heatshrink(char *compressed_file, char *uncompressed_file)
{
    heatshrink_decoder_reset(&hsd);
    if(decode_compressed_file(compressed_file, uncompressed_file))
    {
    	return 1;
    }
    return 0;
}
