#!/bin/sh

# Build for production
gcc main.c -o weem -lX11 -lXft -lfontconfig -I/usr/include/freetype2 -O3 -Werror 

# Build for debug
# gcc main.c -o weem -lX11 -lXft -lfontconfig -I/usr/include/freetype2 -Og -g -fsanitize=undefined,address

