
#ifndef CRC7_H_
#define CRC7_H_

#include <stdint.h>
/*
描述：
    计算出一串数据的CRC校验码
参数：
    buff: [输入]，数据缓冲的首地址
    len: [输入]，数据长度
返回结果：
    计算出的CRC校验码
*/
uint8_t get_crc7(const uint8_t *buff, int len);

#endif /* CRC7_H_ */
