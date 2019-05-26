#include "board.h"
#include "periph/spi.h"

#include "kldaq_fpga_spi.h"
#include "log.h"
#include "byteorder.h"

static int kldaq_fpga_spi_init(kldaq_dev_t *daq);
static int kldaq_fpga_spi_trigger_one_sample(kldaq_dev_t *daq, uint8_t channel);
static uint32_t kldaq_fpga_spi_get_length(kldaq_dev_t *daq, uint8_t channel);
static int kldaq_fpga_spi_read_data(kldaq_dev_t *daq, uint8_t channel, void *dest, uint32_t addr, size_t size);
static uint16_t kldaq_fpga_spi_get_max(kldaq_dev_t *daq, uint8_t channel);

static int kldaq_fpga_spi_is_over_threshold(kldaq_dev_t *daq, uint8_t channel);
static int kldaq_fpga_spi_set_threshold(kldaq_dev_t *daq, uint8_t channel, uint16_t threshold);
static int kldaq_fpga_spi_set_changerate(kldaq_dev_t *daq, uint8_t channel, uint16_t changerate);
static int kldaq_fpga_spi_read_version(kldaq_dev_t *daq, uint16_t *fpga_version);
const daq_opcode_t fpga_spi_opcode_default = {
                .rreg = 0x5,
                .rdat = 0x6,
                .wreg = 0x1
};


const kldaq_func_t kldaq_fpga_spi_driver = {
    .init              = kldaq_fpga_spi_init,
    .trigger_single_sample = kldaq_fpga_spi_trigger_one_sample,
    .get_length        = kldaq_fpga_spi_get_length,
    .read_data         = kldaq_fpga_spi_read_data,
    .get_max           = kldaq_fpga_spi_get_max,
    .is_over_threshold = kldaq_fpga_spi_is_over_threshold,
    .set_threshold     = kldaq_fpga_spi_set_threshold,
	.set_changerate    = kldaq_fpga_spi_set_changerate,
	.read_version      = kldaq_fpga_spi_read_version,

};

/* fpga spi pin cfg */
#define KLSTM32_FPGA_SPI      SPI_DEV(1)
#define KLSTM32_FPGA_SPI_CS   GPIO_PIN(PORT_B, 0)

fpga_spi_dev_t fpga_spi_with_ps_cnf_dev = {
   .base   = {&kldaq_fpga_spi_driver, &kldaq_fpga_ps_cnf_ops},
   .spi    = KLSTM32_FPGA_SPI,
   .opcode = &fpga_spi_opcode_default,
   .cs_pin = KLSTM32_FPGA_SPI_CS,
   .mode   = SPI_MODE_3,
   .clk    = SPI_CLK_10MHZ,
};

fpga_spi_dev_t fpga_spi_with_dummy_cnf_dev = {
   .base   = {&kldaq_fpga_spi_driver, &kldaq_fpga_dummy_cnf_ops},
   .spi    = KLSTM32_FPGA_SPI,
   .opcode = &fpga_spi_opcode_default,
   .cs_pin = KLSTM32_FPGA_SPI_CS,
   .mode   = SPI_MODE_3,
   .clk    = SPI_CLK_10MHZ,
};

static int kldaq_fpga_spi_init(kldaq_dev_t *daq)
{
    LOG_INFO("kldaq_fpga_spi_init");

    fpga_spi_dev_t *spi_conf = (fpga_spi_dev_t *)daq;

    int tmp;
    tmp = spi_init_cs(spi_conf->spi, spi_conf->cs_pin);
    if (tmp != SPI_OK) {
        LOG_ERROR("unable to initialize the given chip select line");
    }

    return 0;
}


#define ADDR_LEN_SIZE (3)
static int kldaq_fpga_spi_read_data(kldaq_dev_t *daq, uint8_t channel, void *dest, uint32_t addr, size_t size)
{
    fpga_spi_dev_t *spi_conf = (fpga_spi_dev_t *)daq;
    LOG_DEBUG("FPGA SPI read sample data with cmd:%02x, chan:%02x, addr:%ld", spi_conf->opcode->rdat, channel, addr);

    int tmp;
    uint8_t cmd_chan = 0;
    uint8_t addr_out[ADDR_LEN_SIZE] = {0};


    tmp = spi_acquire(spi_conf->spi, spi_conf->cs_pin, spi_conf->mode, spi_conf->clk);
    if (tmp == SPI_NOMODE) {
        LOG_ERROR("given SPI mode is not supported");
        return tmp;
    }
    else if (tmp == SPI_NOCLK) {
        LOG_ERROR("targeted clock speed is not supported");
        return tmp;
    }
    else if (tmp != SPI_OK) {
        LOG_ERROR("unable to acquire bus with given parameters");
        return tmp;
    }

    LOG_DEBUG("SPI_DEV(%d) initialized: mode: %i, clk: %i\n", spi_conf->spi, spi_conf->mode, spi_conf->clk);
    cmd_chan = ((spi_conf->opcode->rdat & 0x0F) << 4) | (channel & 0x0F);  //bit0~3: channel number; bit4~7: command code
    spi_transfer_bytes(spi_conf->spi, spi_conf->cs_pin, 1, &cmd_chan, NULL, 1);

    addr_out[0] = (addr >> 16) & 0xFF;
    addr_out[1] = (addr >> 8) & 0xFF;
    addr_out[2] = addr & 0xFF;
    spi_transfer_bytes(spi_conf->spi, spi_conf->cs_pin, 1, addr_out, NULL, ADDR_LEN_SIZE);
    spi_transfer_bytes(spi_conf->spi, spi_conf->cs_pin, 0, NULL, dest, size);

    spi_release(spi_conf->spi);

    return 0;
}
#define FPGA_REG_LEN 2 /*2 Byte*/
static int _spi_read_register(kldaq_dev_t *daq, uint8_t channel, uint8_t reg_addr, uint16_t *value)
{
    fpga_spi_dev_t *spi_conf = (fpga_spi_dev_t *)daq;
    LOG_DEBUG("FPGA SPI read register with cmd:%02x, chan:%02x\n, regAddr:%02x", spi_conf->opcode->rreg, channel, reg_addr);

    int tmp;
    uint8_t cmd_chan = 0;
    uint8_t buff[FPGA_REG_LEN] = { 0 };

    tmp = spi_acquire(spi_conf->spi, spi_conf->cs_pin, spi_conf->mode, spi_conf->clk);
    if (tmp == SPI_NOMODE) {
        LOG_ERROR("given SPI mode is not supported");
        return tmp;
    }
    else if (tmp == SPI_NOCLK) {
        LOG_ERROR("targeted clock speed is not supported");
        return tmp;
    }
    else if (tmp != SPI_OK) {
        LOG_ERROR("unable to acquire bus with given parameters");
        return tmp;
    }

    LOG_DEBUG("SPI_DEV(%d) initialized: mode: %i, clk: %i\n", spi_conf->spi, spi_conf->mode, spi_conf->clk);
    cmd_chan = ((spi_conf->opcode->rreg & 0x0F) << 4) | (channel & 0x0F);  //bit0~3: channel number; bit4~7: command code

    spi_transfer_bytes(spi_conf->spi, spi_conf->cs_pin, 1, &cmd_chan, NULL, 1);
    spi_transfer_bytes(spi_conf->spi, spi_conf->cs_pin, 1, &reg_addr, NULL, 1);
    spi_transfer_bytes(spi_conf->spi, spi_conf->cs_pin, 0, NULL, buff, FPGA_REG_LEN);

    spi_release(spi_conf->spi);

    *value = (buff[0] << 8) | buff[1]; /**/
    LOG_DEBUG("FPGA SPI read register value:%02x", *value);
    return 0;
}

static int _spi_write_register(kldaq_dev_t *daq, uint8_t channel, uint8_t reg_addr, uint16_t value)
{
    fpga_spi_dev_t *spi_conf = (fpga_spi_dev_t *)daq;
    LOG_DEBUG("FPGA SPI write register with cmd:%02x, chan:%02x\n, regAddr:%02x with value 0x%4X", spi_conf->opcode->wreg, channel, reg_addr, value);

    int tmp;
    uint8_t cmd_chan = 0;
    le_uint16_t little_u;
    be_uint16_t big_u;
    little_u.u16 = value;
    big_u = byteorder_ltobs(little_u);

    tmp = spi_acquire(spi_conf->spi, spi_conf->cs_pin, spi_conf->mode, spi_conf->clk);
    if (tmp == SPI_NOMODE) {
        LOG_ERROR("given SPI mode is not supported");
        return tmp;
    }
    else if (tmp == SPI_NOCLK) {
        LOG_ERROR("targeted clock speed is not supported");
        return tmp;
    }
    else if (tmp != SPI_OK) {
        LOG_ERROR("unable to acquire bus with given parameters");
        return tmp;
    }

    LOG_DEBUG("SPI_DEV(%d) initialized: mode: %i, clk: %i\n", spi_conf->spi, spi_conf->mode, spi_conf->clk);
    cmd_chan = ((spi_conf->opcode->wreg & 0x0F) << 4) | (channel & 0x0F);  //bit0~3: channel number; bit4~7: command code

    spi_transfer_bytes(spi_conf->spi, spi_conf->cs_pin, 1, &cmd_chan, NULL, 1);
    spi_transfer_bytes(spi_conf->spi, spi_conf->cs_pin, 1, &reg_addr, NULL, 1);
    spi_transfer_bytes(spi_conf->spi, spi_conf->cs_pin, 0, &big_u.u16, NULL, FPGA_REG_LEN);

    spi_release(spi_conf->spi);

    LOG_DEBUG("FPGA SPI Write register value Success");
    return 0;
}

static uint32_t kldaq_fpga_spi_get_length(kldaq_dev_t *daq, uint8_t channel)
{
   uint16_t tmp_h;
   uint16_t tmp_l;
   uint32_t dat_len;

   _spi_read_register(daq, channel, 2, &tmp_h);
   _spi_read_register(daq, channel, 3, &tmp_l);

   dat_len = (tmp_h << 16) + tmp_l;
   return dat_len;
}

static int kldaq_fpga_spi_read_version(kldaq_dev_t *daq, uint16_t *fpga_version)
{
    uint8_t channel = 0;//读版本时，channel 无意义
    (void)daq;
    (void)channel;
    for(int i = 0; i < 32; i++)
    {
       _spi_read_register(daq, channel, 8, &fpga_version[i]);
//       fpga_version[i]  = i;
    }

   return 0;
}

static int kldaq_fpga_spi_is_over_threshold(kldaq_dev_t *daq, uint8_t channel)
{
   uint16_t ret = 0;
   _spi_read_register(daq, channel, 1, &ret);
   return ret & CHA_STA_REG_INT_MASK;
}



static uint16_t kldaq_fpga_spi_get_max(kldaq_dev_t *daq, uint8_t channel)
{
   (void)daq;

   uint16_t max;
   uint16_t min;
   uint16_t tmp = 0;

   _spi_read_register(daq, channel, 4, &max);
   _spi_read_register(daq, channel, 5, &min);

   if ((max - 2048) >= (2048 - min)) {
       tmp = max;
   }
   else {
       tmp = 2048 - min;
   }
   LOG_INFO("Over voltage curve channel:%d data with Max(Reg4-2048):%d, Min(2048-Reg4):%d", channel, max - 2048, 2048 - min);

   return tmp;
}

static int kldaq_fpga_spi_set_threshold(kldaq_dev_t *daq, uint8_t channel, uint16_t threshold)
{
   (void)daq;

   _spi_write_register(daq, channel, 1, threshold);

   return 0;
}

static int kldaq_fpga_spi_set_changerate(kldaq_dev_t *daq, uint8_t channel, uint16_t changerate)
{
   (void)daq;

   _spi_write_register(daq, channel, 6, changerate);

   return 0;
}


static int kldaq_fpga_spi_trigger_one_sample(kldaq_dev_t *daq, uint8_t channel)
{
   (void)daq;
   (void)channel;
//   uint16_t reg0 = (REG0_CHANNEL_STARTBIT << channel);

   _spi_write_register(daq, channel, 0, 0x0070);
   return 0;
}

#define MAX_FPGA_SPI_DATA_LEN (8192) //24k
static uint8_t g_sample_spi_buf[MAX_FPGA_SPI_DATA_LEN] __attribute__((section(".big_data")));

kldaq_dev_t *g_daq_dev = (kldaq_dev_t*)&fpga_spi_with_ps_cnf_dev;
void kldaq_test_spi_one_channels(uint8_t channel)
{

   uint32_t len = 0;
   printf("Try to test SPI FPGA channel %d\r\n", channel);
   printf("Step1: Read len\r\n");

   len = kldaq_get_length(g_daq_dev, channel);
   printf("Get length on Channel %d: %"PRIu32" successful!\r\n", channel, len);

   if (len > MAX_FPGA_SPI_DATA_LEN) len = MAX_FPGA_SPI_DATA_LEN;
   printf("Step2: Read Data\r\n");

   kldaq_read_data(g_daq_dev, channel, g_sample_spi_buf, 0, len);
   printf("Read data from Channel %d successful!\r\n", channel);

   for (uint32_t i = 0; i < len / 32; i++) {
      LOG_DUMP((const char*)(g_sample_spi_buf + 32 * i), 32);
   }

   printf("Step3: Reset Data Flag\r\n");
   kldaq_trigger_one_sample(g_daq_dev, channel);
   printf("Tell FPGA data been read on channel %d.!\r\n", channel);

   printf("All works Done!!!\r\n");

}

void daq_test_spi_main(uint8_t channel)
{
   kldaq_test_spi_one_channels(channel);
}
