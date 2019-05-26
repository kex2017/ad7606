#ifndef CFG_H_
#define CFG_H_

void cfg_set_device_id(uint16_t id);
uint16_t cfg_get_device_id(void);
void cfg_set_device_uart2_bd(uint32_t baudrate);
void cfg_set_device_version(const char *version);
void cfg_set_device_longitude(double longitude);
double cfg_get_device_longitude(void);
void cfg_set_device_latitude(double latitude);
double cfg_get_device_latitude(void);
void cfg_set_device_height(double height);
double cfg_get_device_height(void);
void load_device_cfg(void);
void update_device_cfg(void);
void print_device_cfg(void);

#endif
