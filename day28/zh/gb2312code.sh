#!/bin/bash
echo "GB2312 简体中文模式" > a.txt
iconv -f utf-8 -t gb2312 a.txt > b.txt
hexdump -C b.txt
rm a.txt b.txt