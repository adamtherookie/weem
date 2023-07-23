#!/bin/sh

echo " -> Creating autostart directory"
mkdir -p ~/.config/weem
echo " -> Creating autostart script"
touch ~/.config/weem/autostart.sh

echo " -> Compiling source code"
sh build.sh
echo " -> Installing"
sudo cp ./weem /usr/bin

echo "[x] Installation finished. You can find the autostart script at ~/.config/weem/autostart.sh"
