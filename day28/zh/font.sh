#!/bin/bash
dd if=../haribote/hankaku.bin of=zh.fnt bs=1 \
    && dd if=HZK16 of=zh.fnt bs=1 count=165440 seek=4096