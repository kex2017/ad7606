#include <stdio.h>
#include <string.h>

#include "security_device.h"
#include "security_private.h"
#include "nrsec3000_spi.h"
#include "log.h"
#include "misc.h"
#include "crc7.h"
#include "expect_match.h"
#include "byteorder.h"
#include "periph/spi.h"

enum CLA {
   CMD_CLA_STANDARD = 0x00,
   CMD_CLA_CUSTOM = 0x80,
   CMD_CLA_SM1_CALC = 0xA0
};

enum INS {
   INS_GET_VERSION = 0x5B,
   INS_GENERATE_RANDOM = 0x84,
   INS_GENERATE_SM2_KEY = 0xB2,
   INS_SM2_ENCRYPT = 0xB3,
   INS_SM2_SIGN = 0xB4,
   INS_SM3_HASH = 0xB5,
   INS_SM2_VERIFY  = 0xB6,
   INS_GENERATE_SM2_CERT_REQ = 0xB7,
   INS_EXPORT_SM2_KEY = 0xB8,
   INS_IMPORT_SM2_KEY = 0xBA,
   INS_IMPORT_SM1_KEY = 0xD4,
   INS_SM1_CRYPT = 0xE0,
};
typedef union cmd_header {
   uint8_t dat[5];
   struct {
      uint8_t cla;
      uint8_t ins;
      uint8_t p1;
      uint8_t p2;
      uint8_t p3;
   };
}header_t;
const size_t CMD_HEADER_LEN = 5;
void cmd_header_fill(header_t* hdr, uint8_t cla, uint8_t ins, uint8_t p1, uint8_t p2, uint8_t p3)
{
   hdr->cla = cla;
   hdr->ins = ins;
   hdr->p1 = p1;
   hdr->p2 = p2;
   hdr->p3 = p3;
}

void spi_keep_sent_aa_and_wait_for_expected(spi_device_t *spi, uint8_t *buf, uint8_t size)
{
   uint8_t tx1 = 0xAA;
   uint8_t rx1;
   expect_match_t *matcher = expect_matcher_create(buf, size);

   int i = 10;
   while (!is_expect_matcher_bingo(matcher) && i--) {
      nrsec3000_spi_sync(spi, &tx1, 1, &rx1);
      expect_matcher_update(matcher, rx1);
   }
   expect_matcher_destroy(matcher);
}

static void send_cmd_header(spi_device_t *spi, header_t* hdr)
{
   uint8_t rx[CMD_HEADER_LEN];
   nrsec3000_spi_sync(spi, hdr->dat, CMD_HEADER_LEN, rx);
}

void send_data_send_flag(spi_device_t *spi)
{
   uint8_t tx1 = 0x55;
   uint8_t rx1;

   nrsec3000_spi_sync(spi, &tx1, 1, &rx1);
}

void keep_send_aa_to_wait_for_cmd_ins_confirm(spi_device_t *spi, uint8_t cmd)
{
   uint8_t expected = cmd;
   spi_keep_sent_aa_and_wait_for_expected(spi, &expected, 1);
   LOG_INFO("CMD %.2X Bingo", expected);
}
void wait_for_status_word(spi_device_t *spi)
{
   uint8_t expected[] = {0x90, 0x00};
   spi_keep_sent_aa_and_wait_for_expected(spi, expected, sizeof(expected));
   LOG_INFO("SW 0x90 0x00 Bingo");
}

uint8_t read_expected_1byte_data_len(spi_device_t *spi)
{
   uint8_t tx1 = 0xAA;
   uint8_t rx1 = 0xAA;

   while (rx1 == tx1) {
      nrsec3000_spi_sync(spi, &tx1, 1, &rx1);
   }

   LOG_INFO("Read expected datalen %d", rx1);
   return rx1;
}

int read_expected_2byte_data_len(spi_device_t *spi)
{
   int  idx = 0;
   union {
      uint16_t len;
      char rx[2];
   }u;

   uint8_t tx1 = 0xAA;
   uint8_t rx1 = 0xAA;

   while (idx < 2) {
      nrsec3000_spi_sync(spi, &tx1, 1, &rx1);
      if (rx1 != tx1) {
         u.rx[idx++] = rx1;
      }
   }

//   len = byteorder_ntohs((network_uint16_t)*(uint16_t*)rx);
//   len = *(uint16_t*)rx;
   LOG_INFO("Read expected datalen %d", u.len);
   return u.len;
}

//static void spi_keep_sent_aa_and_wait_for_notAA(spi_device_t *spi, uint8_t *buf)
//{
//   uint8_t tx1 = 0xAA;
//   uint8_t rx1 = 0xAA;
//
//   while (rx1 == tx1){
//      nrsec3000_spi_sync(spi, &tx1, 1, &rx1);
//   }
//   *buf = rx1;
//}

#define BUF_SIZE 4096
static uint8_t rx_buf[BUF_SIZE] __attribute__((section(".big_data")));
static uint8_t tx_buf[BUF_SIZE] __attribute__((section(".big_data")));
int read_data(spi_device_t *spi, uint8_t *buf, int buflen)
{
   int ret = 0;
   uint8_t crc7;
   int idx, left_len, raw_len = buflen + 1;
   uint8_t *rx = rx_buf;
//   uint8_t *tx = tx_buf;

   uint8_t tx1 = 0xAA;
   uint8_t rx1 = 0xAA;

   idx = 0;
   left_len =  raw_len;
   LOG_INFO("Try to read %d bytes data", raw_len);
   while (left_len) {
      nrsec3000_spi_sync(spi, &tx1, 1, &rx1);
      rx[idx++] = rx1;
      left_len--;
   }
   /*
      memset(tx, 0xAA, raw_len);
      nrsec3000_spi_sync(spi, tx, raw_len, rx);
   */
   LOG_INFO("Read %d bytes data success", raw_len);
   LOG_DUMP((const char*)rx,raw_len);

   crc7=get_crc7(rx, buflen);
   if (crc7 != rx[buflen]) {
      LOG_WARN("CRC7 expectd(%.2X) != wire(%.2X)", crc7, rx[buflen]);
      ret = -1;
   }else {
      memcpy(buf, rx, buflen);
   }

   return ret;
}

int read_status_word(spi_device_t *spi, uint8_t *sw)
{

   uint8_t tx1 = 0xAA;
   uint8_t rx1 = 0xAA;

   LOG_INFO("Try to read status word");
   while (rx1 == tx1) {
      nrsec3000_spi_sync(spi, &tx1, 1, &rx1);
   }
   sw[0] = rx1;
   rx1 = tx1 = 0xAA;
   while (rx1 == tx1) {
      nrsec3000_spi_sync(spi, &tx1, 1, &rx1);
   }
   sw[1] = rx1;

   LOG_INFO("Read status word(%.2X %.2X)", sw[0], sw[1]);

   return 0;
}

void send_data(spi_device_t *spi, uint8_t *buf, uint16_t len)
{
   uint16_t raw_len = len + 1; /*add CRC byte*/
   uint8_t* tx = tx_buf;
   uint8_t* rx = rx_buf;

   memcpy(tx, buf, len);
   tx[len] = get_crc7(buf, len);
   nrsec3000_spi_sync(spi, tx, raw_len, rx);
}

void send_cmd_data_and_recv_sw_frame(spi_device_t *spi, header_t *hdr, uint8_t *buf)
{
   send_cmd_header(spi, hdr);
   keep_send_aa_to_wait_for_cmd_ins_confirm(spi, hdr->ins);
   send_data_send_flag(spi);
   send_data(spi, buf, hdr->p3);
   wait_for_status_word(spi);
}

int send_cmd_and_recv_data_sw_frame(spi_device_t *spi, header_t *hdr, uint8_t *buf, uint8_t len, uint8_t *sw)
{
   uint8_t raw_len;
   send_cmd_header(spi, hdr);
   keep_send_aa_to_wait_for_cmd_ins_confirm(spi, hdr->ins);

   raw_len = read_expected_1byte_data_len(spi);
   if (raw_len - 1 != len) {
      LOG_WARN("Len Mismatch, Expected %d but Actual %d", len + 1, raw_len);
   }
   read_data(spi, buf, len);
   read_status_word(spi, sw);

   return sw[0] << 8 | sw[1];
}

int send_cmd_data_and_recv_size1byte_data_sw_frame(spi_device_t *spi, header_t *hdr,
                                         uint8_t *tx, uint16_t tx_len,
                                         uint8_t *rx, uint8_t rx_len,
                                         uint8_t *sw)
{
   uint8_t raw_len;

   send_cmd_header(spi, hdr);
   keep_send_aa_to_wait_for_cmd_ins_confirm(spi, hdr->ins);
   send_data_send_flag(spi);
   send_data(spi, tx, tx_len);
   keep_send_aa_to_wait_for_cmd_ins_confirm(spi, hdr->ins);
   raw_len = read_expected_1byte_data_len(spi);
   if (raw_len - 1 != rx_len ) {
      LOG_WARN("Len Mismatch, Expected %d but Actual %d", rx_len + 1, raw_len);
   }
   read_data(spi, rx, rx_len);
   read_status_word(spi, sw);
   return sw[0] << 8 | sw[1];
}

int send_cmd_data_and_recv_size2byte_data_sw_frame(spi_device_t *spi, header_t *hdr,
                                         uint8_t *tx, uint16_t tx_len,
                                         uint8_t *rx, uint16_t *p_rx_len,
                                         uint8_t *sw)
{
   uint16_t raw_len;

   send_cmd_header(spi, hdr);
   keep_send_aa_to_wait_for_cmd_ins_confirm(spi, hdr->ins);
   send_data_send_flag(spi);
   send_data(spi, tx, tx_len);
   keep_send_aa_to_wait_for_cmd_ins_confirm(spi, hdr->ins);
   raw_len = read_expected_2byte_data_len(spi);
   read_data(spi, rx, raw_len -1);
   read_status_word(spi, sw);
   *p_rx_len = raw_len -1;
   return sw[0] << 8 | sw[1];
}

void import_type_sm1_key(spi_device_t *spi, uint8_t *key, uint8_t type)
{
   header_t hdr;
   LOG_DEBUG("spi_send_import_sm1_key cmd type %d", type);
   cmd_header_fill(&hdr, CMD_CLA_CUSTOM, INS_IMPORT_SM1_KEY, type, 0x00, SM1_KEY_LENGTH);
   send_cmd_data_and_recv_sw_frame(spi, &hdr, key);
}


/* =================   public interface =========================*/
void* nrsec3000_device_create(void)
{
   return nrsec3000_spi_device_create();
}

#define KLSTM32_NRSEC3000_SPI      SPI_DEV(0)
#define KLSTM32_NRSEC3000_SPI_CS   (GPIO_PIN(PORT_G, 10))


int nrsec3000_import_sm1_key(security_device_t *dev, uint8_t *ekey)
{
   uint8_t akey[16] = {0,};
   spi_device_t *spi = (spi_device_t *)dev->backend_data;

   int tmp;
   tmp = spi_acquire(KLSTM32_NRSEC3000_SPI, KLSTM32_NRSEC3000_SPI_CS, SPI_MODE_3, SPI_CLK_10MHZ);

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

   LOG_INFO("SPI_DEV(%d) initialized: mode: %i, clk: %i\n", KLSTM32_NRSEC3000_SPI, SPI_MODE_3, SPI_CLK_10MHZ);

   gpio_clear(KLSTM32_NRSEC3000_SPI_CS);

   LOG_INFO("NRSEC3000 Try to import sm1 ekey to nrsec3000");
   import_type_sm1_key(spi, ekey, SM1_KEY_TYPE_EK);
   LOG_INFO("NRSEC3000 Try to import sm1 akey to nrsec3000");
   import_type_sm1_key(spi, akey, SM1_KEY_TYPE_AK);

   gpio_set(KLSTM32_NRSEC3000_SPI_CS);
   spi_release(KLSTM32_NRSEC3000_SPI);
   return 0;
}

int nrsec3000_import_sm1_iv(security_device_t *dev, uint8_t *iv)
{
   header_t hdr;
   spi_device_t *spi = (spi_device_t *)dev->backend_data;

   LOG_INFO("NRSEC3000 Try to import sm1 iv");
   cmd_header_fill(&hdr, CMD_CLA_CUSTOM, INS_IMPORT_SM1_KEY, 0x04, 0x00, SM1_IV_LENGTH);
   send_cmd_data_and_recv_sw_frame(spi, &hdr, iv);

   return 0;
}

int nrsec3000_sm1_encrypt(security_device_t *dev, uint8_t *in, uint16_t inlen, uint8_t *out, uint16_t *poutl)
{
   header_t hdr;
   uint8_t sw[2];
   spi_device_t *spi = (spi_device_t *)dev->backend_data;

   LOG_INFO("NRSEC3000 Try to do sm1 encrypt with len %d", inlen);
   cmd_header_fill(&hdr, CMD_CLA_SM1_CALC, INS_SM1_CRYPT, 0x80, (uint8_t)(inlen>>8), (uint8_t)inlen);

   send_cmd_data_and_recv_size2byte_data_sw_frame(spi, &hdr, in, inlen, out, poutl, sw);
   return 0;
}

int nrsec3000_sm1_decrypt(security_device_t *dev, uint8_t *in, uint16_t inlen, uint8_t *out, uint16_t *poutl)
{
   header_t hdr;
   uint8_t sw[2];
   spi_device_t *spi = (spi_device_t *)dev->backend_data;

   LOG_INFO("NRSEC3000 Try to do sm1 decrypt with len %d", inlen);
   cmd_header_fill(&hdr, CMD_CLA_SM1_CALC, INS_SM1_CRYPT, 0x81, (uint8_t)(inlen>>8), (uint8_t)inlen);

   send_cmd_data_and_recv_size2byte_data_sw_frame(spi, &hdr, in, inlen, out, poutl, sw);
   return 0;
}

int nrsec3000_generate_sm2_key(security_device_t *dev, sm2_key_idx_t idx)
{
   header_t hdr;
   spi_device_t *spi = (spi_device_t *)dev->backend_data;

   LOG_INFO("NRSEC3000 Try to generate sm2_key on idx %d", idx);
   cmd_header_fill(&hdr, CMD_CLA_CUSTOM, INS_GENERATE_SM2_KEY, 0x00, idx, 0x00);
   send_cmd_header(spi, &hdr);

   wait_for_status_word(spi);
   return 0;
}

int nrsec3000_export_sm2_key(security_device_t* dev, sm2_key_idx_t key_idx, uint8_t key_type, uint8_t *key_buf, uint8_t key_len)
{
   header_t hdr;
   uint8_t sw[2];
   spi_device_t *spi = (spi_device_t *)dev->backend_data;

   LOG_INFO("NRSEC3000 Send export_sm2_key_cmd type(%d) on idx(%d) with len(%d)", key_type, key_idx, key_len);
   cmd_header_fill(&hdr, CMD_CLA_CUSTOM, INS_EXPORT_SM2_KEY, key_type, key_idx, key_len);

   send_cmd_and_recv_data_sw_frame(spi, &hdr, key_buf, key_len, sw);
   return 0;
}

int nrsec3000_import_sm2_key(security_device_t *dev, sm2_key_idx_t key_idx, uint8_t key_type, uint8_t *key_buf, uint8_t key_len)
{
   header_t hdr;
   spi_device_t *spi = (spi_device_t*)dev->backend_data;

   LOG_INFO("NRSEC3000 try to import sm2 key with type public on idx %d", key_idx);
   cmd_header_fill(&hdr, CMD_CLA_CUSTOM, INS_IMPORT_SM2_KEY, key_type, key_idx, key_len);
   send_cmd_data_and_recv_sw_frame(spi, &hdr, key_buf);

   return 0;
}

int nrsec3000_generate_sm2_cert_request(security_device_t *dev,
                                              sm2_cert_format_t fmt,
                                              sm2_key_idx_t key_idx,
                                              char *subject,
                                              uint8_t *out, uint16_t *poutl)
{
   header_t hdr;
   uint8_t sw[2];
   size_t sub_len = strlen(subject);
   spi_device_t *spi = (spi_device_t *)dev->backend_data;

   LOG_INFO("NRSEC3000 Try to generate_sm2_cert_request fmt %d with subject(%d:%s) on idx %d", fmt, sub_len, subject, key_idx);
   cmd_header_fill(&hdr, CMD_CLA_CUSTOM, INS_GENERATE_SM2_CERT_REQ, fmt, key_idx, sub_len);

   send_cmd_data_and_recv_size2byte_data_sw_frame(spi, &hdr, (uint8_t*)subject, sub_len, out, poutl, sw);
   return 0;
}

int nrsec3000_sm3_hash(security_device_t *dev, uint8_t *in, uint16_t inlen, uint8_t *out)
{
   uint8_t sw[2];
   header_t hdr;
   spi_device_t *spi = (spi_device_t*)dev->backend_data;

   LOG_INFO("NRSEC3000 try to do sm3 hash on datlen %d with hi %.2X lo %.2X", inlen, (uint8_t)(inlen>>8), (uint8_t)inlen);
   cmd_header_fill(&hdr, CMD_CLA_CUSTOM, INS_SM3_HASH, 0x00, (uint8_t)(inlen>>8), (uint8_t)inlen);
   send_cmd_data_and_recv_size1byte_data_sw_frame(spi, &hdr,
                                        in, inlen,
                                        out, SM3_HASH_VALUE_LENGTH,
                                        sw);
   return 0;
}

int nrsec3000_sm2_encrypt(security_device_t *dev, sm2_key_idx_t idx, uint8_t *data, uint8_t *crypted)
{
   uint8_t sw[2];
   header_t hdr;
   spi_device_t *spi = (spi_device_t*)dev->backend_data;

   LOG_INFO("NRSEC3000 try to encrypt sm2 data");
   cmd_header_fill(&hdr, CMD_CLA_CUSTOM, INS_SM2_ENCRYPT, 0x01, idx, SM2_PLAIN_DATA_LENGTH);
   send_cmd_data_and_recv_size1byte_data_sw_frame(spi, &hdr,
                                        data, SM2_PLAIN_DATA_LENGTH,
                                        crypted, SM2_CIPHER_DATA_LENGTH,
                                        sw);

   return 0;
}
int nrsec3000_sm2_decrypt(security_device_t *dev, sm2_key_idx_t idx, uint8_t *crypted, uint8_t *decrypted)
{
   uint8_t sw[2];
   header_t hdr;
   spi_device_t *spi = (spi_device_t*)dev->backend_data;

   LOG_INFO("NRSEC3000 try to decrypt sm2 data");
   cmd_header_fill(&hdr, CMD_CLA_CUSTOM, INS_SM2_ENCRYPT, 0x81, idx, SM2_CIPHER_DATA_LENGTH);
   send_cmd_data_and_recv_size1byte_data_sw_frame(spi, &hdr,
                                        crypted, SM2_CIPHER_DATA_LENGTH ,
                                        decrypted, SM2_PLAIN_DATA_LENGTH,
                                        sw);

   return 0;
}

int nrsec3000_sm2_sign(security_device_t *dev, sm2_key_idx_t key_idx, uint8_t *sm3_hash, uint8_t *sm2_sign)
{
   header_t hdr;
   uint8_t sw[2];
   spi_device_t *spi = (spi_device_t*)dev->backend_data;

   LOG_INFO("Try to do sm2 sign");
   cmd_header_fill(&hdr, CMD_CLA_CUSTOM, INS_SM2_SIGN, 0x00, key_idx, SM3_HASH_VALUE_LENGTH);

   return    send_cmd_data_and_recv_size1byte_data_sw_frame(spi, &hdr,
                                                  sm3_hash, SM3_HASH_VALUE_LENGTH,
                                                  sm2_sign, SM2_SIGN_DATA_LENGTH,
                                                  sw);
}

int nrsec3000_sm2_verify(security_device_t *dev, sm2_key_idx_t key_idx, uint8_t *sm3_hash, uint8_t *sm2_sign)
{
   header_t hdr;
   uint8_t buf[0x60];
   spi_device_t *spi = (spi_device_t*)dev->backend_data;

   LOG_INFO("Try to do sm2 verify");
   cmd_header_fill(&hdr, CMD_CLA_CUSTOM, INS_SM2_VERIFY, 0x00, key_idx, SM3_HASH_VALUE_LENGTH + SM2_SIGN_DATA_LENGTH);
   memcpy(buf, sm3_hash, SM3_HASH_VALUE_LENGTH);
   memcpy(buf + SM3_HASH_VALUE_LENGTH, sm2_sign, SM2_SIGN_DATA_LENGTH);
   send_cmd_data_and_recv_sw_frame(spi, &hdr, buf);
   return 0;
}


int nrsec3000_generate_random(security_device_t *dev, uint8_t *rdm, uint8_t len)
{
   header_t hdr;
   uint8_t sw[2];
   spi_device_t *spi = (spi_device_t*)dev->backend_data;

   LOG_INFO("Try to generate %d bytes random number", len);
   cmd_header_fill(&hdr, CMD_CLA_STANDARD, INS_GENERATE_RANDOM, 0x00, 0x00, len);

   return send_cmd_and_recv_data_sw_frame(spi, &hdr, rdm, len, sw);
}

int nrsec3000_get_version(security_device_t *dev, uint8_t *version)
{
   header_t hdr;
   uint8_t sw[2];
   spi_device_t *spi = (spi_device_t*)dev->backend_data;

   LOG_INFO("NRSEC3000 Try to get version");
   cmd_header_fill(&hdr, CMD_CLA_STANDARD, INS_GET_VERSION, 0x00, 0x00, VERSION_LENGTH);

   return send_cmd_and_recv_data_sw_frame(spi, &hdr, version, VERSION_LENGTH, sw);
}

int nrsec3000_auth_cert(security_device_t *dev, uint8_t *factor, uint8_t *auth_info)
{
   header_t hdr;
   uint8_t sw[2];
   spi_device_t *spi = (spi_device_t*)dev->backend_data;

   LOG_INFO(" NRSEC3000 Try auth cert");
   cmd_header_fill(&hdr, CMD_CLA_CUSTOM, INS_SM2_ENCRYPT, 0x01, 0x04, AUTH_FACTOR_LENGTH);
   return send_cmd_data_and_recv_size1byte_data_sw_frame(spi, &hdr,
                                               factor, AUTH_FACTOR_LENGTH,
                                               auth_info, AUTH_INFO_LENGTH,
                                                     sw);
}

security_ops_t security_nrsec3000_ops = {
            .type = _SECURITY_BACKEND_TYPE_NRSEC3000,
            .create = nrsec3000_device_create,
            .import_sm1_key = nrsec3000_import_sm1_key,
            .import_sm1_iv = nrsec3000_import_sm1_iv,
            .sm1_encrypt = nrsec3000_sm1_encrypt,
            .sm1_decrypt = nrsec3000_sm1_decrypt,
            .generate_sm2_key = nrsec3000_generate_sm2_key,
            .export_sm2_key = nrsec3000_export_sm2_key,
            .import_sm2_key = nrsec3000_import_sm2_key,
            .sm3_hash = nrsec3000_sm3_hash,
            .sm2_sign = nrsec3000_sm2_sign,
            .sm2_verify = nrsec3000_sm2_verify,
            .sm2_encrypt = nrsec3000_sm2_encrypt,
            .sm2_decrypt = nrsec3000_sm2_decrypt,
            .generate_sm2_cert_request = nrsec3000_generate_sm2_cert_request,
            .generate_random = nrsec3000_generate_random,
            .get_version = nrsec3000_get_version,
            .auth_cert = nrsec3000_auth_cert,
};
