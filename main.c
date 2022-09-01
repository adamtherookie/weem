#include "config.h"
#include "main.h"

Display *display;
int screen;
Window root;

XWindowAttributes attr;

unsigned int width;
unsigned int height;

XButtonEvent start;
XEvent e;
XKeyEvent key;

Desktop desktops[NUM_DESKTOPS];

void logger(char *msg) {
  printf(ANSI_COLOR_GREEN " -> weem INFO: %s\n" ANSI_COLOR_RESET, msg);
}

void err(char *msg) {
  printf(ANSI_COLOR_RED " -> weem ERROR: %s\n" ANSI_COLOR_RESET, msg);
}

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

static inline void OnButtonPress(XEvent e) {
  logger("Event ButtonPress Called");
  if (e.xbutton.subwindow == None) return;
  start = e.xbutton;
  XRaiseWindow(display, start.subwindow);
  XGetWindowAttributes(display, start.subwindow, &attr);
  logger("Event ButtonPress Finished");
}

static inline void OnKeyPress(XEvent e) {
  logger("Event KeyPress Called");
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
  if (key.keycode == XKeysymToKeycode(display, die)) {
    logger("kill");
    XCloseDisplay(display);
    exit(EXIT_SUCCESS);
  }
  logger("Event KeyPress Finished");
}

static inline void OnMotionNotify(XEvent e) {
  if (start.subwindow == None) return;

  int xdiff = e.xbutton.x_root - start.x_root;
  int ydiff = e.xbutton.y_root - start.y_root;
  XMoveResizeWindow(display, start.subwindow,
      attr.x + (start.button == 1 ? xdiff : 0),
      attr.y + (start.button == 1 ? ydiff : 0),
      MAX(1, attr.width + (start.button == 3 ? xdiff : 0)),
      MAX(1, attr.height + (start.button == 3 ? ydiff : 0)));
}

void loop() {
  logger("Entered loop\n");
  while(true) {
    XNextEvent(display, &e);

    switch(e.type) {
      case KeyPress: OnKeyPress(e); break;
      case ButtonPress: OnButtonPress(e); break;
      case MotionNotify: OnMotionNotify(e); break;
      case ButtonRelease: start.subwindow = None; break;
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
    XGrabKey(display, XKeysymToKeycode(display, die), Mod4Mask, root, True, GrabModeAsync, GrabModeAsync);

    XGrabButton(display, AnyButton, Mod4Mask, root, True, ButtonPressMask | ButtonReleaseMask | PointerMotionMask | OwnerGrabButtonMask, 
        GrabModeAsync, GrabModeAsync, None, None);

    // INIT ALL DESKTOPS TO 0
    for (int i = 0; i < NUM_DESKTOPS; i ++) {
      desktops[i].num_wins = 0;
    }
  }
}

int main(void) {
  init(); 
  loop();

  XCloseDisplay(display);
  return 0;
}
