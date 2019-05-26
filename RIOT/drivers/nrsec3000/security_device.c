#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#include "security_device.h"
#include "security_private.h"
#include <string.h>
#include "log.h"

extern security_ops_t security_nrsec3000_ops;

security_device_t g_security_dev;
security_device_t *security_device_create(const char *type)
{
   security_device_t *dev = 0;

   LOG_INFO("Try to create security device with type '%s'", type);
   dev = &g_security_dev;
   if (!strncmp(type, "nrsec3000", strlen(type))) {
      dev->sops = &security_nrsec3000_ops;
   }
   dev->backend_data = dev->sops->create();
   return dev;
}


int security_device_import_sm1_key(security_device_t *dev, uint8_t* sm1_key)
{
   LOG_INFO("Try to import sm1 key");
   return dev->sops->import_sm1_key(dev, sm1_key);
}

int security_import_sm1_iv(security_device_t *dev, uint8_t *iv)
{
   LOG_INFO("Try to import sm1 IV");
   return dev->sops->import_sm1_iv(dev, iv);
}

int security_sm1_encrypt(security_device_t *dev, uint8_t *in, uint16_t inl, uint8_t *out, uint16_t *poutl)
{
   LOG_INFO("Try to encrypt data with sm1 algo");
   if (inl % 16 != 0) {
      LOG_WARN("The input length(%d) is INVALID. it is not multi-16", inl);
      return -1;
   }
   return dev->sops->sm1_encrypt(dev, in, inl, out, poutl);
}

int security_sm1_decrypt(security_device_t *dev, uint8_t *in, uint16_t inl, uint8_t *out, uint16_t *poutl)
{
   LOG_INFO("Try to decrypt data with sm1 algo");
   /*TODO check has to be multi-16 or not*/
   if (inl % 16 != 0) {
      LOG_WARN("The input length(%d) is INVALID. it is not multi-16", inl);
      return -1;
   }
   return dev->sops->sm1_decrypt(dev, in, inl, out, poutl);
}

int security_device_generate_sm2_key(security_device_t *dev, sm2_key_idx_t idx)
{
   LOG_INFO("Try to generate sm2 key");
   return dev->sops->generate_sm2_key(dev, idx);
}

enum {
   SM2_KEY_TYPE_PUBLIC = 0x01,
   SM2_KEY_TYPE_PRIVATE = 0x02,
};
int security_device_export_sm2_public_key(security_device_t *dev, sm2_key_idx_t idx, uint8_t *key_buf)
{
   LOG_INFO("Try to export sm2 public key of idx(%d)");
   return dev->sops->export_sm2_key(dev, idx, SM2_KEY_TYPE_PUBLIC, key_buf, SM2_PUBLIC_KEY_LENGTH);
}
int security_device_export_sm2_private_key(security_device_t *dev, sm2_key_idx_t idx, uint8_t *key_buf)
{
   LOG_INFO("Try to export sm2 private key of idx(%d)");
   return dev->sops->export_sm2_key(dev, idx, SM2_KEY_TYPE_PRIVATE, key_buf, SM2_PRIVATE_KEY_LENGTH);
}

int security_device_import_sm2_public_key(security_device_t *dev, sm2_key_idx_t idx, uint8_t *pub_key)
{
   LOG_INFO("Try to import server sm2 public key on idx(%d)", idx);
   return dev->sops->import_sm2_key(dev, idx, SM2_KEY_TYPE_PUBLIC, pub_key, SM2_PUBLIC_KEY_LENGTH);
}

int security_device_import_sm2_private_key(security_device_t *dev, sm2_key_idx_t idx, uint8_t *pub_key)
{
   LOG_INFO("Try to import server sm2 private key on idx(%d)", idx);
   return dev->sops->import_sm2_key(dev, idx, SM2_KEY_TYPE_PRIVATE, pub_key, SM2_PRIVATE_KEY_LENGTH);
}

int security_device_generate_sm2_cert_request(security_device_t *dev,
                                              sm2_cert_format_t fmt, sm2_key_idx_t idx, char *subject,
                                              uint8_t *buf, uint16_t *plen)
{
   LOG_INFO("Try to  generate_sm2_cert");
   return dev->sops->generate_sm2_cert_request(dev, fmt, idx, subject, buf, plen);

}
int security_device_sm2_init(security_device_t *dev)
{
   uint8_t server_sm2_ECCrefPublicKey[SM2_PUBLIC_KEY_LENGTH + 4];

   uint8_t sm2_pub_key[SM2_PUBLIC_KEY_LENGTH] = {
               0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15,
               16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31,
               32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47,
               48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63
   };

   uint8_t sm2_pri_key[SM2_PRIVATE_KEY_LENGTH] = {
               0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15,
               16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31,
   };


   uint16_t sm2_cert_len1, sm2_cert_len2;
   uint8_t sm2_fmt1_cert_request[65536], sm2_fmt2_cert_request[65536];
   char *subject = "C=CN,ST=zhejiang,L=hangzhou,O=GDD,OU=kelin,CN=test007";

   LOG_INFO("Try to init sm2 device");
   LOG_INFO("step1.0: Try to generate sm2 key on idx(%d)", SM2_INTERNAL_KEY_IDX);
   security_device_generate_sm2_key(dev, SM2_INTERNAL_KEY_IDX);

   LOG_INFO("step1.1: Try to generate sm2 key on idx(%d)", SM2_EXTERNAL_KEY_IDX);
   security_device_generate_sm2_key(dev, SM2_EXTERNAL_KEY_IDX);

   LOG_INFO("step2: Try to export sm2 key on idx(%d)", SM2_INTERNAL_KEY_IDX);
   sleep(1);
   security_device_export_sm2_public_key(dev, SM2_INTERNAL_KEY_IDX, sm2_pub_key);

   sleep(1);
   security_device_export_sm2_private_key(dev, SM2_INTERNAL_KEY_IDX, sm2_pri_key);

   sleep(1);
   LOG_INFO("step3.1: Try to generate sm2 cert request for fmt %d on idx(%d)", SM2_CERT_FORMAT_1, SM2_INTERNAL_KEY_IDX);
   security_device_generate_sm2_cert_request(dev, SM2_CERT_FORMAT_1, SM2_INTERNAL_KEY_IDX, subject, sm2_fmt1_cert_request, &sm2_cert_len1);
   FILE *fp = fopen("sm2_cert_request_fmt1.cert", "w");
   fwrite(sm2_fmt1_cert_request, sm2_cert_len1, 1, fp);
   fclose(fp);

   LOG_INFO("step3.2: Try to generate sm2 cert request for fmt %d on idx(%d)", SM2_CERT_FORMAT_2, SM2_INTERNAL_KEY_IDX);
   security_device_generate_sm2_cert_request(dev, SM2_CERT_FORMAT_2, SM2_INTERNAL_KEY_IDX, subject, sm2_fmt2_cert_request, &sm2_cert_len2);
   fp = fopen("sm2_cert_request_fmt2.cert", "w");
   fwrite(sm2_fmt2_cert_request, sm2_cert_len2, 1, fp);
   fclose(fp);

   fp = fopen("pubkey.bin", "r");
   fread(server_sm2_ECCrefPublicKey, 68, 1, fp);
   fclose(fp);
   security_device_import_sm2_public_key(dev, SM2_EXTERNAL_KEY_IDX, server_sm2_ECCrefPublicKey + 4);

   return 0;
}


int security_device_sm2_encrypt(security_device_t *dev, sm2_key_idx_t idx, uint8_t *data, uint8_t *crypted)
{
   LOG_INFO("Try to encrypt sm2 data using idx(%d) key", idx);
   return dev->sops->sm2_encrypt(dev, idx, data, crypted);
}

int security_device_sm2_decrypt(security_device_t *dev, sm2_key_idx_t idx, uint8_t *cipher, uint8_t *plain)
{
   LOG_INFO("Try to decrypt sm2 data using idx(%d) key", idx);
   return dev->sops->sm2_decrypt(dev, idx, cipher, plain);
}

int security_device_sm3_hash (security_device_t *dev, uint8_t *in, uint16_t inlen, uint8_t *out)
{
   LOG_INFO("Try to do sm3 hash algo, datlen is %d", inlen);
   return dev->sops->sm3_hash(dev, in, inlen, out);
}

int security_sm3(security_device_t *dev, uint8_t *in, int inl,uint8_t *out,
                 uint8_t *pubkey, uint8_t *pucID, int idl)
{
   LOG_INFO("Try to do sm3 hash, datlen is %d with pucId len %d", inl, idl);
   int nRet, l;
   uint8_t *Z = NULL;
   int entl = 0;
   uint8_t tmpm[32];

   uint8_t abxy[32 * 4] = {
               0xFF, 0xFF, 0xFF, 0xFE, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, /* a */
               0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
               0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
               0xFF, 0xFC,
               0x28, 0xE9, 0xFA, 0x9E, 0x9D, 0x9F, 0x5E, 0x34, 0x4D, 0x5A, /* b */
               0x9E, 0x4B, 0xCF, 0x65, 0x09, 0xA7, 0xF3, 0x97, 0x89, 0xF5,
               0x15, 0xAB, 0x8F, 0x92, 0xDD, 0xBC, 0xBD, 0x41, 0x4D, 0x94,
               0x0E, 0x93,
               0x32, 0xC4, 0xAE, 0x2C, 0x1F, 0x19, 0x81, 0x19, 0x5F, 0x99, /* x */
               0x04, 0x46, 0x6A, 0x39, 0xC9, 0x94, 0x8F, 0xE3, 0x0B, 0xBF,
               0xF2, 0x66, 0x0B, 0xE1, 0x71, 0x5A, 0x45, 0x89, 0x33, 0x4C,
               0x74, 0xC7,
               0xBC, 0x37, 0x36, 0xA2, 0xF4, 0xF6, 0x77, 0x9C, 0x59, 0xBD, /* y */
               0xCE, 0xE3, 0x6B, 0x69, 0x21, 0x53, 0xD0, 0xA9, 0x87, 0x7C,
               0xC6, 0x2A, 0x47, 0x40, 0x02, 0xDF, 0x32, 0xE5, 0x21, 0x39,
               0xF0, 0xA0
   };

   l = 2 + idl + 32 * 6;
   Z = (uint8_t *)malloc(l);
   if (!Z) return -1;

   entl = idl * 8;
   memset(Z + 1, entl & 0xFF, 1);
   entl >>= 8;
   memset(Z, entl & 0xFF, 1);
   memcpy(Z + 2, pucID, idl);
   memcpy(Z + 2 + idl, abxy, 32 *4);
   memcpy(Z + 2 + idl + 4 * 32, pubkey, 32);
   memcpy(Z + 2 + idl + 5 * 32, pubkey+32, 32);


   nRet = security_device_sm3_hash(dev, Z, l, tmpm);
   if (nRet != 0) goto quit;

   free(Z);
   l = inl + 32;
   Z = (unsigned char *)malloc(l);
   if (!Z) {
      nRet = -1;
      goto quit;
   }
   memcpy(Z, tmpm, 32);
   memcpy(Z + 32, in, inl);
   nRet = security_device_sm3_hash(dev, Z, l, out);

quit:
   if (Z) free(Z);
   return nRet;
}

int security_device_sm2_sign(security_device_t *dev, sm2_key_idx_t idx, uint8_t *sm3_hash, uint8_t *sm2_sign)
{
   LOG_INFO("Try to do sm2 sign");
   return dev->sops->sm2_sign(dev, idx, sm3_hash, sm2_sign);
}

int security_device_sm2_verify(security_device_t *dev, sm2_key_idx_t idx, uint8_t *sm3_hash, uint8_t *sm2_sign)
{
   LOG_INFO("Try to do sm2 verify");
   return dev->sops->sm2_verify(dev, idx, sm3_hash, sm2_sign);
}

int security_device_generate_random(security_device_t *dev, uint8_t *rdm, uint8_t len)
{
   LOG_INFO("Try to generate %d bytes random", len);
   return dev->sops->generate_random(dev, rdm, len);
}

int security_device_get_version(security_device_t *dev, uint8_t *version)
{
   LOG_INFO("Try to get version info");
   return dev->sops->get_version(dev, version);
}

int security_device_auth_cert(security_device_t *dev, uint8_t *factor, uint8_t *auth_info)
{
   LOG_INFO("Try to verify certificate");
   return dev->sops->auth_cert(dev, factor, auth_info);
}
