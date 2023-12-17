#!/bin/bash
dd if=../haribote/hankaku.bin of=zh.org bs=1 
dd if=HZK16 of=zh.org bs=1 count=165440 seek=4096 
mv zh.org zh.fnt 
# ../../z_tools/bimzbin -osacmp in:zh.org out:zh.fnt -tek5 
rm zh.org