#include <stdio.h>
#include "ec20_at.h"
#include "timex.h"
#include "xtimer.h"
#include "msg.h"
#include "heart_beat.h"
#include "periph/rtt.h"

#define ENABLE_DEBUG    (1)
#include "debug.h"

static char p_ip_addr[20] = "www.klec.com.cn";
static uint16_t p_port = 6890;

//char p_ip_addr[20] = {0};//"112.244.250.234"
//static uint16_t p_port = 0;//6891

//void get_dev_ip_port(void){
//	server_info_t *server_info = get_dev_server_info();
//    p_port = server_info->port;
//    memset(p_ip_addr, 0, sizeof(p_ip_addr));
//    sprintf(p_ip_addr, "%d.%d.%d.%d", server_info->ip[0], server_info->ip[1], server_info->ip[2],server_info->ip[3]);
//    DEBUG("get server ip as:%s, server port as:%d\n", p_ip_addr, p_port);
//}

static void delay_ms(int ms)
{
	xtimer_ticks32_t last_wakeup = xtimer_now();
	xtimer_periodic_wakeup(&last_wakeup, US_PER_SEC/1000 * ms);
}

static char _rx_buf_mem[EC20_RX_BUFFSIZE];
at_cfg_t g_at_cfg = {
       .uart = EC20_UART_DEV,
       .baudrate = UART_BAUDRATE,
       .isrpipe_buf = _rx_buf_mem,
       .isrpipe_buf_size = EC20_RX_BUFFSIZE,
       .set_highest_priority = 1,
};

static void uart_send_data(ec20_dev_t *dev, char *data, uint16_t len)
{
    (void)dev;
    uart_write(dev->at_cfg.uart, (const uint8_t *)data, (size_t)len);
}

static int uart_recv_data(ec20_dev_t *dev, char *buf, uint16_t len)
{
    int ret = 0;
    ret = isrpipe_read_timeout(&dev->at_cfg.isrpipe, buf, len, 300);
//    DEBUG("receive from ec20");
//    for(uint16_t i = 0; i < len; i++){
//        DEBUG("%", buf[i]);
//    }
//    DEBUG("\n");
    return ret;
}

int test_at_cmd(ec20_dev_t *dev, char* at_cmd, char* out)
{
    uart_send_data(dev, at_cmd, strlen(at_cmd));
    delay_ms(1000);

    return uart_recv_data(dev, out, EC20_AT_BUFFSIZE);
}

static int find_str(const char* string, const char* match_str)
{
    if (strstr(string, match_str) != NULL) {
        return 1;
    }
    return 0;
}

static int exec_at_cmd_with_expect_str(ec20_dev_t* dev, char* at_cmd, char* expect_str, uint16_t retry_times,
                                       int timeout_ms)
{
    int i = 0;
    char buffer[EC20_AT_BUFFSIZE] = { 0 };
    tsrb_init(&(dev->at_cfg.isrpipe.tsrb), dev->at_cfg.isrpipe_buf, dev->at_cfg.isrpipe_buf_size);

    for (i = 0; i < retry_times; i++) {
        uart_send_data(dev, at_cmd, strlen(at_cmd));
        delay_ms(timeout_ms);
        uart_recv_data(dev, buffer, EC20_AT_BUFFSIZE);
        if (find_str(buffer, expect_str)) {
            return 1;
        }
        else {
            DEBUG("[ec20]: exec %s", at_cmd);
            DEBUG("[ec20]: failed %d times\r\n", i);
        }
    }
    return 0;
}

static int exec_at_cmd_with_expect_2str(ec20_dev_t* dev, char* at_cmd, char* expect_str1, char* expect_str2,
                                        uint16_t retry_times, int timeout_ms)
{
    int i = 0;
    char buffer[EC20_AT_BUFFSIZE] = { 0 };
    tsrb_init(&(dev->at_cfg.isrpipe.tsrb), dev->at_cfg.isrpipe_buf, dev->at_cfg.isrpipe_buf_size);

    for (i = 0; i < retry_times; i++) {
        uart_send_data(dev, at_cmd, strlen(at_cmd));
        delay_ms(timeout_ms);
        uart_recv_data(dev, buffer, EC20_AT_BUFFSIZE);
        if (find_str(buffer, expect_str1) || find_str(buffer, expect_str2)) {
            DEBUG("[ec20]: exec %s", at_cmd);
            DEBUG("[ec20]: successfully\r\n");
            return 1;
        }
        else {
            DEBUG("[ec20]: exec %s", at_cmd);
            DEBUG("[ec20]: failed %d times\r\n", i);
        }
    }
    return 0;
}

static int test_at(ec20_dev_t* dev)
{
    return exec_at_cmd_with_expect_str(dev, "AT\r\n", "OK", 10, 300);
}

static int check_ec20_creg(ec20_dev_t* dev)
{
    return exec_at_cmd_with_expect_2str(dev, "AT+CREG?\r\n", "+CREG: 0,1", "+CREG: 0,5", 10, 300);
}

static int set_ec20_deact(ec20_dev_t* dev)
{
    return exec_at_cmd_with_expect_str(dev, "AT+QIDEACT=1\r\n", "OK", 10, 300);
}

static int set_ec20_act(ec20_dev_t* dev)
{
    return exec_at_cmd_with_expect_str(dev, "AT+QIACT=1\r\n", "OK", 10, 300);
}

static int setup_ec20_tcp(ec20_dev_t* dev)
{
    char buf[EC20_AT_BUFFSIZE] = { 0 };

    snprintf(buf, EC20_AT_BUFFSIZE, "AT+QIOPEN=1,0,\"TCP\",\"%s\",%d,0,0\r\n", p_ip_addr, p_port);
    printf("AT+QIOPEN=1,0,\"TCP\",\"%s\",%d,0,0\r\n", p_ip_addr, p_port);
    return exec_at_cmd_with_expect_str(dev, buf, "+QIOPEN: 0,0", 3, 5000);
}

static int set_ec20_send_rcv_format(ec20_dev_t* dev)
{
    return exec_at_cmd_with_expect_str(dev, "at+qicfg=\"dataformat\",1,1\r\n", "OK", 10, 300);
}

static int close_ec20_echo(ec20_dev_t* dev)
{
    return exec_at_cmd_with_expect_str(dev,"ATE0\r\n", "OK", 10, 300);
}

int close_ec20_tcp(ec20_dev_t* dev)
{
    return exec_at_cmd_with_expect_str(dev, "at+qiclose=0\r\n", "OK", 10, 300);
}

int ec20_4g_csq(ec20_dev_t* dev)
{
    int rssi = 0;
    int ber = 0;
    int ret = 0;

    char *at_cmd = "AT+CSQ\r\n";
    char buffer[EC20_AT_BUFFSIZE] = { 0 };
    tsrb_init(&(dev->at_cfg.isrpipe.tsrb), dev->at_cfg.isrpipe_buf, dev->at_cfg.isrpipe_buf_size);
    uart_send_data(dev, at_cmd, strlen(at_cmd));
    delay_ms(500);
    uart_recv_data(dev, buffer, EC20_RX_BUFFSIZE);

    char delims[] = "\r\n";
    char *result = NULL;

    if (find_str(buffer, "OK")) {
        result = strtok(buffer, delims);
        while (result != NULL) {
            if (find_str(result, "+CSQ")) {
                ret = sscanf(result, "+CSQ: %d,%d", &rssi, &ber);
                if (ret == 2) {
                    DEBUG("[air202]: rssi %d, ber %d\r\n", rssi, ber);
                    dev->csq.rssi = rssi;
                    dev->csq.ber = ber;
                    dev->csq.q = 1;
                    return 1;
                }
            }
            result = strtok( NULL, delims);
        }
    }
    dev->csq.q = 0;
    return 0;
}


int ec20_4g_imei(ec20_dev_t* dev)
{
    char *at_cmd = "AT+CGSN\r\n";
    char buffer[EC20_AT_BUFFSIZE] = { 0 };
    tsrb_init(&(dev->at_cfg.isrpipe.tsrb), dev->at_cfg.isrpipe_buf, dev->at_cfg.isrpipe_buf_size);
    uart_send_data(dev, at_cmd, strlen(at_cmd));
    delay_ms(800);
    uart_recv_data(dev, buffer, EC20_RX_BUFFSIZE);

    char delims[] = "\r\n";
    char *result = NULL;

    if (find_str(buffer, "OK")) {
        result = strtok(buffer, delims);
        while (result != NULL) {
            if (!find_str(result, "OK")) {
                DEBUG("[ec20]: IMEI: %s\r\n", result);
                strncpy(dev->imei, result, EC20_SIM_IMEI_MAX_LEN);
                return 1;
            }
            result = strtok( NULL, delims);
        }
    }
    return 0;
}

static int ec20_start_4g(ec20_dev_t* dev)
{
    if (!test_at(dev)) {
        DEBUG("[ec20]: start ec20 failed (test_at)\r\n");
        return 0;
    }
    if (!close_ec20_echo(dev)) {
        DEBUG("[ec20]: start ec20 failed (close_ec20_echo)\r\n");
        return 0;
    }
    if (!close_ec20_tcp(dev)) {
        DEBUG("[ec20]: start ec20 failed (close_ec20_tcp)\r\n");
        return 0;
    }
    if (!check_ec20_creg(dev)) {
        DEBUG("[ec20]: start ec20 failed (check_ec20_creg)\r\n");
        return 0;
    }
    if (!set_ec20_deact(dev)) {
        DEBUG("[ec20]: start ec20 failed (set_ec20_deact)\r\n");
        return 0;
    }
    if (!set_ec20_act(dev)) {
        DEBUG("[ec20]: start ec20 failed (set_ec20_act)\r\n");
        return 0;
    }
    if (!setup_ec20_tcp(dev)) {
        DEBUG("[ec20]: start ec20 failed (setup_ec20_tcp)\r\n");
        return 0;
    }
    if (!set_ec20_send_rcv_format(dev)) {
        DEBUG("[ec20]: start ec20 failed (set_ec20_send_rcv_format)\r\n");
        return 0;
    }
    if (!ec20_4g_csq(dev)) {
        DEBUG("[ec20]: start ec20 failed (ec20_4g_csq)\r\n");
        return 0;
    }
    if (!ec20_4g_imei(dev)) {
        DEBUG("[ec20]: start ec20 failed (ec20_4g_imei)\r\n");
        return 0;
    }

    return 1;
}

uint8_t ec20_get_link_status(ec20_dev_t* dev)
{
    return dev->link_status;
}

void ec20_dev_reset(ec20_dev_t* dev)
{
    gpio_set(dev->reset_pin);
    delay_ms(200);
    gpio_clear(dev->reset_pin);
    delay_ms(2000);
}

void ec20_at_setup(ec20_dev_t* dev, at_cfg_t* p_at_cfg, gpio_t p_reset_pin)
{
    dev->link_status = LINK_BREAK;

    dev->reset_pin = p_reset_pin;
    gpio_init(dev->reset_pin, GPIO_OUT);
    gpio_clear(dev->reset_pin);
//    get_dev_ip_port();
    for(uint8_t i = 0; i < 4 ; i++){
    	 dev->ip[i] = p_ip_addr[i];
    }
    dev->port = p_port;

    dev->at_cfg.uart = p_at_cfg->uart;
    dev->at_cfg.isrpipe_buf = p_at_cfg->isrpipe_buf;
    dev->at_cfg.isrpipe_buf_size = p_at_cfg->isrpipe_buf_size;
    dev->at_cfg.baudrate = p_at_cfg->baudrate;
    isrpipe_init(&dev->at_cfg.isrpipe, dev->at_cfg.isrpipe_buf, dev->at_cfg.isrpipe_buf_size);

    uart_init(dev->at_cfg.uart, dev->at_cfg.baudrate, (uart_rx_cb_t) isrpipe_write_one, &dev->at_cfg.isrpipe);
    if (dev->at_cfg.set_highest_priority == 1) {
//        NVIC_SetPriority(USART2_IRQn, 0);
    }

    dev->csq.q = 0;
    dev->csq.rssi = 20;
    dev->csq.ber = 0;
}

int ec20_link_up(ec20_dev_t* dev)
{
    if (ec20_start_4g(dev)) {
        DEBUG("[ec20]: link up!\r\n");
        dev->link_status = LINK_UP;
        return 1;
    }
    else {
        DEBUG("[ec20]: link break!\r\n");
//    	init_register_info();
        dev->link_status = LINK_BREAK;
        return 0;
    }
}

int ec20_at_send(ec20_dev_t* dev, uint8_t* data, uint16_t data_len)
{
    int ret = 0;
    char buf[EC20_AT_BUFFSIZE] = { 0 };
    char out_char_buf[512] = {0};

    fmt_bytes_hex(out_char_buf, data, data_len);
    buf[data_len*2] = '\0';
    snprintf(buf, EC20_AT_BUFFSIZE, "at+qisendex=0,\"%s\"\r\n", out_char_buf);
    ret = exec_at_cmd_with_expect_str(dev, buf, "SEND OK", 3, 700);
    if(!ret)
    {
    	 dev->link_status = LINK_BREAK;
    	 return -1;
    }
    return 0;
}

int ec20_at_recv(ec20_dev_t* dev, uint8_t* recv_data)
{
    char recv_buf[EC20_AT_BUFFSIZE] = { 0 };
    int rd_data_len = 0;
    uint8_t ret = 0;
    char *rd_at_cmd = "at+qird=0,512\r\n";

    tsrb_init(&(dev->at_cfg.isrpipe.tsrb), dev->at_cfg.isrpipe_buf, dev->at_cfg.isrpipe_buf_size);
    uart_send_data(dev, rd_at_cmd, strlen(rd_at_cmd));
    delay_ms(700);
    uart_recv_data(dev, recv_buf, EC20_AT_BUFFSIZE);
    char delims[] = "\r\n";
    char *result = NULL;

    result = strtok(recv_buf, delims);

    while (result != NULL) {
        if (find_str(result, "+QIRD:")) {
            ret = sscanf(result, "+QIRD: %d", &rd_data_len);
            if(ret != 1){
                DEBUG("ret is %d\n",ret);
                return 0;
            }
        }
        result = strtok(NULL, delims);
        if (rd_data_len) {
            fmt_hex_bytes(recv_data, result);
            break;
        }
    }
    return rd_data_len;
}

int ec20_at_close(ec20_dev_t* dev)
{
    if (!close_ec20_tcp(dev)) {
        DEBUG("[ec20]: start ec20 failed (close_ec20_tcp)\r\n");
        return 0;
    }
    if (!set_ec20_deact(dev)) {
        DEBUG("[ec20]: start ec20 failed (set_ec20_deact)\r\n");
        return 0;
    }

    return 1;
}
