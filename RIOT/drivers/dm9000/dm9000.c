
#include "dm9000.h"
#include "board.h"
#include "dm9000_registers.h"

#include "net/ethernet.h"
#include "net/netdev/eth.h"

/*Debug flags*/
#define ENABLE_DEBUG    (1)
#include "debug.h"
#define ENABLE_TRACE    (1)

#if ENABLE_TRACE
#define TRACE(...) DEBUG(__VA_ARGS__)
#else
#define TRACE(...)
#endif

#define FifoPointCheck
#define Point_Error_Reset

/**
 * Davicom DM9000A uP NIC fast Ethernet driver.
 */
#define   NET_BASE_ADDR        DM9000_BASE_ADDR
#define   NET_REG_ADDR        (*((volatile uint16_t *) NET_BASE_ADDR))
#define   NET_REG_DATA        (*((volatile uint16_t *) (NET_BASE_ADDR + 8)))

/* DM9000A 接收函数设置宏 */
//#define Rx_Int_enable
#define Max_Int_Count         1
#define Max_Ethernet_Lenth    1536
#define Broadcast_Jump
#define Max_Broadcast_Lenth      500

/* DM9000A 传送函数设置宏 */
#define Max_Send_Pack         2

/*Global Variables*/
uint8_t SendPackOk = 0;
uint8_t s_FSMC_Init_Ok = 0; /* 用于指示FSMC是否初始化 */
static uint8_t DEF_MAC_ADDR[ETH_ADDR_LEN] = { 0x00, 0x60, 0x6e, 0x90, 0x01, 0xae };

void dm9k_udelay(uint16_t time)
{
   uint16_t i, k;

   for (i = 0; i < time; i++) {
      for (k = 0; k < 80; k++)
         ;
   }
   while (time--)
      ;
}

/*******************************************************************************
 *  函数名: ior
 *  参  数: reg ：寄存器地址
 *  返  回: 无
 *  功  能: 读出寄存器的值
 */
uint8_t ior(uint8_t reg)
{
   NET_REG_ADDR = reg;
   return (NET_REG_DATA);
}

/*******************************************************************************
 *  函数名: iow
 *  参  数: reg ：寄存器地址
 *        writedata : 写入的数据
 *  返  回: 无
 *  功  能: 写DM9000AE寄存器的值
 */
void iow(uint8_t reg, uint8_t writedata)
{
   NET_REG_ADDR = reg;
   NET_REG_DATA = writedata;
}

static void dm9k_get_mac(uint8_t *mac)
{
   mac[0] = DEF_MAC_ADDR[0];
   mac[1] = DEF_MAC_ADDR[1];
   mac[2] = DEF_MAC_ADDR[2];
   mac[3] = DEF_MAC_ADDR[3];
   mac[4] = DEF_MAC_ADDR[4];
   mac[5] = DEF_MAC_ADDR[5];
}

static void dm9k_set_mac(uint8_t* macadd)
{
   DEF_MAC_ADDR[0] = macadd[0];
   DEF_MAC_ADDR[1] = macadd[1];
   DEF_MAC_ADDR[2] = macadd[2];
   DEF_MAC_ADDR[3] = macadd[3];
   DEF_MAC_ADDR[4] = macadd[4];
   DEF_MAC_ADDR[5] = macadd[5];
}

/*******************************************************************************
 *  函数名: dm9k_hash_table
 *  参  数: 无
 *  返  回: 无
 *  功  能: 设置 DM9000A MAC 、 广播 、 多播 寄存器
 */
void dm9k_hash_table(void)
{
   uint8_t i;

   /* 设置 网卡 MAC 位置，来自於 MyHardware */
   for (i = 0; i < 6; i++)
      iow(DM9000_REG_PAR + i, DEF_MAC_ADDR[i]);

   for (i = 0; i < 8; i++) /* 清除 网卡多播设置 */
      iow(DM9000_REG_MAR + i, 0x00);
   iow(DM9000_REG_MAR + 7, 0x80); /* 速设置 广播包 设置 */
}

/*******************************************************************************
 *  函数名: dm9k_reset
 *  参  数: 无
 *  返  回: 无
 *  功  能: 对DM9000AE进行软件复位
 */
void dm9k_reset(void)
{
   iow(DM9000_REG_NCR, DM9000_REG_RESET); /* 对 DM9000A 进行软件重置 */
   dm9k_udelay(10); /* delay 10us */
   iow(DM9000_REG_NCR, DM9000_REG_RESET); /* 对 DM9000A 进行软件重置 */
   dm9k_udelay(10); /* delay 10us */

   /* 基本记存器相关设置 */
   iow(DM9000_REG_IMR, DM9000_IMR_OFF); /* 开启内存自环模式 */
   iow(DM9000_REG_TCR2, DM9000_TCR2_SET); /* 设置 LED 显示模式1:全双工亮，半双工灭 */

   /* 清除多余资讯 */
   iow(DM9000_REG_NSR, 0x2c);
   iow(DM9000_REG_TCR, 0x00);
   iow(DM9000_REG_ISR, 0x3f);

#ifdef DM9000A_FLOW_CONTROL
   iow(DM9000_REG_BPTR, DM9000_BPTR_SET); /* 半双工流控设置 */
   iow(DM9000_REG_FCTR, DM9000_FCTR_SET); /* 全双工流控设置 */
   iow(DM9000_REG_FCR, DM9000_FCR_SET); /* 开启流控设置 */
#endif

#ifdef DM9000A_UPTO_100M
   /* DM9000A无此寄存器 */
   iow(DM9000_REG_OTCR, DM9000_OTCR_SET); /* 工作频率到 100Mhz 设置 */
#endif

#ifdef  Rx_Int_enable
   iow(DM9000_REG_IMR, DM9000_IMR_SET); /* 开启 中断模式 */
#else
   iow(DM9000_REG_IMR, DM9000_IMR_OFF); /* 关闭 中断模式 */
#endif

   iow(DM9000_REG_RCR, DM9000_RCR_SET); /* 开启 接收工能 */

   SendPackOk = 0;
}

/*******************************************************************************
 *  函数名: dm9k_phy_write
 *  参  数: phy_reg ： 寄存器地址
 *        writedata ： 写入的数据
 *  返  回: 无
 *  功  能: 写DM9000A PHY 寄存器
 */
void dm9k_phy_write(uint8_t phy_reg, uint16_t writedata)
{
   /* 设置写入 PHY 寄存器的位置 */
   iow(DM9000_REG_EPAR, phy_reg | DM9000_PHY);

   /* 设置写入 PHY 寄存器的值 */
   iow(DM9000_REG_EPDRH, (writedata >> 8) & 0xff);
   iow(DM9000_REG_EPDRL, writedata & 0xff);

   iow(DM9000_REG_EPCR, 0x0a); /* 将资料写入 PHY 寄存器 */
   while (ior(DM9000_REG_EPCR) & 0x01)
      ; /* 查寻是否执行结束 */
   iow(DM9000_REG_EPCR, 0x08); /* 清除写入命令 */
}

/*******************************************************************************
 *  函数名: dm9k_initnic
 *  参  数: 无
 *  返  回: 无
 *  功  能: 初始化DM9000AE
 */
void dm9k_initnic(void)
{
   iow(DM9000_REG_NCR, DM9000_REG_RESET); /* 对 DM9000A 进行软件重置 */
   dm9k_udelay(10); /* delay 10us */

   dm9k_hash_table(); /* 设置 DM9000A MAC 及 多播*/

   dm9k_reset(); /* 进行 DM9000A 软件设置 */

   iow(DM9000_REG_GPR, DM9000_PHY_OFF); /* 关闭 PHY ，进行 PHY 设置*/
   dm9k_phy_write(0x00, 0x8000); /* 重置 PHY 的寄存器 */
#ifdef DM9000A_FLOW_CONTROL
   dm9k_phy_write(0x04, 0x01e1 | 0x0400); /* 设置 自适应模式相容表 */
#else
   dm9k_phy_write(0x04, 0x01e1); /* 设置 自适应模式相容表 */
#endif
   /* 连接模式设置
    * 0x0000 : 固定10M半双工
    * 0x0100 : 固定10M全双工
    * 0x2000 : 固定100M半双工
    * 0x2100 : 固定100M全双工
    * 0x1000 : 自适应模式
    */
   dm9k_phy_write(0x00, 0x1000); /* 设置 基本连接模式 */

   iow(DM9000_REG_GPR, DM9000_PHY_ON); /* 结束 PHY 设置, 开启 PHY */
// dm9000_irq_init();
   //dm9k_debug_test();
}

/*******************************************************************************
 *  函数名: dm9k_receive_packet
 *  参  数: _uip_buf : 接收缓冲区
 *  返  回: > 0 表示接收的数据长度, 0表示没有数据
 *  功  能: 读取一包数据
 */
uint16_t dm9k_receive_packet(uint8_t *_uip_buf)
{
   uint16_t ReceiveLength;
   uint16_t *ReceiveData;
   uint8_t rx_int_count = 0;
   uint8_t rx_checkbyte;
   uint16_t rx_status, rx_length;
   uint8_t jump_packet;
   uint16_t i;
   uint16_t calc_len;
   uint16_t calc_MRR;

   do {
      ReceiveLength = 0; /* 清除接收的长度 */
      ReceiveData = (uint16_t *)_uip_buf;
      jump_packet = 0; /* 清除跳包动作 */
      ior(DM9000_REG_MRCMDX); /* 读取内存数据，地址不增加 */
      /* 计算内存数据位置 */
      calc_MRR = (ior(DM9000_REG_MRRH) << 8) + ior(DM9000_REG_MRRL);
      rx_checkbyte = ior(DM9000_REG_MRCMDX); /*  */

      if (rx_checkbyte == DM9000_PKT_RDY) /* 取 */
      {
         /* 读取封包相关资讯 及 长度 */
         NET_REG_ADDR = DM9000_REG_MRCMD;
         rx_status = NET_REG_DATA;
         rx_length = NET_REG_DATA;

         /* 若收到超过系统可承受的封包，此包跳过 */
         if (rx_length > Max_Ethernet_Lenth) jump_packet = 1;

#ifdef Broadcast_Jump
         /* 若收到的广播或多播包超过特定长度，此包跳过 */
         if (rx_status & 0x4000) {
            if (rx_length > Max_Broadcast_Lenth) jump_packet = 1;
         }
#endif
         /* 计算下一个包的指针位 , 若接收长度为奇数，需加一对齐偶字节。*/
         /* 若是超过 0x3fff ，则需回归绕到 0x0c00 起始位置 */
         calc_MRR += (rx_length + 4);
         if (rx_length & 0x01) calc_MRR++;
         if (calc_MRR > 0x3fff) calc_MRR -= 0x3400;

         if (jump_packet == 0x01) {
            /* 将指针移到下一个包的包头位置 */
            iow(DM9000_REG_MRRH, (calc_MRR >> 8) & 0xff);
            iow(DM9000_REG_MRRL, calc_MRR & 0xff);
            continue;
         }

         /* 开始将内存的资料搬到到系统中，每次移动一个 word */
         calc_len = (rx_length + 1) >> 1;
         for (i = 0; i < calc_len; i++)
            ReceiveData[i] = NET_REG_DATA;

         /* 将包长回报给 TCP/IP 上层，并减去最後 4 BYTE 的 CRC-32 检核码 */
         ReceiveLength = rx_length - 4;

         rx_int_count++; /* 累计收包次数 */

#ifdef FifoPointCheck
         if (calc_MRR != ((ior(DM9000_REG_MRRH) << 8) + ior(DM9000_REG_MRRL))) {
#ifdef Point_Error_Reset
            dm9k_reset(); /* 若是指针出错，重置 */
            return ReceiveLength;
#endif
            /*若是指针出错，将指针移到下一个包的包头位置  */
            iow(DM9000_REG_MRRH, (calc_MRR >> 8) & 0xff);
            iow(DM9000_REG_MRRL, calc_MRR & 0xff);
         }
#endif
         /*         printf("Receive pack len %d ", ReceiveLength);
          for(i=0; i<ReceiveLength; i++){
          printf("%02x ", _uip_buf[i]);
          }
          printf("\r\n");*/
         return ReceiveLength;
      }
      else {
         if (rx_checkbyte == DM9000_PKT_NORDY) /* 未收到包 */
         {
            iow(DM9000_REG_ISR, 0x3f); /*  */
         }
         else {
            dm9k_reset(); /* 接收指针出错，重置 */
         }
         return (0);
      }
   } while (rx_int_count < Max_Int_Count); /* 是否超过最多接收封包计数 */
   return 0;
}

/*******************************************************************************
 *  函数名: dm9k_send_packet
 *  参  数: p_char : 发送数据缓冲区
 *        length : 数据长度
 *  返  回: 无
 *  功  能: 发送一包数据
 */
void dm9k_send_packet(uint8_t *p_char, uint16_t length)
{
   uint16_t SendLength = length;
   uint16_t *SendData = (uint16_t *)p_char;
   uint16_t i;
   uint16_t calc_len;
   __IO uint16_t
   calc_MWR;

   /* 检查 DM9000A 是否还在传送中！若是等待直到传送结束 */
   if (SendPackOk == Max_Send_Pack) {
      while (ior(DM9000_REG_TCR) & DM9000_TCR_SET) {
         dm9k_udelay(5);
      }
      SendPackOk = 0;
   }

   SendPackOk++; /* 设置传送计数 */

#ifdef FifoPointCheck
   /* 计算下一个传送的指针位 , 若接收长度为奇数，需加一对齐偶字节。*/
   /* 若是超过 0x0bff ，则需回归绕到 0x0000 起始位置 */
   calc_MWR = (ior(DM9000_REG_MWRH) << 8) + ior(DM9000_REG_MWRL);
   calc_MWR += SendLength;
   if (SendLength & 0x01) calc_MWR++;
   if (calc_MWR > 0x0bff) calc_MWR -= 0x0c00;
#endif

   iow(DM9000_REG_TXPLH, (SendLength >> 8) & 0xff); /* 设置传送封包的长度 */
   iow(DM9000_REG_TXPLL, SendLength & 0xff);

   /* 开始将系统的资料搬到到内存中，每次移动一个 word */
   NET_REG_ADDR = DM9000_REG_MWCMD;
   calc_len = (SendLength + 1) >> 1;
   for (i = 0; i < calc_len; i++)
      NET_REG_DATA = SendData[i];

   iow(DM9000_REG_TCR, DM9000_TCR_SET); /* 进行传送 */

#ifdef FifoPointCheck
   if (calc_MWR != ((ior(DM9000_REG_MWRH) << 8) + ior(DM9000_REG_MWRL))) {
#ifdef Point_Error_Reset
      /* 若是指针出错，等待此一封包送完 , 之後进行重置 */
      while (ior(DM9000_REG_TCR) & DM9000_TCR_SET)
         dm9k_udelay(5);
      dm9k_reset();
      return;
#endif
      /*若是指针出错，将指针移到下一个传送包的包头位置  */
      iow(DM9000_REG_MWRH, (calc_MWR >> 8) & 0xff);
      iow(DM9000_REG_MWRL, calc_MWR & 0xff);
   }
#endif

   return;
}


/*******************************************************************************
 *  函数名: dm9k_debug_test
 *  参  数: 无
 *  返  回: 无
 *  功  能: 测试DM9000AE的函数,用于排错
 */
void dm9k_debug_test(void)
{
   uint32_t check_device;
   uint8_t check_iomode;
   uint8_t check_reg_fail = 0;
   uint8_t check_fifo_fail = 0;
   uint16_t i;
   uint16_t j;
   DEBUG("dm9k_debug_test:  Try dm9000.Test Start!!mak!\n");
   iow(DM9000_REG_NCR, DM9000_REG_RESET); /* 对 DM9000A 进行软件重置 */
   dm9k_udelay(10); /* delay 10us */
   iow(DM9000_REG_NCR, DM9000_REG_RESET); /* 对 DM9000A 进行软件重置 */
   dm9k_udelay(10); /* delay 10us */

   uint8_t vendor_l, vendor_h, product_l, product_h;

   dm9k_udelay(10); /* delay 10us */
   vendor_l = ior(DM9000_REG_VID_L);
   dm9k_udelay(10); /* delay 10us */
   vendor_h = ior(DM9000_REG_VID_H);
   dm9k_udelay(10); /* delay 10us */
   product_l = ior(DM9000_REG_PID_L);
   dm9k_udelay(10); /* delay 10us */
   product_h = ior(DM9000_REG_PID_H);
   dm9k_udelay(10); /* delay 10us */
   DEBUG("dm9k_debug_test:  DEVICEID Vendor Id  0x%2X,  0x%2X, PId: 0x%2X, 0x%2X \n", vendor_h, vendor_l, product_h, product_l);

   i = 5;
   while (i--) {
      dm9k_udelay(10); /* delay 10us */
      vendor_l = ior(DM9000_REG_VID_L);
      dm9k_udelay(10); /* delay 10us */
      vendor_h = ior(DM9000_REG_VID_H);
      dm9k_udelay(10); /* delay 10us */
      product_l = ior(DM9000_REG_PID_L);
      dm9k_udelay(10); /* delay 10us */
      product_h = ior(DM9000_REG_PID_H);
      dm9k_udelay(10); /* delay 10us */
      DEBUG("dm9k_debug_test:  DEVICEID Vendor Id  0x%2X,  0x%2X, PId: 0x%2X, 0x%2X \n", vendor_h, vendor_l, product_h, product_l);
   }

   check_device = ior(DM9000_REG_VID_L);
   check_device |= ior(DM9000_REG_VID_H) << 8;
   check_device |= ior(DM9000_REG_PID_L) << 16;
   check_device |= ior(DM9000_REG_PID_H) << 24;

   if (check_device != 0x90000A46) {
      DEBUG("dm9k_debug_test:  DEIVCE NOT FOUND, SYSTEM HOLD !!\n");
      while (1)
         ;
   }
   else {
      DEBUG("dm9k_debug_test:  DEIVCE FOUND !!\n");
   }

   check_iomode = ior(DM9000_REG_ISR) >> 6;
   if (check_iomode != DM9000_WORD_MODE) {
      DEBUG("dm9k_debug_test:  DEIVCE NOT WORD MODE, SYSTEM HOLD !!\n");
      while (1)
         ;
   }
   else {
      DEBUG("dm9k_debug_test:  DEIVCE IS WORD MODE !!\n");
   }

   DEBUG("dm9k_debug_test:  REGISTER R/W TEST !!\n");
   NET_REG_ADDR = DM9000_REG_MAR;
   for (i = 0; i < 0x0100; i++) {
      NET_REG_DATA = i;
      if (i != (NET_REG_DATA & 0xff)) {
         DEBUG("dm9k_debug_test: error W %02x , R %02x \n", i, NET_REG_DATA);
         check_reg_fail = 1;
      }
   }

   if (check_reg_fail) {
      DEBUG("dm9k_debug_test:  REGISTER R/W FAIL, SYSTEM HOLD !!\n");
      while (1)
         ;
   }

   DEBUG("dm9k_debug_test:  FIFO R/W TEST !!\n");DEBUG("dm9k_debug_test:  FIFO WRITE START POINT 0x%02x%02x \n",
               ior(DM9000_REG_MWRH), ior(DM9000_REG_MWRL));

   NET_REG_ADDR = DM9000_REG_MWCMD;
   for (i = 0; i < 0x1000; i++)
      NET_REG_DATA = ((i & 0xff) * 0x0101);

   DEBUG("dm9k_debug_test:  FIFO WRITE END POINT 0x%02x%02x \n",
               ior(DM9000_REG_MWRH), ior(DM9000_REG_MWRL));

   if ((ior(DM9000_REG_MWRH) != 0x20) || (ior(DM9000_REG_MWRL) != 0x00)) {
      DEBUG("dm9k_debug_test:  FIFO WRITE FAIL, SYSTEM HOLD !!\n");
      while (1)
         ;
   }

   ior(DM9000_REG_MRCMDX);
   DEBUG("dm9k_debug_test:  FIFO READ START POINT 0x%02x%02x \n",
               ior(DM9000_REG_MRRH), ior(DM9000_REG_MRRL));
   ior(DM9000_REG_MRCMDX);

   NET_REG_ADDR = DM9000_REG_MRCMD;
   for (i = 0; i < 0x1000; i++) {
      j = NET_REG_DATA;

      if (((i & 0xff) * 0x0101) != j) {
         //printk("             > error W %04x , R %04x \n",
         //    ((i & 0xff) * 0x0101) , j);
         check_fifo_fail = 1;
      }
   }

   DEBUG("dm9k_debug_test:  FIFO READ END POINT 0x%02x%02x \n",
               ior(DM9000_REG_MRRH), ior(DM9000_REG_MRRL));

   if ((ior(DM9000_REG_MRRH) != 0x20) || (ior(DM9000_REG_MRRL) != 0x00)) {
      DEBUG("dm9k_debug_test:  FIFO WRITE FAIL, SYSTEM HOLD !!\n");
      while (1)
         ;
   }

   if (check_fifo_fail) {
      DEBUG("dm9k_debug_test:  FIFO R/W DATA FAIL, SYSTEM HOLD !!\n");
      while (1)
         ;
   }

   DEBUG("dm9k_debug_test:  PACKET SEND & INT TEST !! \n");
   iow(DM9000_REG_NCR, DM9000_REG_RESET);
   dm9k_udelay(10);
   iow(DM9000_REG_NCR, DM9000_REG_RESET);
   dm9k_udelay(10);

   iow(DM9000_REG_IMR, DM9000_IMR_OFF | DM9000_TX_INTR);

   iow(DM9000_REG_TXPLH, 0x01);
   iow(DM9000_REG_TXPLL, 0x00);

   DEBUG("dm9k_debug_test: Have a break, SYSTEM HOLD !!\n");
   while (1)
      ;

   do {
      iow(DM9000_REG_ISR, DM9000_TX_INTR);
      DEBUG("dm9k_debug_test:  INT PIN IS OFF\n");

      NET_REG_ADDR = DM9000_REG_MWCMD;
      for (i = 0; i < (0x0100 / 2); i++) {
         if (i < 3)
            NET_REG_DATA = 0xffff;
         else
            NET_REG_DATA = i * 0x0101;
      }

      DEBUG("dm9k_debug_test:  PACKET IS SEND \n");
      iow(DM9000_REG_TCR, DM9000_TCR_SET);

      while (ior(DM9000_REG_TCR) & DM9000_TCR_SET)
         dm9k_udelay(5);
      if (ior(DM9000_REG_ISR) & DM9000_TX_INTR)
         DEBUG("dm9k_debug_test:  INT PIN IS ACTIVE \n");
      else
         DEBUG("dm9k_debug_test:  INT PIN IS NOT ACTIVE \n");

      for (i = 0; i < 10; i++)
         dm9k_udelay(1000);

   } while (1);
}


static int nd_dm9k_send(netdev_t *netdev, const struct iovec *data, unsigned count)
{
   dm9000_t *dev = (dm9000_t *)netdev;
   int c = 0;

   mutex_lock(&dev->devlock);

#ifdef MODULE_NETSTATS_L2
   netdev->stats.tx_bytes += count;
#endif


   for (unsigned i = 0; i < count; i++) {
       c += data[i].iov_len;
       dm9k_send_packet((uint8_t *)data[i].iov_base, data[i].iov_len);
   }

   mutex_unlock(&dev->devlock);
   return c;
}

static int nd_dm9k_recv(netdev_t *netdev, void *buf, size_t max_len, void *info)
{
   dm9000_t *dev = (dm9000_t *)netdev;
   size_t size;

   (void)info;
   mutex_lock(&dev->devlock);

   size = dm9k_receive_packet(buf);


#ifdef MODULE_NETSTATS_L2
      netdev->stats.rx_count++;
      netdev->stats.rx_bytes += size;
#endif


   mutex_unlock(&dev->devlock);
   return (int)size;
}

int nd_dm9k_init(netdev_t *netdev)
{
   dm9000_t *dev = (dm9000_t *)netdev;
   /* get exclusive access of the device */
   mutex_lock(&dev->devlock);

   dm9k_initnic();

   mutex_unlock(&dev->devlock);
   return 0;
}


static void nd_dm9k_isr(netdev_t *netdev)
{
   dm9000_t *dev = (dm9000_t *)netdev;
   (void)dev;
   return;
}

static int nd_dm9k_get(netdev_t *netdev, netopt_t opt, void *value, size_t max_len)
{
    switch (opt) {
        case NETOPT_ADDRESS:
            assert(max_len >= ETHERNET_ADDR_LEN);
            dm9k_get_mac( (uint8_t *)value);
            return ETHERNET_ADDR_LEN;
        default:
            return netdev_eth_get(netdev, opt, value, max_len);
    }
}


static int nd_dm9k_set(netdev_t *netdev, netopt_t opt, const void *value, size_t value_len)
{
    switch (opt) {
        case NETOPT_ADDRESS:
            assert(value_len == ETHERNET_ADDR_LEN);
            dm9k_set_mac((uint8_t *)value);
            dm9k_hash_table();
            return ETHERNET_ADDR_LEN;
        default:
            return netdev_eth_set(netdev, opt, value, value_len);
    }
}
const static netdev_driver_t netdev_driver_dm9000 = {
    .send = nd_dm9k_send,
    .recv = nd_dm9k_recv,
    .init = nd_dm9k_init,
    .isr = nd_dm9k_isr,
    .get = nd_dm9k_get,
    .set = nd_dm9k_set,
};

void dm9000_setup(dm9000_t *dev, const dm9000_params_t *params)
{
   dev->netdev.driver = &netdev_driver_dm9000;
   mutex_init(&dev->devlock);
}
