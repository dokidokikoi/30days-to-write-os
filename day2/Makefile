TOOLPATH = ../../z_tools/
MAKE     = make -r
NASK     = $(TOOLPATH)nask
EDIMG    = $(TOOLPATH)edimg
COPY     = cp
DEL      = rm

default :
	make img

ipl.bin : ipl.nas Makefile
	$(NASK) ipl.nas ipl.bin ipl.lst

helloos.img : ipl.bin Makefile
	$(EDIMG)   imgin:$(TOOLPATH)fdimg0at.tek \
		wbinimg src:ipl.bin len:512 from:0 to:0   imgout:helloos.img

img :
	make -r helloos.img

run :
	make img
	$(COPY) helloos.img $(TOOLPATH)bochs/fdimage0.bin
	$(MAKE) -C $(TOOLPATH)bochs

clean :
	-$(DEL) ipl.bin
	-$(DEL) ipl.lst