#include "nrsec3000_spi.h"

#include <string.h>
#include <unistd.h>
#include <errno.h>

#include "log.h"
#include "periph_conf.h"
#include "periph/spi.h"
#include "periph/gpio.h"

/* nrsec3000 spi pin cfg */
#define KLSTM32_NRSEC3000_SPI      SPI_DEV(0)
#define KLSTM32_NRSEC3000_SPI_CS   (GPIO_PIN(PORT_G, 10))

struct _spi_device {
   spi_t bus;
   gpio_t cs_pin;           /**< SPI CS pin */
   spi_mode_t mode;         /**< SPI mode */
   spi_clk_t clk;           /**< SPI clock */
};

int spi_device_setup(spi_device_t *spi)
{
   int tmp;
   tmp = spi_init_cs(spi->bus, spi->cs_pin);
   if (tmp != SPI_OK) {
       LOG_ERROR("unable to initialize the given chip select line");
   }

   return 0;
}

static spi_device_t nrsec3000_spi_dev = {
            .bus = KLSTM32_NRSEC3000_SPI,
            .cs_pin =  KLSTM32_NRSEC3000_SPI_CS,
            .mode   = SPI_MODE_3,
            .clk    = SPI_CLK_10MHZ,

};

spi_device_t* nrsec3000_spi_device_create(void)
{
   spi_device_t *spi = &nrsec3000_spi_dev;

   LOG_INFO("Try to setup nrsec3000 spi device bus(%d) cs_pin(0x%8X) mode(%d) clk(%d)", spi->bus, spi->cs_pin, spi->mode, spi->clk);
   spi_device_setup(spi);

   return spi;
}

void nrsec3000_spi_sync(spi_device_t  *spi, const uint8_t *tx, size_t len,
                                     uint8_t *rx)
{
   size_t i;

   spi_transfer_bytes(spi->bus, spi->cs_pin, 1, tx, rx, len);

   for(i = 0; i < len; i++) {
      LOG_DEBUG(".%.2X %.2X", tx[i], rx[i]);
   }
}
