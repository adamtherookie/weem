#ifndef __CONFIG_H__
#define __CONFIG_H__

#include <X11/Xlib.h>
#include <X11/keysym.h>
#include "main.h"

#define true 1
#define false 0

#define MOD Mod4Mask

KeySym kill_win = XK_W;
KeySym fullscreen = XK_F;
KeySym tile = XK_T;
KeySym floating = XK_S;
KeySym die = XK_K;

KeySym up = XK_Up;
KeySym down = XK_Down;
KeySym left = XK_Left;
KeySym right = XK_Right;

#define num_keys 4

KeyMap keymap[num_keys] = {
  // Terminal
  [0].keysym = XK_Return,
  [0].cmd =  { "kitty", NULL  },
  // Menu
  [1].keysym = XK_D,      
  [1].cmd = { "rofi", "-show", "drun", NULL },
  // Browser
  [2].keysym = XK_Q,
  [2].cmd = { "qutebrowser", NULL },
  // Screenshot
  [3].keysym = XK_G,
  [3].cmd = { "flameshot", "gui", NULL }
};

ChangeDesktop changedesktop[NUM_DESKTOPS] = {
    [0].keysym = XK_1,
  [0].desktop = 0,
    [1].keysym = XK_2,
  [1].desktop = 1,
    [2].keysym = XK_3,
  [2].desktop = 2,
    [3].keysym = XK_4,
  [3].desktop = 3,
    [4].keysym = XK_5,
  [4].desktop = 4,
    [5].keysym = XK_6,
  [5].desktop = 5,
    [6].keysym = XK_7,
  [6].desktop = 6,
    [7].keysym = XK_8,
  [7].desktop = 7,
    [8].keysym = XK_9,
  [8].desktop = 8,
    [9].keysym = XK_0,
  [9].desktop = 9,
};

unsigned int snap = false;
unsigned int snap_threshold  = 10;

float master_size            = 0.6;
float master_tick            = 0.05;

unsigned int gap_width       = 10;
unsigned int border_width    = 2;
unsigned long border_focus   = 0x999f63;
unsigned long border_unfocus = 0xafafaf;

#endif

