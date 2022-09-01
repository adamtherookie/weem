#ifndef __MAIN_H__
#define __MAIN_H__

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>

#include <X11/Xlib.h>
#include <X11/keysym.h>

#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MAX_WINS 16
#define NUM_DESKTOPS 10
#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"

typedef struct KeyMap {
  KeySym keysym;
  char *cmd[10];
} KeyMap;

typedef struct Client {
  unsigned int desktop;
  Window window;
} Client;

typedef struct Desktop {
  unsigned int num_wins;
  Client clients[MAX_WINS];
} Desktop;

#endif

