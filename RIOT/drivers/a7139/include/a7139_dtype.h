
/**
 * @ingroup     drivers_a7139
 * @{
 *
 * @file
 * @brief       dtype definition for A7139 devices
 *
 * @author     xubingcan <xubingcan@klec.com.cn>
 */


#ifndef A7139_DTYPE_H_
#define A7139_DTYPE_H_

#define NO                  0
#define YES                 1

#define CALIBRATE_NUMBER    6

#define LOW                 0
#define HIGH                1

#define ON                  1
#define OFF                 0

#define ENABLE              1
#define DISABLE             0

#define MASTER          1
#define SLAVE           0

#define ONEWAY          0
#define TWOWAY          1

#define TRUE    1
#define FALSE   0


//发送数据结构
typedef struct
{
    uint8_t *pSendData;  //发送数据缓冲区指针
    uint16_t SendLen;    //需要发送数据长度
    uint16_t TranLen;    //已经发送数据长度
    uint8_t isSendOK;  //发送完成
    uint8_t isSendError;//发送失败
}A7139_SEND_TYPE;

//接收数据结构
typedef struct
{
    uint8_t *pRevData;   //接收数据缓冲区指针
    uint16_t RevLen;     //需要接收数据长度
    uint16_t TranLen;    //已经接收数据长度
    uint16_t RevBuffSize;//接收缓冲区大小
    uint8_t isRevOK;   //接收完成
    uint8_t isRevError;//接收失败
}A7139_REV_TYPE;

#endif /* A7139_DTYPE_H_ */
