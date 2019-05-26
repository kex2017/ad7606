/**
 * @ingroup     drivers_a7139
 * @{
 *
 * @file
 * @brief       Implementation of driver internal functions
 *
 * @author     xubingcan <xubingcan@klec.com.cn>
 * @}
 */
#include "a7139_internal.h"
#include "a7139_regs.h"
#include "a7139_dtype.h"
#include "a7139_params.h"
#include "periph_conf.h"
#include "periph/gpio.h"

#include "log.h"
#include "board.h"
#include "xtimer.h"
#include "timex.h"

void  mynop(void)
{
   xtimer_usleep(1);
}

uint8_t  ID_Tab[4]={0x34,0x75,0x34,0x75};   //ID code

uint8_t  fb;
uint8_t  fb_ok=0;


#define INTERVAL (1U * US_PER_SEC)

/* TODO Check with Fuxing
  //6-CSC    ------->>  58-P4.2
  //7-SCK    ------->>  59-P4.3
  //8-SDIO   ------->>  60-P4.4
  //4-GPIO2  ------->>  57-P4.1
  //5-GPIO1  ------->>  56-P4.0
 */

#define GIO2 (GPIO_PIN(PORT_D, 4))
#define GIO1 (GPIO_PIN(PORT_D, 5))
#define SOFT_SPI_SDIO         (GPIO_PIN(PORT_D, 3))
#define SOFT_SPI_SCK          (GPIO_PIN(PORT_D, 2))
#define SOFT_SPI_SCS          (GPIO_PIN(PORT_D, 1))

#define  SPI_SCS_H          (gpio_set(SOFT_SPI_SCS))
#define  SPI_SCS_L          (gpio_clear(SOFT_SPI_SCS))
#define  SPI_SCK_H          (gpio_set(SOFT_SPI_SCK))
#define  SPI_SCK_L          (gpio_clear(SOFT_SPI_SCK))
#define  SIP_DATA_OUT       (gpio_init(SOFT_SPI_SDIO, GPIO_OUT))
#define  SIP_DATA_IN        (gpio_init(SOFT_SPI_SDIO, GPIO_IN))
#define  SPI_DATA_H         (gpio_set(SOFT_SPI_SDIO))
#define  SPI_DATA_L         (gpio_clear(SOFT_SPI_SDIO))
#define  SPI_DATA_HL        (gpio_read(SOFT_SPI_SDIO))

/************************************************************************
* 函数        :   void A7139_pins_init(void)
* 功能        :   A7139管脚初始化
* 说明        :
*
************************************************************************/

void A7139_pins_init(void)
{
   /* initialize spi clock pin */
   gpio_init(SOFT_SPI_SCK, GPIO_OUT);
   gpio_clear(SOFT_SPI_SCK);

   /* initialize spi sdio pins */
   gpio_init(SOFT_SPI_SDIO, GPIO_OUT);
   gpio_clear(SOFT_SPI_SDIO);

   /* Initialize spi CS pin */
   gpio_init(SOFT_SPI_SCS, GPIO_OUT);
   gpio_set(SOFT_SPI_SCS);
}

/*************************************************************************************************************************
* 函数        :   void A7139_StrobeCmd(uint8_t cmd)
* 功能        :   A7139发送Strobe命令
* 说明        :
*************************************************************************************************************************/

void StrobeCMD(uint8_t cmd)
{
   uint8_t i;
   SIP_DATA_OUT;
   SPI_SCS_L;
   for (i = 0; i < 8; i++) {
      if (cmd & 0x80) {
         SPI_DATA_H;
      }
      else {
         SPI_DATA_L;
      }
      mynop();
      SPI_SCK_H;
      mynop();
      SPI_SCK_L;
      cmd <<= 1;
   }
   SPI_SCS_H;
}

/*************************************************************************************************************************
* 函数        :   void A7139_WriteByte(uint8_t data)
* 功能        :   A7139写一字节
* 参数        :   data:需要写入的数据
* 返回        :   无
* 依赖        :   底层宏定义
* 作者        :   cp1300@139.com
* 时间        :   2015-07-19
* 最后修改  :   2015-07-19
* 说明        :   不带片选,最底层写入1B
*************************************************************************************************************************/
void ByteSend(uint8_t data)
{
   uint8_t i;
   SIP_DATA_OUT;

   for (i = 0; i < 8; i++) {

      if (data & 0x80) {
         SPI_DATA_H;
      }
      else {
         SPI_DATA_L;
      }

      mynop();
      SPI_SCK_H;
      mynop();
      SPI_SCK_L;
      data <<= 1;
   }
}

/*************************************************************************************************************************
* 函数        :   uint8_t A7139_ReadByte(void)
* 功能        :   A7139读取一字节
* 参数        :   无
* 返回        :   读取的数据
* 说明        :   不带片选,最底层读取1B
*************************************************************************************************************************/
uint8_t ByteRead(void)
{
   uint8_t i, tmp = 0;

   SIP_DATA_IN;

   for (i = 0; i < 8; i++) {
      if (SPI_DATA_HL) {
         tmp = (tmp << 1) | 0x01;
      }
      else {
         tmp = tmp << 1;
      }
      mynop();
      SPI_SCK_H;
      mynop();
      SPI_SCK_L;
   }
   return tmp;
}

/************************************************************************
*函数        :   void A7139_WriteReg(uint8_t address, uint16_t dataWord)
* 功能        :   写入控制寄存器
* 参数        :   RegAddr:寄存器地址,data:要写入的值
************************************************************************/
void A7139_WriteReg(uint8_t address, uint16_t dataWord)
{
   uint8_t i;
   SIP_DATA_OUT;
   SPI_SCS_L;
   address |= CMD_Reg_W;
   for (i = 0; i < 8; i++) {
      if (address & 0x80) {
         SPI_DATA_H;
      }
      else {
         SPI_DATA_L;
      }

      mynop();
      SPI_SCK_H;
      mynop();
      mynop();
      SPI_SCK_L;
      mynop();
      address <<= 1;
   }
   mynop();

   //send data word
   for (i = 0; i < 16; i++) {
      if (dataWord & 0x8000) {
         SPI_DATA_H;
      }
      else {
         SPI_DATA_L;
      }

      mynop();
      SPI_SCK_H;
      mynop();
      mynop();
      SPI_SCK_L;
      mynop();
      dataWord <<= 1;
   }
   SPI_SCS_H;
}

/************************************************************************
* 函数        :  uint16_t  A7139_ReadReg(uint8_t address)
* 功能        :   读取控制寄存器
* 参数        :   RegAddr:寄存器地址
* 返回        :   寄存器值
* 依赖        :   底层宏定义
************************************************************************/
uint16_t  A7139_ReadReg(uint8_t address)
{
   uint8_t i;
   uint16_t tmp = 0;
   SIP_DATA_OUT;

   SPI_SCS_L;
   address |= CMD_Reg_R;
   for (i = 0; i < 8; i++) {
      if (address & 0x80) {
         SPI_DATA_H;
      }
      else {
         SPI_DATA_L;
      }

      mynop();
      SPI_SCK_H;
      mynop();
      mynop();
      SPI_SCK_L;
      mynop();
      address <<= 1;
   }
   mynop();
   SIP_DATA_IN;
   mynop();

   SPI_DATA_H;
   for (i = 0; i < 16; i++) {
      if (SPI_DATA_HL) {
         tmp = (tmp << 1) | 0x01;
      }
      else {
         tmp = tmp << 1;
      }

      mynop();
      SPI_SCK_H;
      mynop();
      mynop();
      SPI_SCK_L;
      mynop();
   }
   SPI_SCS_H;
   return tmp;
}

/************************************************************************
**  A7139_WritePageA
************************************************************************/
void A7139_WritePageA(uint8_t address, uint16_t dataWord)
{
    uint16_t tmp;

    tmp = address;
    tmp = ((tmp << 12) | A7139Config[CRYSTAL_REG]);
    A7139_WriteReg(CRYSTAL_REG, tmp);
    A7139_WriteReg(PAGEA_REG, dataWord);
}

/************************************************************************
**  A7139_ReadPageA
************************************************************************/
uint16_t A7139_ReadPageA(uint8_t address)
{
    uint16_t tmp;

    tmp = address;
    tmp = ((tmp << 12) | A7139Config[CRYSTAL_REG]);
    A7139_WriteReg(CRYSTAL_REG, tmp);
    tmp = A7139_ReadReg(PAGEA_REG);
    return tmp;
}
/************************************************************************
**  A7139_WritePageB
************************************************************************/
void A7139_WritePageB(uint8_t address, uint16_t dataWord)
{
    uint16_t tmp;

    tmp = address;
    tmp = ((tmp << 7) | A7139Config[CRYSTAL_REG]);
    A7139_WriteReg(CRYSTAL_REG, tmp);
    A7139_WriteReg(PAGEB_REG, dataWord);
}

/************************************************************************
**  A7139_ReadPageB
************************************************************************/
uint16_t A7139_ReadPageB(uint8_t address)
{
    uint16_t tmp;

    tmp = address;
    tmp = ((tmp << 7) | A7139Config[CRYSTAL_REG]);
    A7139_WriteReg(CRYSTAL_REG, tmp);
    tmp = A7139_ReadReg(PAGEB_REG);
    return tmp;
}

/*********************************************************************
** A7139_POR
*********************************************************************/
void A7139_POR(void)
{
   //power on only
    delay_1ms(10);          //for regulator settling time (power on only)

    StrobeCMD(CMD_RF_RST);    //reset A7139 chip
    while(A7139_WriteID())    //check SPI
    {
       StrobeCMD(CMD_RF_RST);  //reset A7139 chip
    }
    A7139_WritePageA(PM_PAGEA, A7139Config_PageA[PM_PAGEA] | 0x1000);   //STS=1
    delay_1ms(2);

    entry_deep_sleep_mode();     //deep sleep
    delay_1ms(2);
    wake_up_from_deep_sleep_mode(); //wake up

    StrobeCMD(CMD_RF_RST);    //reset A7139 chip
    while(A7139_WriteID())    //check SPI
    {
       StrobeCMD(CMD_RF_RST);  //reset A7139 chip
    }
    A7139_WritePageA(PM_PAGEA, A7139Config_PageA[PM_PAGEA] | 0x1000);   //STS=1
    delay_1ms(2);
}

uint8_t InitRF(void)
{
   LOG_DEBUG("A7139 RF init");
   delay_1ms(1);
   StrobeCMD(CMD_RF_RST);  //reset A7139 chip
   delay_1ms(1);
   if (A7139_Config()) {
      LOG_WARN("InitRF Conf Failed!");
      return 1;         //config A7139 chip
   }

   delay_1ms(1);             //for crystal stabilized
   if (A7139_WriteID()) {
      LOG_WARN("InitRF WriteID Failed!");
      return 1;        //write ID code
   }
   if (A7139_Cal()) {
      LOG_WARN("InitRF Cal Failed!");
      return 1;            //IF and VCO calibration
   }
   return 0;
}

/*********************************************************************
** A7139_Config
*********************************************************************/
uint8_t A7139_Config(void)
{
   uint8_t i;
   uint16_t tmp;

   for (i = 0; i < 8; i++)
      A7139_WriteReg(i, A7139Config[i]);

   for (i = 10; i < 16; i++) {
      if ((i == 14) && (fb_ok == 1))
         A7139_WriteReg(i, A7139Config[i] | (1 << 4));        //MIFS=1(Manual)
      else
         A7139_WriteReg(i, A7139Config[i]);
   }

   for (i = 0; i < 16; i++)
      A7139_WritePageA(i, A7139Config_PageA[i]);

   for (i = 0; i < 5; i++)
      A7139_WritePageB(i, A7139Config_PageB[i]);

   //for check
   tmp = A7139_ReadReg(SYSTEMCLOCK_REG);
   if (tmp != A7139Config[SYSTEMCLOCK_REG]) {
      return 1;
   }

   return 0;
}

/************************************************************************
**  WriteID
************************************************************************/
uint8_t A7139_WriteID(void)
{
   uint8_t i;
   uint8_t d1, d2, d3, d4;

   SPI_SCS_L;
   ByteSend(CMD_ID_W);
   for (i = 0; i < 4; i++) {
      ByteSend(ID_Tab[i]);
   }
   SPI_SCS_H;

   delay_1ms(1);
   SPI_SCS_L;
   ByteSend(CMD_ID_R);
   d1 = ByteRead();
   d2 = ByteRead();
   d3 = ByteRead();
   d4 = ByteRead();
   SPI_SCS_H;

   if ((d1 != ID_Tab[0]) || (d2 != ID_Tab[1]) || (d3 != ID_Tab[2]) || (d4 != ID_Tab[3])) {
      return 1;
   }
   return 0;
}

/*********************************************************************
** A7139_Cal
*********************************************************************/
uint8_t A7139_Cal(void)
{
   uint8_t i;
   uint8_t fb_old, fcd, fbcf;        //IF Filter
   uint8_t vb, vbcf;          //VCO Current
   uint8_t vcb, vccf;        //VCO Band
   uint16_t tmp;
   uint8_t fb_fail;

   StrobeCMD(CMD_STBY);

   //IF calibration procedure @STB state
   if (fb_ok == 1) {
      tmp = (A7139Config[CALIBRATION_REG] & 0xFFE0);
      tmp = tmp | fb | (1 << 4);
      A7139_WriteReg(CALIBRATION_REG, tmp);
   }
   else {
      fb_fail = 0;

      for (i = 0; i < 3; i++) {
         A7139_WriteReg(MODE_REG, A7139Config[MODE_REG] | 0x0802);       //IF Filter & VCO Current Calibration
         do {
            tmp = A7139_ReadReg(MODE_REG);
         } while (tmp & 0x0802);

         //for check(IF Filter)
         tmp = A7139_ReadReg(CALIBRATION_REG);
         fb = tmp & 0x0F;
         fcd = (tmp >> 11) & 0x1F;
         fbcf = (tmp >> 4) & 0x01;

         if ((fb < 4) || (fb > 8))
            fb_fail = 1;
         else {
            if (i == 0)
               fb_old = fb;
            else {
               if (fb != fb_old) fb_fail = 1;
            }
         }

         if ((fbcf) || (fb_fail)) {
            return 1;
         }
      }
   }

   //for check(VCO Current)
   tmp = A7139_ReadPageA(VCB_PAGEA);
   vcb = tmp & 0x0F;
   vccf = (tmp >> 4) & 0x01;
   if (vccf) {
      return 1;
   }

   //RSSI Calibration procedure @STB state
   A7139_WriteReg(ADC_REG, 0x4C00);                                    //set ADC average=64
   A7139_WriteReg(MODE_REG, A7139Config[MODE_REG] | 0x1000);           //RSSI Calibration
   do {
      tmp = A7139_ReadReg(MODE_REG);
   } while (tmp & 0x1000);
   A7139_WriteReg(ADC_REG, A7139Config[ADC_REG]);

   //VCO calibration procedure @STB state
   for (i = 0; i < 3; i++) {
      A7139_WriteReg(PLL1_REG, Freq_Cal_Tab[i * 2]);
      A7139_WriteReg(PLL2_REG, Freq_Cal_Tab[i * 2 + 1]);
      A7139_WriteReg(MODE_REG, A7139Config[MODE_REG] | 0x0004);   //VCO Band Calibration
      do {
         tmp = A7139_ReadReg(MODE_REG);
      } while (tmp & 0x0004);

      //for check(VCO Band)
      tmp = A7139_ReadReg(CALIBRATION_REG);
      vb = (tmp >> 5) & 0x07;
      vbcf = (tmp >> 8) & 0x01;
      if (vbcf) {
         return 1;
      }
   }

   fb_ok = 1;
   (void)vcb;
   (void)vb;
   (void)fcd;
   return 0;
}

/*************************************************************************************************************************
* 函数        :   uint32_t A7139_ReadID(void)
* 功能        :   读取A7139 ID
* 参数        :   无
* 返回        :   ID值
* 说明        :   读取ID
*************************************************************************************************************************/
uint32_t A7139_ReadID(void)
{
   uint32_t data;
   uint8_t i;

   SPI_SCS_L;
   ByteSend(CMD_ID_R);         //读ID命令
   SIP_DATA_IN; //输入
   data = 0;
   for (i = 0; i < 4; i++) {
      data <<= 8;
      data |= ByteRead();
   }
   SPI_SCS_H;

   return data;
}
/*************************************************************************************************************************
* 函数        :   void A7139_ReadFIFO(uint8_t *pData, uint8_t DataLen)
* 功能        :   A7139读取FIFO
* 说明        :
*************************************************************************************************************************/
void A7139_ReadFIFO(uint8_t *pData, uint8_t *DataLen)
{
    uint8_t i;

    SPI_SCS_L;
    ByteSend(CMD_FIFO_R);
    *DataLen = ByteRead();
    //循环读取FIFO
    for(i = 0;i < *DataLen;i ++)
    {
        pData[i] = ByteRead();
    }
    SPI_SCS_H;
}

/*************************************************************************************************************************
* 函数        :   void A7139_WriteFIFO(u8 *pData, u8 DataLen)
* 功能        :   A7139写FIFO
* 说明        :
*************************************************************************************************************************/
void A7139_WriteFIFO(uint8_t *pData, uint8_t DataLen)
{
    uint8_t i;

    SPI_SCS_L;
    SIP_DATA_OUT;
    ByteSend(CMD_FIFO_W);
    ByteSend(DataLen);
    //循环写入FIFO
    for(i = 0;i < DataLen;i ++)
    {
        ByteSend(pData[i]);
    }
    SPI_SCS_H;
}

/*********************************************************************
** RC Oscillator Calibration
*********************************************************************/
void RCOSC_Cal(void)
{
    uint16_t tmp;

    A7139_WritePageA(WOR2_PAGEA, A7139Config_PageA[WOR2_PAGEA] | 0x0010);       //enable RC OSC

    while(1)
    {
        A7139_WritePageA(WCAL_PAGEA, A7139Config_PageA[WCAL_PAGEA] | 0x0001);   //set ENCAL=1 to start RC OSC CAL
        do{
            tmp = A7139_ReadPageA(WCAL_PAGEA);
        }while(tmp & 0x0001);

        tmp = (A7139_ReadPageA(WCAL_PAGEA) & 0x03FF);       //read NUMLH[8:0]
        tmp >>= 1;

        if((tmp > 183) && (tmp < 205))      //NUMLH[8:0]=194+-10 (PF8M=6.4M)
        //if((tmp > 232) && (tmp < 254))    //NUMLH[8:0]=243+-10 (PF8M=8M)
        {
            break;
        }
    }
}

/*********************************************************************
** WOR_enable_by_preamble
*********************************************************************/
void WOR_enable_by_preamble(void)
{
    StrobeCMD(CMD_STBY);
    RCOSC_Cal();        //RC Oscillator Calibration

    A7139_WritePageA(GIO_PAGEA, (A7139Config_PageA[GIO_PAGEA] & 0xF000) | 0x004D);  //GIO1=PMDO, GIO2=WTR

    //Real WOR Active Period = (WOR_AC[5:0]+1) x 244us ¡V X'TAL and Regulator Settling Time
    //Note : Be aware that X¡¦tal settling time requirement includes initial tolerance,
    //       temperature drift, aging and crystal loading.
    A7139_WritePageA(WOR1_PAGEA, 0x8005);   //setup WOR Sleep time and Rx time

    A7139_WritePageA(WOR2_PAGEA, A7139Config_PageA[WOR2_PAGEA] | 0x0030);   //enable RC OSC & WOR by preamble
    A7139_WriteReg(MODE_REG, A7139Config[MODE_REG] | 0x0200);               //WORE=1 to enable WOR function

    while(gpio_read(GIO1) == 0);     //Stay in WOR mode until receiving preamble code(preamble detect ok)
}

/*********************************************************************
** WOR_enable_by_sync
*********************************************************************/
void WOR_enable_by_sync(void)
{
    StrobeCMD(CMD_STBY);
    RCOSC_Cal();        //RC Oscillator Calibration

    A7139_WritePageA(GIO_PAGEA, (A7139Config_PageA[GIO_PAGEA] & 0xF000) | 0x0045);  //GIO1=FSYNC, GIO2=WTR

    //Real WOR Active Period = (WOR_AC[5:0]+1) x 244us ¡V X'TAL and Regulator Settling Time
    //Note : Be aware that X¡¦tal settling time requirement includes initial tolerance,
    //       temperature drift, aging and crystal loading.
    A7139_WritePageA(WOR1_PAGEA, 0x8005);   //setup WOR Sleep time and Rx time

    A7139_WritePageA(WOR2_PAGEA, A7139Config_PageA[WOR2_PAGEA] | 0x0010);   //enable RC OSC & WOR by sync
    A7139_WriteReg(MODE_REG, A7139Config[MODE_REG] | 0x0200);               //WORE=1 to enable WOR function

    while(gpio_read(GIO1)==0);     //Stay in WOR mode until receiving ID code(ID detect ok)

    /*
                        ____    ____    ____    ____    ____
    TX WTR       ______|    |__|    |__|    |__|    |__|    |__
                            ______   _______
    WOR WTR      __________|      |_|       |__________________
                                 _       ___
    WOR FSYNC    _______________| |_____|   |__________________
                                ¡õ ¡õ ¡õ       ¡õ
                                ¡õ ¡õ ¡õ       receive completed
                                ¡õ ¡õ entry RX mode
                                ¡õ disable WOR
                                sync ok
    */

    A7139_WriteReg(MODE_REG, A7139Config[MODE_REG] & ~0x0200);              //WORE=0 to disable WOR function
    StrobeCMD(CMD_STBY);
    StrobeCMD(CMD_RX);
    while(gpio_read(GIO2)==0);     //wait RX ready
    while(gpio_read(GIO2)==1);     //wait receive completed
}

/*********************************************************************
** WOR_enable_by_carrier
*********************************************************************/
void WOR_enable_by_carrier(void)
{
    StrobeCMD(CMD_STBY);
    RCOSC_Cal();        //RC Oscillator Calibration

    A7139_WritePageA(GIO_PAGEA, (A7139Config_PageA[GIO_PAGEA] & 0xF000) | 0x0049);  //GIO1=CD, GIO2=WTR

    //Real WOR Active Period = (WOR_AC[5:0]+1) x 244us ¡V X'TAL and Regulator Settling Time
    //Note : Be aware that X¡¦tal settling time requirement includes initial tolerance,
    //       temperature drift, aging and crystal loading.
    A7139_WritePageA(WOR1_PAGEA, 0x8005);   //setup WOR Sleep time and Rx time

    A7139_WritePageA(WOR2_PAGEA, A7139Config_PageA[WOR2_PAGEA] | 0x0410);   //enable RC OSC & WOR by carrier
    A7139_WritePageA(RFI_PAGEA, A7139Config_PageA[RFI_PAGEA] & ~0x6000);    //select RSSI Carrier Detect
    A7139_WriteReg(ADC_REG, A7139Config[ADC_REG] | 0x8069);                 //ARSSI=1, RTH=105(-100dBm)
    A7139_WriteReg(MODE_REG, A7139Config[MODE_REG] | 0x0200);               //WORE=1 to enable WOR function

    while(gpio_read(GIO1)==0);     //Stay in WOR mode until carrier signal strength is greater than the value set by RTH[7:0](carrier detect ok)
}


/*********************************************************************
** entry_deep_sleep_mode
*********************************************************************/
void entry_deep_sleep_mode(void)
{
    StrobeCMD(CMD_RF_RST);              //RF reset
    A7139_WriteReg(PIN_REG, A7139Config[PIN_REG] | 0x0800);             //SCMDS=1
    A7139_WritePageA(PM_PAGEA, A7139Config_PageA[PM_PAGEA] | 0x1010);   //STS=1, QDS=1
    StrobeCMD(CMD_SLEEP);               //entry sleep mode
    delay_1us(600);                      //delay 600us for VDD_A shutdown, C load=0.1uF
    StrobeCMD(CMD_DEEP_SLEEP);          //entry deep sleep mode
    delay_1us(200);                      //delay 200us for VDD_D shutdown, C load=0.1uF
}

/*********************************************************************
** wake_up_from_deep_sleep_mode
*********************************************************************/
void wake_up_from_deep_sleep_mode(void)
{
    StrobeCMD(CMD_STBY);    //wake up
    delay_1ms(2);            //delay 2ms for VDD_D stabilized
    //InitRF();
}

void delay_1us(unsigned int xus)   //误差 0us  12M
{
   xtimer_usleep(xus);
}

void delay_1ms(unsigned int xms)
{
    xtimer_ticks32_t last_wakeup = xtimer_now();
    xtimer_periodic_wakeup(&last_wakeup, MS_PER_SEC*xms);
}


uint8_t     Status;
uint8_t get_status(void) {
   return Status;
}
void set_status(uint8_t status)
{
   Status = status;
}
/*************************************************************************************************************************
 * 函数        :   void A7139_Interrupt(void)
 * 功能        :   接收中断
 * 参数        :
 * 说明        :
 *************************************************************************************************************************/
uint16_t                  num=1;
uint8_t                  recver_LEN;
char recv7139[512];
void A7139_Interrupt(void)
{
   printf("InterruptTimes %d in Status %d\n", num++, Status);
   if (Status == CMD_TX) {
      StrobeCMD(CMD_STBY);
      Status = CMD_RX;
      StrobeCMD(CMD_RX);
   }
   else if (Status == CMD_RX) {
      printf("Start Recv...\n");
      StrobeCMD(CMD_RFR);       //RX FIFO address pointer reset
      A7139_ReadFIFO((uint8_t*)recv7139, &recver_LEN);

      printf("Recv Length %d\n", recver_LEN);
      for (uint8_t cc = 0; cc < recver_LEN; cc++) {
         printf("%2X ", recv7139[cc]);

      }
      printf("\nDone\n");

//    A7139_StrobeCmd(CMD_SLEEP);
      Status = CMD_RX;
      StrobeCMD(CMD_RX);
   }
   else {
      StrobeCMD(CMD_STBY);
      Status = CMD_RX;
      StrobeCMD(CMD_RX);
   }
}

//收发模式记录，用于中断处理发送或接收模式
static bool isSendMode = FALSE; //默认为接收模式 static
void A7139_SendMode(uint8_t x)
{
   isSendMode=x;
}

uint8_t A7139_isSendMode(void)
{
   return isSendMode == TRUE;
}

void My_EXTI4_IRQHandler(void *arg)         //这里为：EXTI4 (外部中断号的4在这里实现）
{
   (void)arg;
   A7139_Interrupt();
//   LOG_INFO("HELLO WORLD!\n");
   ;
}

void A7139_Recv_Int_Init(void)
{
   StrobeCMD(CMD_STBY);
   Status = CMD_RX;
   StrobeCMD(CMD_RX);
   LOG_INFO("Enter RX State");
   gpio_init(GIO1, GPIO_IN_PU);//gio1
   gpio_init_int(GIO2, GPIO_IN_PU, GPIO_FALLING, My_EXTI4_IRQHandler, NULL);
}
