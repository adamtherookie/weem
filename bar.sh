#!/bin/sh

while true
do
  audio=$(awk -F'[][]' '/Left:/ {print $2}' <(amixer sget Master))
  disk_space=$(df -h / | awk 'NR==2 {print $4}')
  network=$(nmcli -t -f NAME connection show --active)
  cpu_usage=$(mpstat 1 1 | awk '/Average:/ {printf "%.0f", 100-$NF}')
  memory_usage=$(free | awk 'NR==2 {printf "%.0f", $3/$2 * 100}')
  temperature=$(sensors | awk '/Package id 0:/ {print $4}')
  notifications=$(dunstctl count)
  
  echo "Audio: $audio | Disk: $disk_space | CPU: $cpu_usage% | Memory: $memory_usage% | " > ~/.weembar
  sleep 1
done
