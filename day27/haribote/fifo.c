#include "bootpack.h"

#define FLAGS_OVERRUN		0x0001

/* 初始化FIFO缓冲区 */
void fifo32_init(struct FIFO32 *fifo, int size, int *buf, struct TASK *task) 
{
    fifo->size = size;
    fifo->buf = buf;
    fifo->free = size;
    fifo->flags = 0; 
    fifo->p = 0; /* 下一个数据写入位置 */
    fifo->q = 0; /* 下一个数据读出位置 */
    fifo->task = task; /* 有数据写入时需要唤醒的任务 */

    return;
}

/* 向FIFO传送数据并保存 */
int fifo32_put(struct FIFO32 *fifo, int data)
{
    if (fifo->free == 0) {
        /* 空余没有了，溢出 */ 
        fifo->flags |= FLAGS_OVERRUN; 
        return -1;
    }
    fifo->buf[fifo->p] = data;
    fifo->p = (fifo->p+1) % fifo->size;
    fifo->free--;

    if (fifo->task != 0) {
		if (fifo->task->flags != 2) { /* 如果任务处于休眠状态 */
			task_run(fifo->task, -1, 0); /* 将任务唤醒 */
		}
	}
    return 0;
}

/* 从FIFO取得一个数据 */
int fifo32_get(struct FIFO32 *fifo)
{
    int data;
    /* 如果缓冲区为空，则返回 -1 */
    if (fifo->free == fifo->size) {
        return -1;
    }

    data = fifo->buf[fifo->q];
    fifo->q = (fifo->q+1) % fifo->size;
    fifo->free++;

    return data;
} 

/* 报告一下到底积攒了多少数据 */
int fifo32_status(struct FIFO32 *fifo) 
{
    return fifo->size - fifo->free;
}