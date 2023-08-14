#!/bin/sh

AUTOSTART_DIR=~/.config/weem
AUTOSTART_SCRIPT=~/.config/weem/autostart.sh

if [ ! -d "$AUTOSTART_DIR" ]; then
  echo " -> Creating autostart directory"
  mkdir -p ~/.config/weem
fi

if [ ! -f "$AUTOSTART_SCRIPT" ]; then
  echo " -> Creating autostart script"
  touch ~/.config/weem/autostart.sh
fi

echo " -> Compiling source code"
sh build.sh || { echo " -> ERROR: compilation failed. Aborting installation."; exit 1; }

echo " -> Installing"
sudo rm -f /usr/bin/weem
sudo cp ./weem /usr/bin/

touch ~/.weembar

echo "[x] Installation finished. You can find the autostart script at ~/.config/weem/autostart.sh"
