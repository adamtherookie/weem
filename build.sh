#!/bin/sh

gcc main.c -o weem -lX11 -lXft -lfontconfig -I/usr/include/freetype2 -Og -g -fsanitize=undefined,address

