#ifndef EC20_AT_H_
#define EC20_AT_H_

#include <string.h>
#include <stdlib.h>
#include "periph/uart.h"
#include "periph/gpio.h"
#include "thread.h"
#include "fmt.h"
#include "isrpipe.h"

#define EC20_SIM_IMEI_MAX_LEN (17)

#define EC20_UART_DEV  UART_DEV(1)
#define UART_BAUDRATE (115200)
#define EC20_RX_BUFFSIZE  (1024)
#define EC20_AT_BUFFSIZE (1024)

#define LINK_BREAK 0
#define LINK_UP 1

#define EC20_RESET_PIN GPIO_PIN(PORT_B,0)

typedef struct at_cfg{
    uart_t uart;
    uint32_t baudrate;
    isrpipe_t isrpipe;
    char *isrpipe_buf;
    unsigned isrpipe_buf_size;
    uint8_t set_highest_priority;
}at_cfg_t;

typedef struct csq{
    int rssi;
    int ber;
    int q;
}csq_t;

typedef struct ec20_dev{
    uint8_t link_status;
    at_cfg_t at_cfg;
    csq_t csq;
    char imei[EC20_SIM_IMEI_MAX_LEN];
    char ip[4];
    uint16_t port;
    gpio_t reset_pin;
}ec20_dev_t;


int ec20_4g_csq(ec20_dev_t* dev);

int close_ec20_tcp(ec20_dev_t* dev);

uint8_t ec20_get_link_status(ec20_dev_t* dev);

void ec20_dev_reset(ec20_dev_t* dev);

void ec20_at_setup(ec20_dev_t* dev, at_cfg_t* p_at_cfg, gpio_t p_reset_pin);

int ec20_link_up(ec20_dev_t* dev);

int ec20_at_send(ec20_dev_t* dev, uint8_t* data, uint16_t data_len);

int ec20_at_recv(ec20_dev_t* dev, uint8_t* recv_data);

int ec20_at_close(ec20_dev_t* dev);

void test_ec20(void);

//uint16_t get_tcp_port(void);

void set_server_ip_port(char *server_ip, uint16_t port);

void break_ec20_link(void);

void get_dev_ip_port(void);

int test_at_cmd(ec20_dev_t *dev, char* at_cmd, char* out);

//void get_dev_ip_port(void);

#endif /* EC20_AT_H_ */
