/* asmhead.nas */
struct BOOTINFO { /* 0x0ff0-0x0fff */
	char cyls; /* bootinfo的起始存放位置 */
	char leds; /* boot时键盘的led状态 */
	char vmode; /* 显示模式 */
	char reserve;
	short scrnx, scrny; /* 画面分辨率 */
	char *vram;
};
#define ADR_BOOTINFO	0x00000ff0 /* bootinfo的起始存放位置 */

/* naskfunc.nas */
void io_hlt(void); // cpu 休眠
void io_cli(void); // 将中断标志置为0(clear interrupt flag)
void io_sti(void); // 将中断标志置为1(set interrupt flag)
void io_stihlt(void); // 将中断标志置为1(set interrupt flag)后休眠cpu
int io_in8(int port); // 向 cpu 之外的设备拿取数据
void io_out8(int port, int data); // 向 cpu 之外的设备传数据
int io_load_eflags(void); // 或取记录中断许可标志eflags
void io_store_eflags(int eflags); // 给记录中断许可标志eflags赋值
void load_gdtr(int limit, int addr); // 给GDTR赋值
void load_idtr(int limit, int addr); // 给IDTR赋值
int load_cr0(void);
void store_cr0(int cr0);
void asm_inthandler21(void); // 键盘中断需要调用的函数
void asm_inthandler27(void);
void asm_inthandler2c(void); // 鼠标中断需要调用的函数
unsigned int memtest_sub(unsigned int start, unsigned int end);

/* graphic.c */
void init_palette(void);
void set_palette(int start, int end, unsigned char *rgb);
void boxfill8(unsigned char *vram, int xsize, unsigned char c, int x0, int y0, int x1, int y1);
void init_screen8(char *vram, int x, int y);
void putfont8(char *vram, int xsize, int x, int y, char c, char *font);
void putfonts8_asc(char *vram, int xsize, int x, int y, char c, unsigned char *s);
void init_mouse_cursor8(char *mouse, char bc);
void putblock8_8(char *vram, int vxsize, int pxsize,
	int pysize, int px0, int py0, char *buf, int bxsize);
/*定义颜色映射*/
#define COL8_000000		0
#define COL8_FF0000		1
#define COL8_00FF00		2
#define COL8_FFFF00		3
#define COL8_0000FF		4
#define COL8_FF00FF		5
#define COL8_00FFFF		6
#define COL8_FFFFFF		7
#define COL8_C6C6C6		8
#define COL8_840000		9
#define COL8_008400		10
#define COL8_848400		11
#define COL8_000084		12
#define COL8_840084		13
#define COL8_008484		14
#define COL8_848484		15

/* dsctbl.c */
/*
段基址：base_low base_mid base_high
段上限：limit_low limit_high(低四位)
段属性：access_right limit_high(高四位)

access_right:
    00000000(0x00):未使用的记录表(descriptor table)。 
    10010010(0x92):系统专用，可读写的段。不可执行。 
    10011010(0x9a):系统专用，可执行的段。可读不可写。 
    11110010(0xf2):应用程序用，可读写的段。不可执行。 
    11111010(0xfa):应用程序用，可执行的段。可读不可写。
*/
struct SEGMENT_DESCRIPTOR {
	short limit_low, base_low;
	char base_mid, access_right;
	char limit_high, base_high;
};
/*
触发中断执行的函数地址：offset_low offset_high
触发中断执行的函数段编号：selector
触发中断执行的函数段属性：access_right dw_count
*/
struct GATE_DESCRIPTOR {
	short offset_low, selector;
	char dw_count, access_right;
	short offset_high;
};
void init_gdtidt(void); // 初始化GDT（Global Descriptor Table）和IDT（Interrupt Descriptor Table）
void set_segmdesc(struct SEGMENT_DESCRIPTOR *sd, unsigned int limit, int base, int ar);
void set_gatedesc(struct GATE_DESCRIPTOR *gd, int offset, int selector, int ar);
#define ADR_IDT			0x0026f800 // 将0x26f800~0x26ffff设为IDT
#define LIMIT_IDT		0x000007ff 
#define ADR_GDT			0x00270000 // 将0x270000~0x27ffff设为GDT
#define LIMIT_GDT		0x0000ffff
#define ADR_BOTPAK		0x00280000 // bootpack 程序的起始地址
#define LIMIT_BOTPAK	0x0007ffff
#define AR_DATA32_RW	0x4092 // 系统专用，可读写的段。不可执行。
#define AR_CODE32_ER	0x409a // 系统专用，可执行的段。可读不可写。 
#define AR_INTGATE32	0x008e // 表示这是用于中断处理的有效设定

/* int.c */
void init_pic(void);
void inthandler27(int *esp);
#define PIC0_ICW1		0x0020
#define PIC0_OCW2		0x0020
#define PIC0_IMR		0x0021
#define PIC0_ICW2		0x0021
#define PIC0_ICW3		0x0021
#define PIC0_ICW4		0x0021
#define PIC1_ICW1		0x00a0
#define PIC1_OCW2		0x00a0
#define PIC1_IMR		0x00a1
#define PIC1_ICW2		0x00a1
#define PIC1_ICW3		0x00a1
#define PIC1_ICW4		0x00a1

/* fifo.c */
/*
 buf: 	缓冲区，长度自定义
 p: 	下一个数据写入位置
 q: 	下一个数据读出位置
 free: 	缓冲区空闲长度
 flags: 
*/
struct FIFO8 {
	unsigned char *buf; 
	int p, q, size, free, flags;
};
void fifo8_init(struct FIFO8 *fifo, int size, unsigned char *buf);
int fifo8_put(struct FIFO8 *fifo, unsigned char data);
int fifo8_get(struct FIFO8 *fifo);
int fifo8_status(struct FIFO8 *fifo);

/* keyboard.c */
void init_keyboard(void); 
void inthandler21(int *esp);
void wait_KBC_sendready(void);
#define PORT_KEYCMD				0x0064
#define PORT_KEYDAT 0x0060 // 键盘的设备编号
extern struct FIFO8 keyfifo;

/* mouse.c */
struct MOUSE_DEC {
	unsigned char buf[3], phase;
	int x, y, btn;
};
extern struct FIFO8 mousefifo;
void inthandler2c(int *esp);
void enable_mouse(struct MOUSE_DEC *mdec);
int mouse_decode(struct MOUSE_DEC *mdec, unsigned char dat);

/* memory.c */
#define MEMMAN_ADDR			0x003c0000
#define MEMMAN_FREES		4090	/* 大约是32KB*/
struct FREEINFO {	/* 可用信息 */
	unsigned int addr, size;
};
/*
frees: 可用信息数目
maxfrees: 用于观察可用状况 --frees的最大值
lostsize: 释放失败的内存的大小总和
losts: 释放失败次数
*/
struct MEMMAN {		/* 内存管理 */
	int frees, maxfrees, lostsize, losts;
	struct FREEINFO free[MEMMAN_FREES];
};
unsigned int memtest(unsigned int start, unsigned int end);
void memman_init(struct MEMMAN *man);
unsigned int memman_total(struct MEMMAN *man);
unsigned int memman_alloc(struct MEMMAN *man, unsigned int size);
int memman_free(struct MEMMAN *man, unsigned int addr, unsigned int size);
