#ifndef __CONFIG_H__
#define __CONFIG_H__

#include <X11/Xlib.h>
#include <X11/keysym.h>
#include "main.h"

#define num_keys 4

KeySym kill_win = XK_W;
KeySym die = XK_K;

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

unsigned int border_width = 3;
unsigned long border_color = 0x999f63;

#endif

