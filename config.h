#ifndef __CONFIG_H__
#define __CONFIG_H__

#include <X11/Xlib.h>
#include <X11/keysym.h>

const unsigned int border_width = 2;
const unsigned long border_color = 0x7a7a7a;
const unsigned long bg_color = 0x0000ff;

KeySym launch_term = XK_Return;
KeySym launch_menu = XK_D;
KeySym launch_browser = XK_B;
KeySym launch_screenshot = XK_S;
KeySym kill_window = XK_Q;
KeySym quit = XK_K;

char *menu[] = {"rofi", "-show", "drun", NULL};
char *term[] = {"kitty", NULL};
char *browser[] = {"qutebrowser", NULL};
char *screenshot[] = {"flameshot", NULL};

#endif

