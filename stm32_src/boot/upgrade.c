#include "upgrade.h"
#include "periph/flashpage.h"
#include "hashes/md5.h"

#define FLASH_PAGE_SIZE    ((uint16_t)0x800)

#define FLASH_BASE_ADDR 0x08000000
#define UPGRADE_CFG_BASE_ADDR 0x08078000

#define ACTIVE_APP_BASE_ADDR  0x08010000
#define UPGRADE_APP_BASE_ADDR 0x08030000
#define UPGRADE_APP_MAX_LENGTH 0x20000

#define MD5_LEN 16

#define UPGRADE_FLAG_SET	0x21212100
#define UPGRADE_FLAG_CLEAN	0x12121200

typedef struct _upgrade_cfg {
	uint32_t flag;
	uint32_t app_len;
	uint8_t app_md5[MD5_LEN];
}upgrade_cfg_t;

typedef union {
	uint8_t cfg_buf[FLASH_PAGE_SIZE];
	upgrade_cfg_t cfg;
}upgrade_cfg_buf_t;

static upgrade_cfg_buf_t g_upgrade_cfg;

uint8_t buf[FLASH_PAGE_SIZE] = { 0 };
static void flash_upgrade_app_to_active_addr(void)
{

	uint32_t app_len = g_upgrade_cfg.cfg.app_len;

	int pages = 0;

	if(0 == app_len % FLASH_PAGE_SIZE)
	{
		pages = app_len / FLASH_PAGE_SIZE;
	}
	else
	{
		pages = (app_len / FLASH_PAGE_SIZE) + 1;
	}
	int i = 0;
	printf("FLASHPAGE_NUMOF=%d",FLASHPAGE_NUMOF);
	for(i = 0; i< pages; i++)
	{
		memset(buf, 0x0, FLASH_PAGE_SIZE );
		memcpy(buf, (uint32_t*)(UPGRADE_APP_BASE_ADDR+i*FLASH_PAGE_SIZE), FLASH_PAGE_SIZE );
//		printf("FLASHPAGE_NUMOF=%d",FLASHPAGE_NUMOF);
		flashpage_write((ACTIVE_APP_BASE_ADDR - FLASH_BASE_ADDR)/FLASH_PAGE_SIZE + i,buf);
	}
}

/*******************************************************************************
** 函数名称:    load_upgrade_cfg
** 函数功能:    加载升级配置
** 形式参数:    无
** 返回参数:    无
*******************************************************************************/
static void load_upgrade_cfg(void)
{
	int page = 0;
	page = (UPGRADE_CFG_BASE_ADDR - FLASH_BASE_ADDR) /  FLASH_PAGE_SIZE;
	flashpage_read(page,g_upgrade_cfg.cfg_buf);
}


/*******************************************************************************
** 函数名称:    save_upgrade_cfg
** 函数功能:    保存升级配置
** 形式参数:    无
** 返回参数:    无
*******************************************************************************/
static void save_upgrade_cfg(void)
{
    int page = 0;
    page = (UPGRADE_CFG_BASE_ADDR - FLASH_BASE_ADDR) /  FLASH_PAGE_SIZE;
    flashpage_write(page, g_upgrade_cfg.cfg_buf);
}


int is_md5sum_right(void)
{
    uint8_t digest[MD5_LEN] = { 0 };
    md5(digest, (void*)UPGRADE_APP_BASE_ADDR, g_upgrade_cfg.cfg.app_len);

    int i = 0;
    printf("[BOOTLOADER]: calc upgrade app md5: ");
	for (i = 0; i < MD5_LEN; i++) {
		printf("%x ",digest[i]);
	}
	printf("\r\n");


	printf("[BOOTLOADER]: upgrade cfg md5: ");
	for (i = 0; i < MD5_LEN; i++) {
		printf("%x ", g_upgrade_cfg.cfg.app_md5[i]);
	}
	printf("\r\n");


    if(0 != memcmp(digest,g_upgrade_cfg.cfg.app_md5,MD5_LEN))
    {
    	return 0;
    }

    return 1;
}


int do_upgrade(void)
{
//	g_upgrade_cfg.cfg.flag = UPGRADE_FLAG_CLEAN;
//	g_upgrade_cfg.cfg.app_len = 0x01;
//
//	save_upgrade_cfg();

	load_upgrade_cfg();

	printf("[BOOTLOADER]: upgrade flag check ...\r\n");
	if(g_upgrade_cfg.cfg.flag != UPGRADE_FLAG_SET)
	{
		return 0;
	}

	if(g_upgrade_cfg.cfg.app_len > UPGRADE_APP_MAX_LENGTH)
	{
		printf("[BOOTLOADER]: [OVERFLOW] (%ld)bytes\r\n\r\n",g_upgrade_cfg.cfg.app_len - UPGRADE_APP_MAX_LENGTH);
		return 0;
	}
	printf("[BOOTLOADER]: [NORMAL] (%ld)bytes\r\n\r\n",g_upgrade_cfg.cfg.app_len);


	printf("[BOOTLOADER]: upgrade app md5 check ...\r\n");
	if(!is_md5sum_right())
	{
		printf("[BOOTLOADER]: [MISMATCH]\r\n\r\n");
		g_upgrade_cfg.cfg.flag = UPGRADE_FLAG_CLEAN;
		save_upgrade_cfg();
		return 0;
	}
	printf("[BOOTLOADER]: [MATCH]\r\n\r\n");


	printf("[BOOTLOADER]: starting flash upgrade app\r\n\r\n");

	flash_upgrade_app_to_active_addr();
	g_upgrade_cfg.cfg.flag = UPGRADE_FLAG_CLEAN;
	save_upgrade_cfg();
	printf("[BOOTLOADER]: flash done!\r\n");
	return 1;

}
