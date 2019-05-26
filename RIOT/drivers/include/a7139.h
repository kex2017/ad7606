/**
 * @defgroup   drivers_a7139 A7139 Sub 1GHz Transceiver device driver
 * @ingroup    drivers_netdev
 * @brief      I2C Analog-to-Digital Converter device driver
 *
 *             This driver works with a7139 versions.
 * @{
 *
 * @file
 * @brief      A7139  Sub 1GHz Transceiver  device driver
 *
 * @author     xubingcan <xubingcan@klec.com.cn>
 */


#ifndef A7139_H_
#define A7139_H_

#include <stdint.h>
#include <stdbool.h>

#include "board.h"

#ifdef __cplusplus
extern "C" {
#endif

/* PHY Data Service */

void A7139_SENDPACKET (uint8_t *lpData, uint8_t LEN);
void A7139_SetSendDataLen(uint16_t DataLen);
bool A7139_SendData(uint8_t *pData, uint16_t DataLen);

bool A7139_SetRevListen(uint8_t *pRevBuff, uint16_t RevBuffSize);
int A7139_GetRxLen(void);
void A7139_Recv_Int_Init(void);

void A7139_RecInterrupt(void);

/* PHY Managemnet Entity Service*/
void A7139_newInitConfig(void);

void A7139_SetTxPowerSupply(uint8_t PowerSupply);
void A7139_Reset(void);
void A7139_Sleep(void);

#ifdef __cplusplus
}
#endif

#endif /* A7139_H_ */
