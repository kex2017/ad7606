#include "comm.h"

#include "periph/uart.h"
#include "isrpipe.h"

#include "x_queue.h"

#define UART_COMMUNICATION_DEV           UART_DEV(1)
#define COMMUNICATION_RX_BUFSIZE    (4096)

static char _rx_buf_mem[COMMUNICATION_RX_BUFSIZE];
isrpipe_t uart_communication_isrpipe = ISRPIPE_INIT(_rx_buf_mem);

static uint32_t UART_COMMUNICATION_BAUDRATE = 38400;

void comm_init(void)
{
   NVIC_SetPriority(USART1_IRQn,0);
   uart_init(UART_COMMUNICATION_DEV, UART_COMMUNICATION_BAUDRATE, (uart_rx_cb_t) isrpipe_write_one, &uart_communication_isrpipe);
}

void comm_send_uplink_request(uint8_t *outbuf, uint32_t len)
{
    for (uint32_t i = 0; i < len; i++) {
        isrpipe_write_one(&uart_communication_isrpipe, outbuf[i]);
    }
}

void add_data_to_queue(circular_queue_t *data_queue, char *buf, int len)
{
	int index = 0;

	if(len > 0)
	{
		for(index = 0; index < len; index++)
		{
			add_queue(data_queue, buf[index]);
		}
	}
}

int comm_get_data(circular_queue_t* data_queue)
{
    int ret = 0;
    char buf[128] = { 0 };
    ret = isrpipe_read_timeout(&uart_communication_isrpipe, buf, 128, 1);
    add_data_to_queue(data_queue, buf, ret);
    return ret;
}

void comm_send_data(uint8_t *data, uint16_t len)
{
   uart_write(UART_COMMUNICATION_DEV, (const uint8_t *)data, (size_t)len);
}









