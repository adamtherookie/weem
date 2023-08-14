#ifndef __CONFIG_H__
#define __CONFIG_H__

#include <X11/Xlib.h>
#include <X11/keysym.h>
#include "main.h"
#include "colors.h"

#define true 1
#define false 0

#define top 1
#define bottom 0

#define overline 1
#define underline 0

#define MOD Mod4Mask
#define SHIFT ShiftMask

#define DEFAULT_LAYOUT MASTER_STACK

KeyMod set_layout_master_stack       = { XK_T, MOD | SHIFT };
KeyMod set_layout_stripes_vertical   = { XK_V, MOD | SHIFT };
KeyMod set_layout_stripes_horizontal = { XK_H, MOD | SHIFT };

KeyMod kill_win   = { XK_W, MOD };
KeyMod fullscreen = { XK_F, MOD };
KeyMod tile       = { XK_T, MOD };
KeyMod floating   = { XK_S, MOD };
KeyMod die        = { XK_K, MOD };

KeyMod up         = { XK_Up, MOD };
KeyMod down       = { XK_Down, MOD };
KeyMod left       = { XK_Left, MOD };
KeyMod right      = { XK_Right, MOD };

KeyMod toggle_bar = { XK_B, MOD };

#define num_keys 8

KeyMap keymap[num_keys] = {
  // Terminal
  [0].keysym = XK_Return,
  [0].cmd =  "kitty",
  // Menu
  [1].keysym = XK_D,      
  [1].cmd = "rofi -show drun",
  // Browser
  [2].keysym = XK_Q,
  [2].cmd = "qutebrowser",
  // Screenshot
  [3].keysym = XK_G,
  [3].cmd = "scrot",
  // Wallpaper
  [4].keysym = XK_P,
  [4].cmd = "wp ~/Pictures",// wp is a script I made, change it for whatever wallpaper service you use
  // Audio up
  [5].keysym = XK_F5,
  [5].cmd = "amixer -D pulse sset Master 5%+ > /dev/null",
  // Audio down
  [6].keysym = XK_F4,
  [6].cmd = "amixer -D pulse sset Master 5%- > /dev/null",
  // Audio mute
  [7].keysym = XK_F1,
  [7].cmd = "amixer -D pulse sset Master 1+ toggle",
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
unsigned long border_focus    = color12;
unsigned long border_unfocus  = color4;

// weembar
unsigned int show_bar         = true;

const char *desktop_icons[NUM_DESKTOPS] = { "I", "II", "III", "IV", "V", "VI", "VII", "VIII", "IX", "X" };
unsigned int icons_offset     = 0;
unsigned int icons_size       = 7;
unsigned int icons_padding    = 10;
unsigned int bar_occu_size    = 1;
unsigned int bar_occu_type    = underline;
unsigned long desktop_focus    = color12;
unsigned long desktop_unfocus  = color4;
unsigned long text_focus       = color15;
unsigned long text_unfocus     = color14;

float bar_refresh_rate        = 1;

unsigned int twelve_hour_time = true;
unsigned int show_seconds     = false;

unsigned int bar_position     = bottom;
unsigned int bar_size         = 30;
unsigned int bar_border_size  = 0;
unsigned long bar_border_color = color6;
unsigned int bar_padding_x    = 0;
unsigned int bar_padding_y    = 0;
unsigned long font_color      = color15;
unsigned long bar_color       = color8;
unsigned int font_size        = 12;
char *font_name               = "monospace:size=9";

#endif

