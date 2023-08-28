#ifndef __MAIN_H__
#define __MAIN_H__

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <time.h>

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>
#include <X11/cursorfont.h>
#include <X11/Xft/Xft.h>
#include <X11/Xatom.h>

#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX_WINS 16
#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"

enum {
  MASTER_STACK,
  STRIPES_VERTICAL,
  STRIPES_HORIZONTAL,
  FLOATING
};

typedef struct KeyMod {
  KeySym keysym;
  unsigned int mod;
} KeyMod;

typedef struct KeyMap {
  KeyMod keymod;
  char *cmd;
} KeyMap;

typedef struct ChangeDesktop {
  KeySym keysym;
  int desktop;
} ChangeDesktop;

typedef struct Client Client;
typedef struct Client {
  Client *next;
  Client *prev;

  int is_fullscreen;
  int is_floating;
  int is_tiled;

  int prev_state;

  int old_x;
  int old_y;
  int old_w;
  int old_h;

  int desktop;

  char *name;

  Window window;
} Client;

typedef struct Desktop {
  Client *head;
  Client *current;

  float master;
  int layout;
} Desktop;

typedef struct FontStruct {
  XftFont *xft_font;
  
  XftColor xft_color_fg;
  XftColor xft_color_selected;
  XftColor xft_color_occupied;

  XftDraw *xft_draw;
} FontStruct;

typedef struct Bar {
  Window window;
  FontStruct font;
} Bar;

#endif

