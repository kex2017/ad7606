#include "ext_fpga_img_upgrade.h"

#include <stdio.h>
#include <string.h>
#include "xtimer.h"
#include "hashes/md5.h"
#include "upgrade_from_sd_card.h"
#include "log.h"
#include "fatfs/ff.h"
#include "mass_fpga_handler.h"
#include "daq.h"

const char *uncompressed_file = "fpga_uc";

/*核心板 FPGA */
static uint8_t g_buf[256] __attribute__((section(".big_data")));

void soft_wip(void)
{
	KL_SPI_PIN cs = KL_SPI_CS;
   uint8_t v = 1;
   uint8_t rdsr_ops = 0x05;//读状态寄存器

//   puts("start soft_wip");
   while(1 == (v & 0x01)) {
	   kl_soft_spi_transfer_bytes(SOFT_SPI_DEV(0), cs, 1, &rdsr_ops, NULL, 1);
	   kl_soft_spi_transfer_bytes(SOFT_SPI_DEV(0), cs, 0, NULL, &v, 1);
//       printf("value:%02x\r\n", v);
   }
//   puts("end soft_wip");
}

//读取flash的ID
void read_kl_fpga_flash_id(void)
{
	int mode = SOFT_SPI_MODE_3;
    int clk = SOFT_SPI_CLK_100KHZ;
    KL_SPI_PIN cs = KL_SPI_CS;
	uint8_t fac_id, dev_id_H, dev_id_L;
	uint8_t outb[] = {0x9F};

	kl_soft_spi_acquire(SOFT_SPI_DEV(0), cs, mode, clk);

	kl_soft_spi_transfer_bytes(SOFT_SPI_DEV(0), cs, 1, &outb, NULL, 1);
	kl_soft_spi_transfer_bytes(SOFT_SPI_DEV(0), cs, 1, NULL, &fac_id, 1);
	kl_soft_spi_transfer_bytes(SOFT_SPI_DEV(0), cs, 1, NULL, &dev_id_H, 1);
	kl_soft_spi_transfer_bytes(SOFT_SPI_DEV(0), cs, 0, NULL, &dev_id_L, 1);

	kl_soft_spi_release(SOFT_SPI_DEV(0));

	LOG_INFO("Get flash fac_id:0x%02x", fac_id);
	LOG_INFO("Get flash dev_id:0x%02x%02x", dev_id_H, dev_id_L);
}

//擦除整个芯片
void erase_fpga_flash(void)
{
    int mode = SOFT_SPI_MODE_3;
    int clk = SOFT_SPI_CLK_100KHZ;
    KL_SPI_PIN cs = KL_SPI_CS;
    uint8_t wen_ops = 0x06;		//wen
    uint8_t erase_ops = 0xc7;	//erase all chip

    kl_soft_spi_acquire(SOFT_SPI_DEV(0), cs, mode, clk);

    kl_soft_spi_transfer_bytes(SOFT_SPI_DEV(0), cs, 0, &wen_ops, NULL, 1);
    LOG_INFO("start erase flash...");
    kl_soft_spi_transfer_bytes(SOFT_SPI_DEV(0), cs, 0, &erase_ops, NULL, 1);
    soft_wip();					//wip==rdsr
    LOG_INFO("erase ok!\r\n");

    kl_soft_spi_release(SOFT_SPI_DEV(0));
}

//读出Flash中指定地址指定长度的数据到g_buf中
void read_fpga_flash_bytes(uint32_t addr, uint32_t len)
{
    int mode = SOFT_SPI_MODE_3;
    int clk = SOFT_SPI_CLK_100KHZ;
    KL_SPI_PIN cs = KL_SPI_CS;
    uint8_t read_ops = 0x03;
    uint8_t read_addr[] = { (addr & 0xffffff) >> 16, (addr & 0xffff) >> 8, (addr & 0xff) }; //read addr

    kl_soft_spi_acquire(SOFT_SPI_DEV(0), cs, mode, clk);
    LOG_DEBUG("start read flash addr:%d, len:%d", addr, len);

    kl_soft_spi_transfer_bytes(SOFT_SPI_DEV(0), cs, 1, &read_ops, NULL, 1);
    kl_soft_spi_transfer_bytes(SOFT_SPI_DEV(0), cs, 1, &read_addr, NULL, 3);
    kl_soft_spi_transfer_bytes(SOFT_SPI_DEV(0), cs, 0, NULL, &g_buf, len);

    kl_soft_spi_release(SOFT_SPI_DEV(0));
    LOG_DEBUG("Get read data at addr:%ld, %d, %d, %d, %d, %d, %d.\r\n", addr, g_buf[0], g_buf[1], g_buf[2], g_buf[3], g_buf[4], g_buf[5]);
}

//向Flash中指定地址写入指定长度的指定数据
void write_fpga_flash_bytes(uint32_t addr, uint8_t* buff, int len)
{
	KL_SPI_PIN cs = KL_SPI_CS;
    int mode = SOFT_SPI_MODE_3;
    int clk = SOFT_SPI_CLK_100KHZ;
    uint8_t write_op = 0x02;
    uint8_t wen_ops = 0x06; //写使能
    uint8_t write_addr[] = { (addr & 0xffffff) >> 16, (addr & 0xffff) >> 8, (addr & 0xff) }; //read addr

    kl_soft_spi_acquire(SOFT_SPI_DEV(0), cs, mode, clk);

    kl_soft_spi_transfer_bytes(SOFT_SPI_DEV(0), cs, 0, &wen_ops, NULL, 1);
    soft_wip();
    kl_soft_spi_transfer_bytes(SOFT_SPI_DEV(0), cs, 1, &write_op, NULL, 1);
    kl_soft_spi_transfer_bytes(SOFT_SPI_DEV(0), cs, 1, &write_addr, NULL, 3);
    kl_soft_spi_transfer_bytes(SOFT_SPI_DEV(0), cs, 0, buff, NULL, len);

    kl_soft_spi_release(SOFT_SPI_DEV(0));
}

//读取flash中指定长度内容写入sd卡中
void bak_kl_fpga_img(int32_t total_len, const char *input_file)
{
    uint32_t i;
    int32_t len;
    FRESULT result;
    FIL file;
    uint32_t bw;

	result = f_open(&file, input_file, FA_CREATE_ALWAYS | FA_WRITE);
	if (result != FR_OK) {
		LOG_ERROR("Don't Find File: %s", input_file);
		return;
	}

    for (i = 0; total_len > 0; i++) {
        len = total_len > 256 ? 256 : total_len;
        LOG_DEBUG("Try to read %ld data from flash", len);
        memset(g_buf, 0, sizeof(g_buf));
        read_fpga_flash_bytes(i * 256, len);//读出flash中指定地址处256B长度的数据存放到g_buf中
        result = f_write(&file, (void*)g_buf, 256, (UINT*)&bw);//每次向文件中追加写入256Byte(一页）
		if (result == FR_OK) {
			LOG_DEBUG("%s File Write Success", input_file);
		} else {
			LOG_ERROR("File Write Failed Error(%d)", result);
		}
        total_len -= 256;
    }

    f_close(&file);
}

uint32_t read_file_data_to_buf(FIL *file, uint32_t *length)
{
    uint32_t result;

    //读取文件到g_buf数组中
    result = f_read(file, &g_buf, sizeof(g_buf), (UINT*)length);
    if (result || *length == 0) {
        *length = 0;
        result = 0;
    }
    else
        result = 1;
    return result;
}

//将sd卡中fpga img文件写入到flash中
int download_ext_fpga_img(const char *input_file)
{
    uint32_t i, length = 0;
    int32_t sum = 0;
	uint32_t result;
	FIL file;
    uint8_t md5_buf[16] = { 0 };
    md5_ctx_t ctx = { 0 };
    memset(&ctx, 0, sizeof(md5_ctx_t));
    md5_init(&ctx);

	result = f_open(&file, input_file, FA_OPEN_EXISTING | FA_READ);
	if (result != FR_OK) {
		LOG_ERROR("Not find input_file : %s, result: %d ", input_file, result);
		return 1;
	}

	LOG_INFO("start download ext fpga img...");
    for (i = 0; ; i++) {
    	result = f_lseek(&file, i * 256);
		if (result != FR_OK) {
			LOG_ERROR("Seek location of input_file : %s, result: %d ", input_file, result);
			f_close(&file);
			return 1;
		}
    	//读取文件到g_buf数组中
		memset(g_buf, 0, sizeof(g_buf));
		if(read_file_data_to_buf(&file, &length) == 0)
		{
			break;
		}
		md5_update(&ctx, g_buf, length);//
		write_fpga_flash_bytes(i * 256, g_buf, length);
		sum += length;
    }
    if (!sum) {
    	LOG_ERROR("read %d bytes", sum);
    	f_close(&file);
        return 1;
    }
    md5_final(&ctx, md5_buf);
	for (i = 0; i < 16; i++) {
		printf("%02x", md5_buf[i]);
	}
    LOG_INFO("upgrade fpga img ok!Total write data length:%ld", sum);
    f_close(&file);
    return 0;
//    memset(md5sum, 0, 33);
//    file_calc_md5sum(input_file, md5sum);
//    LOG_INFO("File:%s md5sum:%s", input_file, md5sum);
//
//    memset(md5sum, 0, 33);
//    flash_calc_md5sum(sum, md5sum);
//    LOG_INFO("Flash md5sum:%s", md5sum);
}

//计算sd卡中指定文件的md5
uint8_t calc_ext_fpga_img_md5sum(const char *filename, uint8_t *md5sum)
{
    FRESULT result;
    FIL file;
    uint32_t bw;
    uint32_t i;
    uint8_t md5_buf[16] = {0};
    md5_ctx_t ctx = {0};

    result = f_open(&file, filename, FA_OPEN_EXISTING | FA_READ);
    if (result != FR_OK) {
        LOG_ERROR("Don't Find File : %s", filename);
        //unmount
        f_mount(0, "", 1);
        return result;
    }
    memset(&ctx, 0, sizeof(md5_ctx_t));
    md5_init(&ctx);
    LOG_INFO("start calculate sdcard file md5sum...");
    for (i = 0; ; i++) {
        result = f_read(&file, &g_buf, sizeof(g_buf), (UINT*)&bw);
        if (result || bw == 0) {
//            printf("Read file Error (%d:%ld)\r\n", result, bw);
            break;
        }
        else {
            LOG_DEBUG("read file len:%ld", g_buf, bw);
            md5_update(&ctx, g_buf, bw);
            memset(g_buf, 0, sizeof(g_buf));
        }
    }
    md5_final(&ctx, md5_buf);
    for (i = 0; i< 16; i++) {
        printf("%02x", md5_buf[i]);
        sprintf((char*)(md5sum + i * 2), "%02x", md5_buf[i]);
    }
    printf("\r\n");
    LOG_INFO("calculate md5sum done.");

    f_close(&file);

    return 0;
}

//计算flash中指定长度数据的md5值(8M对应长度为8388608)
void calc_ext_fpga_flash_md5sum(int32_t total_len, uint8_t *md5sum)
{
    uint32_t i;
    int32_t len;
    uint8_t md5_buf[16] = { 0 };
    md5_ctx_t ctx = { 0 };

    memset(&ctx, 0, sizeof(md5_ctx_t));
    md5_init(&ctx);

    LOG_INFO("start calculate flash file md5sum...");
    for (i = 0; total_len > 0; i++) {
        len = total_len > 256 ? 256 : total_len;
//        LOG_INFO("Try to read %ld data from flash", len);
        //从flash中读取256字节数据到g_buf中
        read_fpga_flash_bytes(i * 256, len);
        md5_update(&ctx, g_buf, len);
        total_len -= 256;
        memset(g_buf, 0, sizeof(g_buf));
    }

    md5_final(&ctx, md5_buf);
    for (i = 0; i< 16; i++) {
        printf("%02x", md5_buf[i]);
        sprintf((char*)(md5sum + i * 2), "%02x", md5_buf[i]);
    }
    printf("\r\n");
    LOG_INFO("calculate md5sum done.");
}

//解压文件
int uncompress_file(char *compressed_file, const char *uncompressed_file)
{
	LOG_INFO("start uncompress file %s...", compressed_file);

	if(heatshrink(compressed_file, (char*)uncompressed_file))
	{
		return 1;
	}

	LOG_INFO("uncompress file %s to %s ok!", compressed_file, uncompressed_file);
	return 0;
}

int check_md5(uint8_t* sd_md5sum, uint8_t* flash_md5sum,int len)
{
	return strncmp((const char*)sd_md5sum, (const char*)flash_md5sum, len);
}

int update_ext_fpga_flash(uint8_t *sd_md5sum)
{
	uint8_t flash_md5sum[33] = { 0 };
	uint8_t upgrade_retry_times = 3;  //升级失败后重新升级的次数
	do {
		erase_fpga_flash();							 		 //3.擦除flash
		if (download_ext_fpga_img(uncompressed_file) && (0 == upgrade_retry_times-1)){		 //4.下载fpga程序
			f_mount(0, "", 1);
			return UP_LOAD_FPGA_IMG_ERR;
		}
		calc_ext_fpga_flash_md5sum(FPGA_FLASH_SIZE, flash_md5sum);//5.计算升级后flash中数据的md5
		if (check_md5(sd_md5sum, flash_md5sum, sizeof(flash_md5sum))) {
			LOG_ERROR("upgrade fpga error!try to upgrade fpga again...");
			upgrade_retry_times--;
			if (0 == upgrade_retry_times) {
				LOG_ERROR("upgrade fpga error!");
				f_mount(0, "", 1);
				return UPGRADE_ERR;
			}
		} else {
			LOG_INFO("upgrade fpga success!");
			upgrade_retry_times = 0;
			daq_fsmc_fpga_power_reset();								//6.重启fpga
		}
	} while (upgrade_retry_times);
	return 0;
}

//升级fpga接口函数，传入SD卡中压缩的FPGA img文件名和指定的解压文件名
int upgrade_ext_fpga(char *compressed_file)
{
	FRESULT result;
	FATFS fs;
	DIR dir_info;
	uint8_t sd_md5sum[33] = { 0 };
	uint8_t errcode = 0;
	result = f_mount(&fs, "", 1);
	if (result != FR_OK) {
		LOG_ERROR("FileSystem Mounted Failed (%d)", result);
		return UP_FS_MOUNT_ERR;
	}
	result = f_opendir(&dir_info, "0:/");
	if (result != FR_OK) {
		LOG_ERROR("Root Directory is Open Error (%d)", result);
		f_mount(0, "", 1);
		return UP_FS_OPEN_DIR_ERR;
	}

	if (uncompress_file(compressed_file, uncompressed_file)){//1.解压SD中压缩文件
		LOG_ERROR("uncompress file error");
		f_mount(0, "", 1);
		return UP_UNCOMPRESS_ERR;
	}
	calc_ext_fpga_img_md5sum(uncompressed_file, sd_md5sum);  //2.计算解压后文件的md5
	//TODO:do-while封装接口(已加)
	errcode = update_ext_fpga_flash(sd_md5sum);
	if(errcode)
	{
		return errcode;
	}
	f_mount(0, "", 1);
	return UP_OK;
}

