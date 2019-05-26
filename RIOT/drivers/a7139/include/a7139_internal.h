/**
 * @ingroup     drivers_a7139
 * @{
 *
 * @file
 * @brief       Internal functions for A7139 devices
 *
 * @author     xubingcan <xubingcan@klec.com.cn>
 */

#ifndef A7139_INTERNAL_H_
#define A7139_INTERNAL_H_

#include <stdint.h>

#include "a7139.h"

#ifdef __cplusplus
extern "C" {
#endif


#define TRUE    1
#define FALSE   0

void  mynop(void);
void A7139_pins_init(void);

void A7139_POR(void);
uint8_t InitRF(void);
uint8_t A7139_Config(void);
uint8_t A7139_WriteID(void);
uint8_t A7139_Cal(void);
uint32_t A7139_ReadID(void);

void StrobeCMD(uint8_t cmd);
void ByteSend(uint8_t src);
uint8_t ByteRead(void);
void A7139_WriteReg(uint8_t address, uint16_t dataWord);
uint16_t  A7139_ReadReg(uint8_t address);
void A7139_WritePageA(uint8_t address, uint16_t dataWord);
uint16_t A7139_ReadPageA(uint8_t address);
void A7139_WritePageB(uint8_t address, uint16_t dataWord);
uint16_t A7139_ReadPageB(uint8_t address);


void A7139_ReadFIFO(uint8_t *pData, uint8_t *DataLen);
void A7139_WriteFIFO(uint8_t *pData, uint8_t DataLen);
void Err_State(void);

void RCOSC_Cal(void);
void WOR_enable_by_preamble(void);
void WOR_enable_by_sync(void);
void WOR_enable_by_carrier(void);
void WOT_enable(void);
void TWOR_enable(void);
void RSSI_measurement(void);
void FIFO_extension_TX(void);
void FIFO_extension_RX(void);
void FIFO_extension_Infinite_TX(void);
void FIFO_extension_Infinite_RX(void);
void Auto_Resend(void);
void Auto_ACK(void);


void delay_1us(unsigned int xus);   //误差 0us  12M
void delay_1ms(unsigned int xms);
void A7139_Interrupt(void);
uint8_t get_status(void) ;
void set_status(uint8_t status);

//收发模式记录，用于中断处理发送或接收模式
void A7139_SendMode(uint8_t x);
uint8_t A7139_isSendMode(void);

void entry_deep_sleep_mode(void);
void wake_up_from_deep_sleep_mode(void);

#ifdef __cplusplus
}
#endif


#endif /* A7139_INTERNAL_H_ */
