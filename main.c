#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>

#include "config.h"

Display *display;
int screen;
Window root;

XWindowAttributes attr;

unsigned int width;
unsigned int height;

XButtonEvent start;
XEvent event;

void kill(Window w) {
  if (w == None) return;
  // Since I have no idea how to handle this, I took (stole) it from:
  //  https://github.com/dacousb/clarawm/blob/master/clarawm.c
  XEvent ke;
  ke.type = ClientMessage;
  ke.xclient.window = w;
  ke.xclient.message_type = XInternAtom(display, "WM_PROTOCOLS", True);
  ke.xclient.format = 32;
  ke.xclient.data.l[0] = XInternAtom(display, "WM_DELETE_WINDOW", True);
  ke.xclient.data.l[1] = CurrentTime;
  XSendEvent(display, w, False, NoEventMask, &ke);
}

void spawn(char **args) {
  if (fork() == 0) {
    setsid();
    execvp(args[0], args);
    exit(EXIT_SUCCESS);
  }
}

void OnButtonPress(Window w) {
  if (w == None) return;
  XRaiseWindow(display, w);
  XGetWindowAttributes(display, w, &attr);
}

void err(char *msg) {
  fprintf(stderr, " -> weem ERROR: %s\n", msg);
}

void logger(char *msg) {
  fprintf(stdout, " -> weem INFO: %s\n", msg);
}

void loop() {
  logger("Entered loop\n");
  while(true) {
    XEvent e;
    XKeyEvent key;
    XNextEvent(display, &e);

    switch(e.type) {
      case KeyPress:
        key = e.xkey;
        for (int i = 0; i < num_keys; i ++) {
          if (key.keycode == XKeysymToKeycode(display, keymap[i].keysym)) {
            spawn(keymap[i].cmd);
            break;
          }
        }
        if (key.keycode == XKeysymToKeycode(display, kill_win)) {
          kill(e.xbutton.subwindow);
        }
      case ButtonPress:
        OnButtonPress(e.xbutton.subwindow);
        start = e.xbutton;
        break;
      case MotionNotify:
        if (start.subwindow != None) {
          int xdiff = e.xbutton.x_root - start.x_root;
          int ydiff = e.xbutton.y_root - start.y_root;
          XMoveResizeWindow(display, start.subwindow,
            attr.x + (start.button==1 ? xdiff : 0),
            attr.y + (start.button==1 ? ydiff : 0),
            MAX(1, attr.width + (start.button==3 ? xdiff : 0)),
            MAX(1, attr.height + (start.button==3 ? ydiff : 0)));    
        } 
      break;
    }
  }
}

void init() {
  display = XOpenDisplay(NULL);

  if (display == NULL) {
    err("Failed to open display");
  } else {
    root = XDefaultRootWindow(display);
    screen = XDefaultScreen(display);

    height = XDisplayHeight(display, screen);
    width = XDisplayWidth(display, screen);

    // Grab
    for (int i = 0; i < num_keys; i ++) {
      XGrabKey(display, XKeysymToKeycode(display, keymap[i].keysym), Mod4Mask, root, True, GrabModeAsync, GrabModeAsync);
    }

    XGrabKey(display, XKeysymToKeycode(display, kill_win), Mod4Mask, root, True, GrabModeAsync, GrabModeAsync);
    
    XGrabButton(display, AnyButton, Mod4Mask, root, True, ButtonPressMask | ButtonReleaseMask | PointerMotionMask | OwnerGrabButtonMask, 
        GrabModeAsync, GrabModeAsync, None, None);

  }
}

int main(void) {
 init(); 
 loop();

 return 0;
}
