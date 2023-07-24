#!/bin/sh

./build.sh
Xephyr -br -ac -noreset -screen 800x600 :2 &

sleep 1

DISPLAY=:2 ./weem
