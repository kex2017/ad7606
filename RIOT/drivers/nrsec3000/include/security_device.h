#ifndef SECURITY_DEVICE_H_
#define SECURITY_DEVICE_H_

struct security_device;
typedef struct security_device security_device_t;

security_device_t *security_device_create(const char *dev_type);
void security_device_destroy(security_device_t *dev);
int nrsec3000_test(void);

int security_device_import_sm1_key(security_device_t *dev, uint8_t* sm1_key);
int security_import_sm1_iv(security_device_t *dev, uint8_t *iv);
int security_sm1_encrypt(security_device_t *dev, uint8_t *in, uint16_t inl, uint8_t *out, uint16_t *poutl);
int security_sm1_decrypt(security_device_t *dev, uint8_t *in, uint16_t inl, uint8_t *out, uint16_t *poutl);

typedef enum sm2_key_idx {
   SM2_INTERNAL_KEY_IDX = 0x00,
   SM2_EXTERNAL_KEY_IDX = 0x01
}sm2_key_idx_t;
int security_device_generate_sm2_key(security_device_t *dev, sm2_key_idx_t idx);

enum {
   SM2_PUBLIC_KEY_LENGTH = 0x40,
   SM2_PRIVATE_KEY_LENGTH = 0x20,
};
int security_device_export_sm2_public_key(security_device_t *dev, sm2_key_idx_t idx, uint8_t *pub_key);
int security_device_export_sm2_private_key(security_device_t *dev, sm2_key_idx_t idx, uint8_t *pri_key);
int security_device_import_sm2_public_key(security_device_t *dev, sm2_key_idx_t idx, uint8_t *pub_key);
int security_device_import_sm2_private_key(security_device_t *dev, sm2_key_idx_t idx, uint8_t *pub_key);

typedef enum sm2_cert_format {
   SM2_CERT_FORMAT_1 = 0x00,
   SM2_CERT_FORMAT_2 = 0x01
}sm2_cert_format_t;
int security_device_generate_sm2_cert_request(security_device_t *dev,
                                              sm2_cert_format_t fmt,  sm2_key_idx_t idx, char *subject,
                                              uint8_t *buf, uint16_t *plen);
int security_device_sm2_init(security_device_t *dev);

int security_device_sm3_hash (security_device_t *dev, uint8_t *in, uint16_t inlen, uint8_t *out);
int security_sm3(security_device_t *dev, uint8_t *in, int inl,uint8_t *out,
                 uint8_t *pubkey, uint8_t *pucID, int idl);
int security_device_sm2_sign(security_device_t *dev, sm2_key_idx_t idx, uint8_t *sm3_hash, uint8_t *sm2_sign);
int security_device_sm2_verify(security_device_t *dev, sm2_key_idx_t idx, uint8_t *sm3_hash, uint8_t *sm2_sign);

enum {
   SM1_IV_LENGTH = 0x10,
   SM2_PLAIN_DATA_LENGTH = 0x20,
   SM2_CIPHER_DATA_LENGTH = 0x80,
   SM2_SIGN_DATA_LENGTH = 0x40,
   SM3_HASH_VALUE_LENGTH = 0x20,
   AUTH_FACTOR_LENGTH = 0x20,
   AUTH_INFO_LENGTH = 0x92,
   VERSION_LENGTH = 0x40
};
int security_device_sm2_encrypt(security_device_t *dev, sm2_key_idx_t idx, uint8_t *data, uint8_t *cipher);
int security_device_sm2_decrypt(security_device_t *dev, sm2_key_idx_t idx, uint8_t *cipher, uint8_t *plain);

int security_device_generate_random(security_device_t *dev, uint8_t *rdm, uint8_t len);
int security_device_get_version(security_device_t *dev, uint8_t *version);
int security_device_auth_cert(security_device_t *dev, uint8_t *factor, uint8_t *auth_info);

#endif /* SECURITY_DEVICE_H_ */
