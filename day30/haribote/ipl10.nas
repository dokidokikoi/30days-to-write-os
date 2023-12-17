; haribote-ipl
; TAB=4

CYLS	EQU		26				; どこまで読み込むか

	ORG		0x7c00			; このプログラムがどこに読み込まれるのか

; 以下は標準的なFAT12フォーマットフロッピーディスクのための記述

    JMP		entry
    DB		0x90
    DB		"HARIBOTE"		; ブートセクタの名前を自由に書いてよい（8バイト）
    DW		512				; 1セクタの大きさ（512にしなければいけない）
    DB		1				; クラスタの大きさ（1セクタにしなければいけない）
    DW		1				; FATがどこから始まるか（普通は1セクタ目からにする）
    DB		2				; FATの個数（2にしなければいけない）
    DW		224				; ルートディレクトリ領域の大きさ（普通は224エントリにする）
    DW		2880			; このドライブの大きさ（2880セクタにしなければいけない）
    DB		0xf0			; メディアのタイプ（0xf0にしなければいけない）
    DW		9				; FAT領域の長さ（9セクタにしなければいけない）
    DW		18				; 1トラックにいくつのセクタがあるか（18にしなければいけない）
    DW		2				; ヘッドの数（2にしなければいけない）
    DD		0				; パーティションを使ってないのでここは必ず0
    DD		2880			; このドライブ大きさをもう一度書く
    DB		0,0,0x29		; よくわからないけどこの値にしておくといいらしい
    DD		0xffffffff		; たぶんボリュームシリアル番号
    DB		"HARIBOTEOS "	; ディスクの名前（11バイト）
    DB		"FAT12   "		; フォーマットの名前（8バイト）
    RESB	18				; とりあえず18バイトあけておく

; プログラム本体

entry:
    MOV		AX,0			; レジスタ初期化
    MOV		SS,AX
    MOV		SP,0x7c00
    MOV		DS,AX

; 读磁盘

    MOV		AX,0x0820
    MOV		ES,AX
    MOV		CH,0			; 柱面0
    MOV		DH,0			; 磁头0
    MOV		CL,2			; 扇区2
    MOV		BX,18*2*CYLS-1	; 要读取的合计扇区数
	CALL	readfast        ; 告诉读取

; 读取结束，运行haribote.sys！

    MOV		BYTE [0x0ff0],CYLS	; 记录IPL实际读取了多少内容
    JMP		0xc200

error:
    MOV		AX,0
    MOV		ES,AX

putloop:
    MOV		AL,[SI]
    ADD		SI,1			; SIに1を足す
    CMP		AL,0
    JE		fin
    MOV		AH,0x0e			; 一文字表示ファンクション
    MOV		BX,15			; カラーコード
    INT		0x10			; ビデオBIOS呼び出し
    JMP		putloop
fin:
    HLT						; 何かあるまでCPUを停止させる
    JMP		fin				; 無限ループ
msg:
    DB		0x0a, 0x0a		; 改行を2つ
    DB		"load error"
    DB		0x0a			; 改行
    DB		0

readfast:	; 使用AL尽量一次性读取数据
;	ES:读取地址, CH:柱面, DH:磁头, CL:扇区, BX:读取扇区数

		MOV		AX,ES			; < 通过ES计算AL的最大值 >
		SHL		AX,3			; 将AX除以32，将结果存入AH(SHL是左移位指令)
		AND		AH,0x7f			; AH是AH除以128所得的余数(512*128=64K)
		MOV		AL,128			; AL = 128 - AH; AH是AH除以128所得的余数(512*128=64K)
		SUB		AL,AH

		MOV		AH,BL			; < 通过BX计算AL的最大值并存入AH >
		CMP		BH,0			; if (BH != 0) { AH = 18; }
		JE		.skip1
		MOV		AH,18
.skip1:
		CMP		AL,AH			; if (AL > AH) { AL = AH; }
		JBE		.skip2
		MOV		AL,AH
.skip2:

		MOV		AH,19			; < 通过CL计算AL的最大值并存入AH >
		SUB		AH,CL			; AH = 19 - CL;
		CMP		AL,AH			; if (AL > AH) { AL = AH; }
		JBE		.skip3
		MOV		AL,AH
.skip3:

		PUSH	BX
		MOV		SI,0			; 计算失败次数的寄存器
retry:
		MOV		AH,0x02			; AH=0x02 : 读取磁盘
		MOV		BX,0
		MOV		DL,0x00			; A盘
		PUSH	ES
		PUSH	DX
		PUSH	CX
		PUSH	AX
		INT		0x13			; 调用磁盘BIOS
		JNC		next			; 没有出错的话则跳转至next
		ADD		SI,1			; 将SI加1
		CMP		SI,5			; 将SI与5比较
		JAE		error			; SI >= 5则跳转至error
		MOV		AH,0x00
		MOV		DL,0x00			; A盘
		INT		0x13			; 驱动器重置
		POP		AX
		POP		CX
		POP		DX
		POP		ES
		JMP		retry
next:
		POP		AX
		POP		CX
		POP		DX
		POP		BX				; 将ES的内容存入BX
		SHR		BX,5			; 将BX由16字节为单位转换为512字节为单位
		MOV		AH,0
		ADD		BX,AX			; BX += AL;
		SHL		BX,5			; 将BX由512字节为单位转换为16字节为单位
		MOV		ES,BX			; 相当于EX += AL * 0x20;
		POP		BX
		SUB		BX,AX
		JZ		.ret
		ADD		CL,AL			; 将CL加上AL
		CMP		CL,18			; 将CL与18比较
		JBE		readfast		; CL <= 18则跳转至readfast
		MOV		CL,1
		ADD		DH,1
		CMP		DH,2
		JB		readfast		; DH < 2则跳转至readfast
		MOV		DH,0
		ADD		CH,1
		JMP		readfast
.ret:
		RET

		RESB	0x7dfe-$		; 到0x7dfe为止用0x00填充的指令

		DB		0x55, 0xaa