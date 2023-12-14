#!/bin/bash
../../z_tools/edimg copy nocmp: from:jpn16v00.fnt to:jpn16v00.bin 
dd if=jpn16v00.bin of=nihongo.org bs=1 count=4096 skip=303104 
dd if=jpn16v00.bin of=nihongo.org bs=32 count=4418 seek=128 
mv nihongo.org nihongo.fnt 
# ../../z_tools/bim2bin -osacmp in:nihongo.org out:nihongo.fnt -tek5 
rm jpn16v00.bin nihongo.org 