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

/*
void OnMapRequest(XMapRequestEvent e) {
  XGetWindowAttributes(display, e.window, &attr); 
  
  const Window frame = XCreateSimpleWindow(
      display,
      root,
      attr.x,
      attr.y,
      attr.width,
      attr.height,
      border_width,
      border_color,
      bg_color
    );

  XSelectInput(
      display,
      frame,
      SubstructureRedirectMask | SubstructureNotifyMask);

  XAddToSaveSet(display, e.window);
  
  XReparentWindow(display, e.window, frame, 0, 0);

  XMapWindow(display, e.window);
}
*/

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

void spawn(char **app) {
  // Also taken from clarawm cuz me noob :/
  if (fork() == 0) {
    setsid();
    execvp(app[0], app);
    exit(EXIT_SUCCESS);
  }
}

void loop() {
  while(true) {
    XEvent e;
    XKeyEvent key;
    XNextEvent(display, &e);

    switch(e.type) {
      case KeyPress:
        key = e.xkey;
        if (key.keycode == XKeysymToKeycode(display, launch_term))
          spawn(term);
        if (key.keycode == XKeysymToKeycode(display, launch_menu))
          spawn(menu);
        if (key.keycode == XKeysymToKeycode(display, launch_browser))
          spawn(browser);
        if (key.keycode == XKeysymToKeycode(display, launch_screenshot))
          spawn(screenshot);
        if (key.keycode == XKeysymToKeycode(display, kill_window)) {
          kill(e.xbutton.subwindow);
        }
     // case MapRequest:
     //     OnMapRequest(e.xmaprequest);
     //    break;
    }
  }
}

void init() {
  display = XOpenDisplay(NULL);

  if (display == NULL) {
    printf(" -> Error: Failed to open display");
  } else {
    root = XDefaultRootWindow(display);
    screen = XDefaultScreen(display);

    height = XDisplayHeight(display, screen);
    width = XDisplayWidth(display, screen);
    
    printf(" -> Info:\n h: %d | w: %d\n", height, width);

    XGrabKey(display, XKeysymToKeycode(display, launch_term), Mod4Mask, root, True, GrabModeAsync, GrabModeAsync);
    XGrabKey(display, XKeysymToKeycode(display, launch_menu), Mod4Mask, root, True, GrabModeAsync, GrabModeAsync);
    XGrabKey(display, XKeysymToKeycode(display, launch_browser), Mod4Mask, root, True, GrabModeAsync, GrabModeAsync);
    XGrabKey(display, XKeysymToKeycode(display, kill_window), Mod4Mask, root, True, GrabModeAsync, GrabModeAsync);
    XGrabKey(display, XKeysymToKeycode(display, quit), Mod4Mask, root, True, GrabModeAsync, GrabModeAsync);
    XGrabKey(display, XKeysymToKeycode(display, launch_screenshot), Mod4Mask, root, True, GrabModeAsync, GrabModeAsync);
    XGrabButton(display, AnyButton, Mod4Mask, root, True, ButtonPressMask | ButtonReleaseMask | PointerMotionMask | OwnerGrabButtonMask,
                GrabModeAsync, GrabModeAsync, None, None);
  }
}

int main(void) {
 init(); 
 loop();

 return 0;
}
