#include <string.h>
#include <stdio.h>

#include "x_delay.h"
#include "x_queue.h"

#define  QUEUE_DELAY    delay_ms
//  __disable_irq();
//  __enable_irq();

#define lock_ring_buf(pRingBuf)      //__disable_irq()
#define unlock_ring_buf(pRingBuf)   //__enable_irq()


void init_ring_buf(ring_buf_t* pRingBuf)
{
	pRingBuf->rear = pRingBuf->front=0;
	pRingBuf->lock = 0;
}

int get_current_ring_buf_data_len(ring_buf_t* pRingBuf)
{
   return (pRingBuf->rear - pRingBuf->front + MAX_RING_BUF_LEN) % MAX_RING_BUF_LEN;
}

void push_ring_buf(ring_buf_t* pRingBuf, unsigned char em)
{
   lock_ring_buf(pRingBuf);
   if ((pRingBuf->rear + 1) % MAX_RING_BUF_LEN == pRingBuf->front) {
	   pRingBuf->front = (pRingBuf->front + 1) % MAX_RING_BUF_LEN;
   }

   pRingBuf->rear = (pRingBuf->rear + 1) % MAX_RING_BUF_LEN;
   pRingBuf->r_buf[pRingBuf->rear] = em;
   unlock_ring_buf(pRingBuf);
}

void clean_ring_buf(ring_buf_t* pRingBuf)
{
   lock_ring_buf(pRingBuf);
   pRingBuf->rear = pRingBuf->front = 0;
   memset((void *)pRingBuf->r_buf, 0, sizeof(pRingBuf->r_buf));
   unlock_ring_buf(pRingBuf);
}

int pop_stack(circular_stack_t* pStack, unsigned char *q)
{
   int ret = 0;

   lock_ring_buf(pStack);
   if (pStack->front == pStack->rear) {
      ret = 0;
   }
   else {
      *q = pStack->r_buf[pStack->rear];
      if(pStack->rear == 0){
    	  pStack->rear =  MAX_RING_BUF_LEN - 1;
      }else{
    	  pStack->rear -= 1;
      }
      ret = 1;
   }

   unlock_ring_buf(pQueue);
   return ret;
}

int pop_queue(circular_queue_t* pQueue, unsigned char *q)
{
   int ret = 0;

   lock_ring_buf(pQueue);
   if (pQueue->front == pQueue->rear) {
      ret = 0;
   }
   else {
      pQueue->front = (pQueue->front + 1) % MAX_RING_BUF_LEN;
      *q = pQueue->r_buf[pQueue->front];
      pQueue->r_buf[pQueue->front] = 0;
      ret = 1;
   }

   unlock_ring_buf(pQueue);
   return ret;
}

int read_queue2array_timeout(circular_queue_t* pQueue, unsigned char* buf, int len, unsigned int timeout)
{
   int i = 0, j = 0, n;
   uint32_t timestamp = 0;

   while (timestamp <= timeout && i < len) {
      if ((n = get_current_queue_data_len(pQueue)) > 0) {
         n = n > len ? len : n;
         for (j = 0; i < len && j < n; j++, i++) {
            if (pop_queue(pQueue, buf)) {
               buf++;
            }
         }
      }
      QUEUE_DELAY(timeout == 0 ? 0 : 1);
      timestamp += 1;
   }

   return i;
}
/********************************************************************************************************************************
Try to read "len" bytes data from queue in "timeout" milliseconds from "pQueue_src" to "pQueue_dst"
********************************************************************************************************************************/
int read_queue2queue_timeout(circular_queue_t* pQueue_src, circular_queue_t* pQueue_dst, int len, unsigned int timeout)
{
   int index = 0;
   int data_len = 0;
   int queue_length = 0;
   unsigned char tmp;
   uint32_t times = 0;

   while (times <= timeout && data_len < len) {
      if ((queue_length = get_current_queue_data_len(pQueue_src)) > 0) {
         queue_length = queue_length > len ? len : queue_length;
         for (index = 0; data_len < len && index < queue_length; index++, data_len++) {
            if (pop_queue(pQueue_src, &tmp)) {
               add_queue(pQueue_dst, tmp);
            }
         }
      }
      QUEUE_DELAY(timeout == 0 ? 0 : 1);
      times += 1;
   }

   return data_len;
}

/********************************************************************************************************************
 *        copy_queue2array()
 * copy the data from pQueue to dst begin with position for len items, and don't change anything else
 ********************************************************************************************************************/
void copy_queue2array(circular_queue_t* pQueue, unsigned int start_position, unsigned char* dst, unsigned int len)
{
   int extra_len = 0;

   if (start_position + len <= MAX_RING_BUF_LEN) {
      memcpy(dst, pQueue->r_buf + start_position , len);
      return;
   }

   extra_len = start_position + len - MAX_RING_BUF_LEN;
   memcpy(dst, pQueue->r_buf + start_position, MAX_RING_BUF_LEN - start_position);
   memcpy(dst + (MAX_RING_BUF_LEN - start_position), pQueue->r_buf, extra_len);
}

unsigned char get_queue_item_n(circular_queue_t* pQueue_src, unsigned short n)
{
   return pQueue_src->r_buf[(pQueue_src->front + n + 1) % MAX_RING_BUF_LEN];
}

void queue_dump(circular_queue_t* pQueue_src)
{
   int i, len = get_current_queue_data_len(pQueue_src);
   printf("Queue dump begin: len(%02XH)\r\n", len);
   for (i = 0; i < len; i++) {
      printf("%02X ", get_queue_item_n(pQueue_src, i) & 0xFF);
   }
   printf("\r\nQueue dump end\r\n");
}

void queue_dump_ascii(circular_queue_t* pQueue_src)
{
   int i, len = get_current_queue_data_len(pQueue_src);
   printf("Queue dump ascii begin\r\n");
   for (i = 0; i < len; i++) {
      printf("%c", get_queue_item_n(pQueue_src, i) & 0xFF);
   }
   printf("\r\nQueue dump ascii end\r\n");
}
