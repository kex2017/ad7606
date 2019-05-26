#ifndef KL_SOFT_SPI_H
#define KL_SOFT_SPI_H

#include "periph/gpio.h"
#include "periph/spi.h"
#include "mutex.h"

#include "soft_spi.h"

typedef enum {
    KL_SPI_MOSI = 0,
    KL_SPI_CLK  = 1,
    KL_SPI_CS   = 2,
    KL_SPI_MISO = 15,
} KL_SPI_PIN;

#ifdef __cplusplus
extern "C" {
#endif


int kl_soft_spi_acquire(soft_spi_t bus, KL_SPI_PIN cs, soft_spi_mode_t mode, soft_spi_clk_t clk);

void kl_soft_spi_release(soft_spi_t bus);

void kl_soft_spi_transfer_bytes(soft_spi_t bus, KL_SPI_PIN cs, bool cont,
                        const void *out, void *in, size_t len);



#ifdef __cplusplus
}
#endif

#endif /* SOFT_SPI_H */
/** @} */
