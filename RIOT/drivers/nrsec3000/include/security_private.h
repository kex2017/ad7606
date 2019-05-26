#ifndef SECURITY_PRIVATE_H_
#define SECURITY_PRIVATE_H_

typedef enum {
    _SECURITY_BACKEND_TYPE_NRSEC3000=0,
    _SECURITY_BACKEND_TYPE_MOCK
} security_bakend_type_t;

enum {
   SM1_KEY_LENGTH = 0x10,
};

enum sm1_key_type{
   SM1_KEY_TYPE_EK = 0x01,
   SM1_KEY_TYPE_AK = 0x02
};

typedef struct security_backend {
   security_bakend_type_t type;
   void* (*create)(void);

   int (*import_sm1_key) (security_device_t* dev, uint8_t *key);
   int (*import_sm1_iv) (security_device_t* dev, uint8_t *iv);
   int (*sm1_encrypt)(security_device_t *dev, uint8_t *in, uint16_t inlen, uint8_t *out, uint16_t *outl);
   int (*sm1_decrypt)(security_device_t *dev, uint8_t *in, uint16_t inlen, uint8_t *out, uint16_t *outl);

   int (*generate_sm2_key) (security_device_t* dev, sm2_key_idx_t idx);
   int (*export_sm2_key)(security_device_t* dev, sm2_key_idx_t key_idx, uint8_t key_type, uint8_t *key_buf, uint8_t key_len);
   int (*import_sm2_key)(security_device_t *dev, sm2_key_idx_t key_idx, uint8_t key_type, uint8_t *key_buf, uint8_t key_len);
   int (*sm3_hash)(security_device_t *dev, uint8_t *in, uint16_t inlen, uint8_t *out);
   int (*sm2_sign)(security_device_t *dev, sm2_key_idx_t key_idx, uint8_t *sm3_hash, uint8_t *sm2_sign);
   int (*sm2_verify)(security_device_t *dev, sm2_key_idx_t key_idx, uint8_t *sm3_hash, uint8_t *sm2_sign);
   int (*sm2_encrypt)(security_device_t *dev, sm2_key_idx_t idx, uint8_t *data, uint8_t *crypted);
   int (*sm2_decrypt)(security_device_t *dev, sm2_key_idx_t idx, uint8_t *crypted, uint8_t *decrypted);
   int (*generate_sm2_cert_request) (security_device_t *dev,
               sm2_cert_format_t fmt, sm2_key_idx_t idx, char *subject,
               uint8_t *buf, uint16_t *len);

   int (*generate_random)(security_device_t *dev, uint8_t *rdm, uint8_t len);
   int (*get_version)(security_device_t *dev, uint8_t *version);
   int (*auth_cert)(security_device_t *dev, uint8_t *fact, uint8_t *auth_info);
}security_ops_t;

struct security_device {
   const security_ops_t *sops;
   void *backend_data;
};

#endif /* SECURITY_PRIVATE_H_ */
