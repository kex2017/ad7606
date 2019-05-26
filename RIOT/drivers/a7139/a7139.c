/**
 * @defgroup   drivers_a7139 A7139 Sub 1GHz Transceiver device driver
 * @ingroup    drivers_netdev
 * @brief      I2C Analog-to-Digital Converter device driver
 *
 *             This driver works with a7139 versions.
 * @{
 *
 * @file
 * @brief      Implementation of A7139  Sub 1GHz Transceiver  device driver
 * @author     xubingcan <xubingcan@klec.com.cn>
 */
#include "a7139.h"
#include "a7139_regs.h"

#include "a7139_dtype.h"
#include "a7139_internal.h"
#include "log.h"
#include "periph/gpio.h"


/************************************************************************
* 函数        :   void A7139_newInitConfig(void)
* 功能        :   A7139初始化
* 说明        :
************************************************************************/
void A7139_newInitConfig(void)
{
   A7139_pins_init();

   A7139_POR();
   LOG_INFO("A7139 POR Success!");

   while (1) {
      if (InitRF()) //init RF
      {
         entry_deep_sleep_mode();
         delay_1ms(2);
         wake_up_from_deep_sleep_mode();
      }
      else {
         break;
      }
   }
   LOG_INFO("A7139 Init Success!");
}

/************************************************************************
* 函数        :   void A7139_Reset(void)
* 功能        :   A7139复位
* 说明        :
************************************************************************/
void A7139_Reset(void)
{
   StrobeCMD(CMD_STBY);
   StrobeCMD(CMD_RF_RST);
}

/************************************************************************
* 函数        :   void A7139_Sleep(void)
* 功能        :   A7139睡眠模式
* 说明        :
************************************************************************/
void A7139_Sleep(void)
{
    StrobeCMD(CMD_STBY);
    StrobeCMD(CMD_SLEEP);
}


/************************************************************************
* 函数        :   void A7139_SENDPACKET (uint8_t *lpData, uint8_t LEN)
* 功能        :   A7139发送数据
* 说明        :
************************************************************************/
void A7139_SENDPACKET (uint8_t *lpData, uint8_t LEN)
{
   set_status(CMD_STBY);
   StrobeCMD(CMD_STBY);
   mynop();mynop();mynop();mynop();

   StrobeCMD(CMD_TFR);   //TX FIFO address pointer reset
   A7139_WriteFIFO(lpData, LEN);
   set_status(CMD_TX);
   StrobeCMD(CMD_TX);
   LOG_DEBUG("A7139_SENDPACKET ......end....\r\n");
 }




