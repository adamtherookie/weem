#!/bin/sh

while true
do
  echo -n "Audio: " > ~/.weembar
  echo -n $(awk -F"[][]" '/Left:/ { print $2 }' <(amixer sget Master)) >> ~/.weembar
  sleep 1
done
