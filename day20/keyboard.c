#include "bootpack.h"

struct FIFO32 *keyfifo;
int keydata0;

void inthandler21(int *esp)
/* 来自PS/2键盘的中断 */
{
	int data;
	/* 通知PIC，说IRQ-01的受理已经完成 */
	io_out8(PIC0_OCW2, 0x61);
	data = io_in8(PORT_KEYDAT);

	fifo32_put(keyfifo, data + keydata0);
	
	return;
}

#define PORT_KEYSTA				0x0064

#define KEYSTA_SEND_NOTREADY	0x02
#define KEYCMD_WRITE_MODE		0x60 
#define KBC_MODE 				0x47 // 鼠标模式的模式号码

void wait_KBC_sendready(void)
{
	for(;;) {
		/* 等待键盘控制电路准备完毕 */
		// CPU从设备号码0x0064处所读取的数据的倒数第二位(从低位开始数的第二位)为0准备完毕。
		if((io_in8(PORT_KEYSTA) & KEYSTA_SEND_NOTREADY) == 0) {
			break;
		}
	}
	return;
}

/* 初始化键盘控制电路 */
void init_keyboard(struct FIFO32 *fifo, int data0) 
{
	keyfifo = fifo;
	keydata0 = data0;

	wait_KBC_sendready();
	io_out8(PORT_KEYCMD, KEYCMD_WRITE_MODE);
	wait_KBC_sendready();
	// 模式设定
	io_out8(PORT_KEYDAT, KBC_MODE);
	return;
}