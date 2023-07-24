<div style="text-align:center">
<h1>weem</h1>
A small, lightweight, customizable X window manager written in C.
</div>

<hr>

![screenshot](./screenshot.png)

## Features
- Tiling (master and stack)
- Fullscreen
- Multiple Desktops
- Status Bar (in progress)
- Autostart script

## Installing
Clone the repo:
```console
git clone https://github.com/adamtherookie/weem
```
Edit `config.h` to match your preferences, setup the startup script at `~/.config/weem/autostart.sh` if you wish, then run the install script (requires root access): `sudo sh install.sh`.

Upon changing the configuration, you need to rerun the install script for the changes to take place. 

Finally, add `exec weem` to the end of your `xinitrc`.

## Default Keybindings
| Key | Action |
|-----|--------|
| Super + Enter | Spawn terminal |
| Super + Q | Spawn web browser |
| Super + D | Spawn application launcher |
| Super + W | Close window |
| Super + G | Screenshot |
| Super + Up | Move up in list |
| Super + Down | Move down in list |
| Super + Right | Increase master size |
| Super + Left | Decrease master size |
| Super + F | Make window fullscreen |
| Super + T | Make window tiled |
| Super + S | Make window floating |
| Super + B | Toggle bar |
| Super + K | Kill weem |
| Super + NUM_KEY | Switch to desktop |
| Super + Shift + NUM_KEY | Send window to desktop |

## Thanks
A big thanks to these awesome people and projects without which weem wouldn't exist:
- [clarawm](https://github.com/dacousb/clarawm) by [@dacousb](https://github.com/dacousb)
- [swm](https://github.com/SollyBunny/swm) by [@SollyBunny](https://github.com/SollyBunny)
- [dwm](https://dwm.suckless.org/) by suckless
