#ifndef __X_QUEUE_H__
#define __X_QUEUE_H__

#define MAX_RING_BUF_LEN 2048

#define init_queue  	init_ring_buf
#define get_current_queue_data_len  	get_current_ring_buf_data_len
#define add_queue  		push_ring_buf
#define clean_queue  	clean_ring_buf

#define init_stack  		init_ring_buf
#define get_current_stack_data_len  	get_current_ring_buf_data_len
#define push_stack  		push_ring_buf
#define clean_stack  		clean_ring_buf

/**************************************************************************************************
 * There are something you MUST know before this circular queue used in your codes!
 * the front of queue is always empty, but the (front + 1) is the first element in this queue,
 * the rear is always the last element in this queue.
 * And the actual max length of this queue is (MAX_RING_BUF_LEN - 1).
 **************************************************************************************************/
typedef struct ring_buf_stu{
	unsigned char r_buf[MAX_RING_BUF_LEN];
	int front;
	int rear;
	int lock;
}ring_buf_t;
typedef struct ring_buf_stu  circular_queue_t;
typedef struct ring_buf_stu  circular_stack_t;

void init_ring_buf(ring_buf_t* pRingBuf);
int get_current_ring_buf_data_len(ring_buf_t* pRingBuf);
void push_ring_buf(ring_buf_t* pRingBuf, unsigned char em);
void clean_ring_buf(ring_buf_t* pRingBuf);

int pop_stack(circular_stack_t* pStack, unsigned char *q);

int pop_queue(circular_queue_t* pQueue, unsigned char *q);
//both of the following function will delete the item after read
int read_queue2array_timeout(circular_queue_t* pQueue, unsigned char* buf, int len, unsigned int timeout);
int read_queue2queue_timeout(circular_queue_t* pQueue_src, circular_queue_t* pQueue_dst, int len, unsigned int timeout);

//only copy the items from source queue, and don't change anything
void copy_queue2array(circular_queue_t* pQueue, unsigned int start_position, unsigned char* dst, unsigned int len);

unsigned char get_queue_item_n(circular_queue_t* pQueue_src,unsigned short n);
void queue_dump(circular_queue_t* pQueue_src);
void queue_dump_ascii(circular_queue_t* pQueue_src);

#endif
