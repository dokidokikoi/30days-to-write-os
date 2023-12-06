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
#define ADR_DISKIMG		0x00100000

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
void load_tr(int tr);
void asm_inthandler0c(void);
void asm_inthandler0d(void);
void asm_inthandler20(void);
void asm_inthandler21(void); // 键盘中断需要调用的函数
void asm_inthandler27(void);
void asm_inthandler2c(void); // 鼠标中断需要调用的函数
unsigned int memtest_sub(unsigned int start, unsigned int end);
void farjmp(int eip, int cs);
void farcall(int eip, int cs);
void asm_hrb_api(void);
void start_app(int eip, int cs, int esp, int ds, int *tss_esp0);
void asm_end_app(void);

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
#define AR_TSS32		0x0089

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
struct FIFO32 {
	int *buf;
	int p, q, size, free, flags;
	struct TASK *task;
};
void fifo32_init(struct FIFO32 *fifo, int size, int *buf, struct TASK *task);
int fifo32_put(struct FIFO32 *fifo, int data);
int fifo32_get(struct FIFO32 *fifo);
int fifo32_status(struct FIFO32 *fifo);

/* keyboard.c */
void init_keyboard(struct FIFO32 *fifo, int data0);
void inthandler21(int *esp);
void wait_KBC_sendready(void);
#define PORT_KEYCMD				0x0064
#define PORT_KEYDAT 0x0060 // 键盘的设备编号

/* mouse.c */
struct MOUSE_DEC {
	unsigned char buf[3], phase;
	int x, y, btn;
};
void inthandler2c(int *esp);
void enable_mouse(struct FIFO32 *fifo, int data0, struct MOUSE_DEC *mdec);
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
unsigned int memman_alloc_4k(struct MEMMAN *man, unsigned int size);
int memman_free_4k(struct MEMMAN *man, unsigned int addr, unsigned int size);

/* sheet.c */
#define MAX_SHEETS		256
/*
buf: 图像内容
bxsize, bysize: 窗口大小
vx0, vy0: 窗口在界面上的位置，v 是VRAM的略语
col_inv: 透明色色号，它是color(颜色)和invisible(透明)的组合略语
height: 图层高度
flags: 用于存放有关图层的各种设定信息
*/
struct SHEET {
	unsigned char *buf;
	int bxsize, bysize, vx0, vy0, col_inv, height, flags;
	struct SHTCTL *ctl;
	struct TASK *task;
};
/*
xsize, ysize: 画面分辨率
top: 最上面图层的高度
sheets0: 存放准备的256个图层的信息
sheets: 记忆地址变量。由于sheets0中的图层顺序混乱，所以把它们按照高度进行升序排列，然后将其地址写入sheets中
*/
struct SHTCTL {
	unsigned char *vram, *map;
	int xsize, ysize, top;
	struct SHEET *sheets[MAX_SHEETS];
	struct SHEET sheets0[MAX_SHEETS];
};
struct SHTCTL *shtctl_init(struct MEMMAN *memman, unsigned char *vram, int xsize, int ysize);
struct SHEET *sheet_alloc(struct SHTCTL *ctl);
void sheet_setbuf(struct SHEET *sht, unsigned char *buf, int xsize, int ysize, int col_inv);
void sheet_updown(struct SHEET *sht, int height);
void sheet_refresh(struct SHEET *sht, int bx0, int by0, int bx1, int by1);
void sheet_slide(struct SHEET *sht, int vx0, int vy0);
void sheet_free(struct SHEET *sht);

/* timer.c */
#define MAX_TIMER 	500
struct TIMER {
	struct TIMER *next;
	unsigned int timeout;
	char flags, flags2;
	struct FIFO32 *fifo;
	int data;
};
struct TIMERCTL {
	unsigned int count, next, using;
	struct TIMER *t0;
	struct TIMER timers0[MAX_TIMER];
};
extern struct TIMERCTL timerctl;
void init_pit(void);
void inthandler20(int *esp);
struct TIMER *timer_alloc(void);
void timer_free(struct TIMER *timer);
void timer_init(struct TIMER *timer, struct FIFO32 *fifo, int data);
void timer_settime(struct TIMER *timer, unsigned int timeout);
int timer_cancel(struct TIMER *timer);
void timer_cancelall(struct FIFO32 *fifo);

/* mtask.c */
#define MAX_TASKS		1000	/* 最大任务数量 */
#define TASK_GDT0		3		/* 定义从GDT的几号开始分配给TSS */
#define MAX_TASKS_LV	100
#define MAX_TASKLEVELS	10
struct TSS32 {
	int backlink, esp0, ss0, esp1, ss1, esp2, ss2, cr3;
	int eip, eflags, eax, ecx, edx, ebx, esp, ebp, esi, edi;
	int es, cs, ss, ds, fs, gs;
	int ldtr, iomap;
};
struct TASK {
	int sel, flags; /* sel用来存放GDT的编号 */
	int level, priority; /* 优先级 */
	struct FIFO32 fifo;
	struct TSS32 tss;
	struct CONSOLE *cons;
	int ds_base, cons_stack;
};
struct TASKLEVEL {
	int running; /* 正在运行的任务数量 */
	int now; /* 这个变量用来记录当前正在运行的是哪个任务 */
	struct TASK *tasks[MAX_TASKS_LV];
};
struct TASKCTL {
	int now_lv; /*现在活动中的LEVEL */
	char lv_change; /*在下次任务切换时是否需要改变LEVEL */
	struct TASKLEVEL level[MAX_TASKLEVELS];
	struct TASK tasks0[MAX_TASKS];
};
extern struct TASKCTL *taskctl;
extern struct TIMER *task_timer;
struct TASK *task_init(struct MEMMAN *memman);
struct TASK *task_alloc(void);
void task_run(struct TASK *task, int level, int priority);
void task_switch(void);
void task_sleep(struct TASK *task);

/* window.c */
void make_window8(unsigned char *buf, int xsize, int ysize, char *title, char act);
void make_wtitle8(unsigned char *buf, int xsize, char *title, char act);
void putfonts8_asc_sht(struct SHEET *sht, int x, int y, int c, int b, char *s, int l);
void make_textbox8(struct SHEET *sht, int x0, int y0, int sx, int sy, int c);
void change_wtitle8(struct SHEET *sht, char act);

/* file.c */
struct FILEINFO {
	unsigned char name[8], ext[3], type;
	char reserve[10];
	unsigned short time, date, clustno;
	unsigned int size;
};
void file_readfat(int *fat, unsigned char *img);
void file_loadfile(int clustno, int size, char *buf, int *fat, char *img);

/* console.c */
struct CONSOLE {
	struct SHEET *sht;
	int cur_x, cur_y, cur_c;
	struct TIMER *timer;
};
void console_task(struct SHEET *sheet, unsigned int memtotal);
void cons_newline(struct CONSOLE *cons);
void cons_runcmd(char *cmdline, struct CONSOLE *cons, int *fat, unsigned int memtotal);
void cmd_mem(struct CONSOLE *cons, unsigned int memtotal);
void cmd_cls(struct CONSOLE *cons);
void cmd_dir(struct CONSOLE *cons);
void cmd_type(struct CONSOLE *cons, int *fat, char *cmdline);
int cmd_app(struct CONSOLE *cons, int *fat, char *cmdline);
int *hrb_api(int edi, int esi, int ebp, int esp, int ebx, int edx, int ecx, int eax);
int *inthandler0c(int *esp);
int *inthandler0d(int *esp);
void hrb_api_linewin(struct SHEET *sht, int x0, int y0, int x1, int y1, int col);
