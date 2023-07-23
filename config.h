#ifndef __CONFIG_H__
#define __CONFIG_H__

#include <X11/Xlib.h>
#include <X11/keysym.h>
#include "main.h"

#define true 1
#define false 0

#define top 1
#define bottom 0

#define overline 1
#define underline 0

#define MOD Mod4Mask
#define SHIFT ShiftMask

KeyMod kill_win   = { XK_W, MOD };
KeyMod fullscreen = { XK_F, MOD };
KeyMod tile       = { XK_T, MOD };
KeyMod floating   = { XK_S, MOD };
KeyMod die        = { XK_K, MOD };

KeyMod up         = { XK_Up, MOD };
KeyMod down       = { XK_Down, MOD };
KeyMod left       = { XK_Left, MOD };
KeyMod right      = { XK_Right, MOD };

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

unsigned int snap             = false;
unsigned int snap_threshold   = 10;

float master_size             = 0.6;
float master_tick             = 0.05;

unsigned int gap_width        = 6;
unsigned int border_width     = 2;
unsigned long border_focus    = 0x5E81AC;
unsigned long border_unfocus  = 0x3B4252;

// weembar
unsigned int show_bar         = true;

const char *desktop_icons[NUM_DESKTOPS] = { "I", "II", "III", "IV", "V", "VI", "VII", "VIII", "IX", "X" };
unsigned int icons_offset     = 0;
unsigned int icons_size       = 7;
unsigned int icons_padding    = 10;
unsigned int bar_occu_size    = 3;
unsigned int bar_occu_type    = underline;
unsigned int desktop_focus    = 0x5E81AC;
unsigned int desktop_unfocus  = 0x3B4252;
unsigned int text_focus       = 0x393939;
unsigned int text_unfocus     = 0xECEFF4;

unsigned int bar_position     = bottom;
unsigned int bar_size         = 30;
unsigned int bar_border_size  = 0;
unsigned int bar_border_color = 0xff0000;
unsigned int bar_padding_x    = 0;
unsigned int bar_padding_y    = 0;
unsigned long font_color      = 0xECEFF4;
unsigned long bar_color       = 0x2E3440;
unsigned int font_size        = 12;
char *font_name               = "JetBrainsMono NF:size=12";

#endif
