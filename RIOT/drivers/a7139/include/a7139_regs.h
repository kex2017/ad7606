/********************************************************************
*   A7139REG.h
*   RF Chip-A7139 Hardware Definitions
*
*   This file provides the constants associated with the
*   AMICCOM A7139 device.
*
********************************************************************/
#ifndef A7139_REGS_H_
#define A7139_REGS_H_

/* Control Register Table */
#define SYSTEMCLOCK_REG     0x00
#define PLL1_REG            0x01
#define PLL2_REG            0x02
#define PLL3_REG            0x03
#define PLL4_REG            0x04
#define PLL5_REG            0x05
#define PLL6_REG            0x06
#define CRYSTAL_REG         0x07
#define PAGEA_REG           0x08
#define PAGEB_REG           0x09
#define RX1_REG             0x0A
#define RX2_REG             0x0B
#define ADC_REG             0x0C
#define PIN_REG             0x0D
#define CALIBRATION_REG     0x0E
#define MODE_REG            0x0F

/*Page A */
#define TX1_PAGEA           0x00
#define WOR1_PAGEA          0x01
#define WOR2_PAGEA          0x02
#define RFI_PAGEA           0x03
#define PM_PAGEA            0x04
#define RTH_PAGEA           0x05
#define AGC1_PAGEA          0x06
#define AGC2_PAGEA          0x07
#define GIO_PAGEA           0x08
#define CKO_PAGEA           0x09
#define VCB_PAGEA           0x0A
#define CHG1_PAGEA          0x0B
#define CHG2_PAGEA          0x0C
#define FIFO_PAGEA          0x0D
#define CODE_PAGEA          0x0E
#define WCAL_PAGEA          0x0F

/* Page B */
#define TX2_PAGEB           0x00
#define IF1_PAGEB           0x01
#define IF2_PAGEB           0x02
#define ACK_PAGEB           0x03
#define ART_PAGEB           0x04

/* CMD flag */
#define CMD_Reg_W           0x00    //000x,xxxx control register write
#define CMD_Reg_R           0x80    //100x,xxxx control register read
#define CMD_ID_W            0x20    //001x,xxxx ID write
#define CMD_ID_R            0xA0    //101x,xxxx ID Read
#define CMD_FIFO_W          0x40    //010x,xxxx TX FIFO Write
#define CMD_FIFO_R          0xC0    //110x,xxxx RX FIFO Read
#define CMD_RF_RST         0xFF  //x111,xxxx：1111,1111 RF reset
#define CMD_TFR             0x60    //0110,xxxx TX FIFO address pointrt reset
#define CMD_RFR             0xE0    //1110,xxxx RX FIFO address pointer reset

#define CMD_SLEEP           0x10    //0001,0000 SLEEP mode
#define CMD_IDLE            0x12    //0001,0010 IDLE mode
#define CMD_STBY            0x14    //0001,0100 Standby mode
#define CMD_PLL             0x16    //0001,0110 PLL mode
#define CMD_RX              0x18    //0001,1000 RX mode
#define CMD_TX              0x1A    //0001,1010 TX mode
//#define CMD_DEEP_SLEEP   0x1C  //0001,1100 Deep Sleep mode(tri-state)
#define CMD_DEEP_SLEEP     0x1F  //0001,1111 Deep Sleep mode(pull-high)


//控制寄存器组A
typedef enum
{
    //寄存器8
    A7139_REG8_TX1      =   0,          //addr8 page0,
    A7139_REG8_WOR1     =   1,          //addr8 page1,
    A7139_REG8_WOR2     =   2,          //addr8 page2,
    A7139_REG8_RF       =   3,          //addr8 page3,
    A7139_REG8_POWER    =   4,          //addr8 page4,
    A7139_REG8_AGCRC    =   5,          //addr8 page5,
    A7139_REG8_AGCCON1  =   6,          //addr8 page6,
    A7139_REG8_AGCCON2  =   7,          //addr8 page7,
    A7139_REG8_GPIO     =   8,          //addr8 page8,
    A7139_REG8_CKO      =   9,          //addr8 page9,
    A7139_REG8_VCO      =   10,         //addr8 page10,
    A7139_REG8_CHG1     =   11,         //addr8 page11,
    A7139_REG8_CHG2     =   12,         //addr8 page12,
    A7139_REG8_FIFO     =   13,         //addr8 page13,
    A7139_REG8_CODE     =   14,         //addr8 page14,
    A7139_REG8_WCAL     =   15,         //addr8 page15,
}A7139_PAGE_A;

//控制寄存器组B
typedef enum
{
    //寄存器9
    A7139_REG9_TX2      =   0,      //addr9 page0,
    A7139_REG9_IF1      =   1,      //addr9 page1,
    A7139_REG9_IF2      =   2,      //addr9 page2,
    A7139_REG9_ACK      =   3,      //addr9 page3,
    A7139_REG9_ART      =   4,      //addr9 page4,
}A7139_PAGE_B;

typedef enum
{
    A7139_SCLOCK    =   0x00,       //系统时钟寄存器
    A7139_PLL1      =   0x01,       //PLL1
    A7139_PLL2      =   0x02,       //PLL2
    A7139_PLL3      =   0x03,       //PLL3
    A7139_PLL4      =   0x04,       //PLL4
    A7139_PLL5      =   0x05,       //PLL5
    A7139_PLL6      =   0x06,       //PLL6
    A7139_CRYSTAL   =   0x07,       //晶振设置
    A7139_PREG8S    =   0x08,       //寄存器组,由CRYSTAL控制切换
    A7139_PREG9S    =   0x09,       //寄存器组,由CRYSTAL控制切换
    A7139_RX1       =   0x0A,       //接收设置1
    A7139_RX2       =   0x0B,       //接收设置2
    A7139_ADC       =   0x0C,       //ADC
    A7139_PIN       =   0x0D,       //PIN
    A7139_CALIB     =   0x0E,       //Calibration
    A7139_MODE      =   0x0F,       //模式控制
}A7139_CREG;
#endif /* A7139_REGS_H_ */
