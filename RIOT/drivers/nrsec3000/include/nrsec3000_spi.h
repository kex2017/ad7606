
#ifndef NRSEC3000_SPI_H_
#define NRSEC3000_SPI_H_

#include <stdint.h>
#include <stdio.h>
struct _spi_device;
typedef struct _spi_device spi_device_t;

spi_device_t *nrsec3000_spi_device_create(void);
void nrsec3000_spi_sync(spi_device_t  *spi, const uint8_t *tx, size_t len, uint8_t *rx);

#endif /* NRSEC3000_SPI_H_ */
