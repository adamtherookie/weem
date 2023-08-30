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

// Uncomment this and define width/height for multi-monitor setups
// #define WIDTH 2732
// #define HEIGHT 768

#define DEFAULT_LAYOUT MASTER_STACK

#define NUM_DESKTOPS 10

KeyMod set_layout_master_stack       = { XK_T, MOD | SHIFT };
KeyMod set_layout_stripes_vertical   = { XK_V, MOD | SHIFT };
KeyMod set_layout_stripes_horizontal = { XK_H, MOD | SHIFT };
KeyMod set_layout_floating           = { XK_F, MOD | SHIFT };

KeyMod kill_win   = { XK_W, MOD };
KeyMod fullscreen = { XK_F, MOD };
KeyMod tile       = { XK_T, MOD };
KeyMod floating   = { XK_S, MOD };
KeyMod die        = { XK_K, MOD };

KeyMod up         = { XK_Up, MOD };
KeyMod down       = { XK_Down, MOD };
KeyMod left       = { XK_Left, MOD };
KeyMod right      = { XK_Right, MOD };

KeyMod toggle_bar          = { XK_B, MOD };
KeyMod toggle_bar_position = { XK_B, MOD | SHIFT };

KeyMod cycle_colorscheme_up   = { XK_C, MOD };
KeyMod cycle_colorscheme_down = { XK_C, MOD | SHIFT };

#define num_keys 7

KeyMap keymap[num_keys] = {
  // Terminal
  [0].keymod = { XK_Return, MOD },
  [0].cmd =  "xterm",
  // Menu
  [1].keymod = { XK_D, MOD },      
  [1].cmd = "rofi -show drun",
  // Browser
  [2].keymod = { XK_Q, MOD },
  [2].cmd = "firefox",
  // Screenshot
  [3].keymod = { XK_G, MOD },
  [3].cmd = "ksnip -r",
  // Audio up
  [4].keymod = { XK_F5, AnyModifier },
  [4].cmd = "amixer -D pulse sset Master 5%+ > /dev/null",
  // Audio down
  [5].keymod = { XK_F4, AnyModifier },
  [5].cmd = "amixer -D pulse sset Master 5%- > /dev/null",
  // Audio mute
  [6].keymod = { XK_F1, AnyModifier },
  [6].cmd = "amixer -D pulse sset Master 1+ toggle",
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
KeyMod inc_gap_width          = { XK_plus, MOD };
KeyMod dec_gap_width          = { XK_minus, MOD };
unsigned int max_gap_width    = 40;
unsigned int min_gap_width    = 1;

unsigned int border_width     = 2;

// weembar
unsigned int show_bar         = true;

char *desktop_icons[NUM_DESKTOPS] = { "I", "II", "III", "IV", "V", "VI", "VII", "VIII", "IX", "X" };
unsigned int icons_offset     = 0;
unsigned int icons_size       = 7;
unsigned int icons_padding    = 10; // for best results, make it (icons_size + (font_size / 2))
unsigned int bar_occu_size    = 3;
unsigned int bar_occu_type    = underline;

unsigned int bar_refresh_rate = 10000; // in microseconds: 1s is recommended

unsigned int twelve_hour_time = true;
unsigned int show_seconds     = false;

unsigned int bar_position     = bottom;
unsigned int bar_size         = 30;
unsigned int bar_border_size  = 0;
unsigned int bar_padding_x    = 0;
unsigned int bar_padding_y    = 0;
unsigned int font_size        = 9;
char *font_name               = "monospace:size=9";

unsigned int max_name_length  = 40;

unsigned int show_indicators  = false;
char *bar_indicators[2]       = {
                                  "[]", // Tiled
                                  "><>", // Floating
                                };

// Colorschemes
#define NUM_COLORSCHEMES 2
Colorscheme colorschemes[NUM_COLORSCHEMES] = {
  (Colorscheme){
    .border_focus    = 0x7287FD,
    .border_unfocus  = 0x1E2030,
    .desktop_focus   = 0x7287FD,
    .desktop_unfocus = 0x1E2030,
    .bar_border      = 0x181296,
    .bar_color       = 0x181296,
    .font_color      = 0xFFFFFF,
    .font_color_sel  = 0x363A4F,
    .font_color_occu = 0x181926,
  },
  (Colorscheme){
    .border_focus    = 0x7287fD,
    .border_unfocus  = 0x9CA0B0,
    .desktop_focus   = 0x7287FD,
    .desktop_unfocus = 0x9CA0B0,
    .bar_border      = 0xDCE0E8,
    .bar_color       = 0xDCE0E8,
    .font_color      = 0x4C4F69,
    .font_color_sel  = 0x4C4F69,
    .font_color_occu = 0x4C4F69,
  }
}

#endif
