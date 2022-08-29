#ifndef __CONFIG_H__
#define __CONFIG_H__

#include <X11/Xlib.h>
#include <X11/keysym.h>

#define num_keys 4
#define MAX(a, b) ((a) > (b) ? (a) : (b))

typedef struct KeyMap {
  KeySym keysym;
  char *cmd[10];
} KeyMap;

KeySym kill_win = XK_W;

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

#endif

