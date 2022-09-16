#include "config.h"
#include "main.h"

Display *display;
Screen *screen;

int scr_num;

Window root;

XWindowAttributes attr;

unsigned int width;
unsigned int height;

XButtonEvent start;
XEvent e;
XKeyEvent key;

static Client *head = NULL;
static Client *current = NULL;

static Desktop desktops[NUM_DESKTOPS];
int current_desktop = 0;

void logger(char *msg) {
  printf(ANSI_COLOR_GREEN " -> weem INFO:" ANSI_COLOR_RESET " %s\n", msg);
}

void err(char *msg) {
  printf(ANSI_COLOR_RED " -> weem ERROR:" ANSI_COLOR_RESET "%s\n" , msg);
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

void KillClient() {
  if (current == NULL) return;
  XEvent ke;
	ke.type = ClientMessage;
	ke.xclient.window = current->window;
	ke.xclient.message_type = XInternAtom(display, "WM_PROTOCOLS", True);
	ke.xclient.format = 32;
	ke.xclient.data.l[0] = XInternAtom(display, "WM_DELETE_WINDOW", True);
	ke.xclient.data.l[1] = CurrentTime;
	XSendEvent(display, current->window, False, NoEventMask, &ke);
  kill(current->window);
}

void spawn(char **args) {
  if (fork() == 0) {
    setsid();
    execvp(args[0], args);
    exit(EXIT_SUCCESS);
  }
}

void UpdateCurrent() {
  Client *c;

  for (c = head; c; c = c->next) {
    if (current == c) {
      logger("Raising window");
      XSetWindowBorderWidth(display, c->window, border_width);
      XSetWindowBorder(display, c->window, border_focus);
      XSetInputFocus(display, c->window, RevertToParent, CurrentTime);
      XRaiseWindow(display, c->window);
    } else {
      XSetWindowBorder(display, c->window, border_unfocus);
    }
  }
}

void AddWin(Window w) {
  Client *c;

  if (!(c = (Client *)calloc(1, sizeof(Client))))
    err("calloc error");
  
  if (head == NULL) {
    // no wins in desktop, so this one will be head
    c->next = NULL;
    c->prev = NULL;
    c->window = w;
    
    head = c;
  } else {
    Client *t;
    for(t = head; t->next; t = t->next);

    c->next = NULL;
    c->prev = t;
    c->window = w;

    t->next = c;
  }

  current = c;
}

void RemoveWindow(Window w) {
  Client *c;
  
  for(c = head; c; c = c->next) {
    if (c->window == w) {
      if (c->prev == NULL && c->next == NULL) {
        logger("prev null and next null");
        free(head);
        head = NULL;
        current = NULL;
        return;
      }
      if (c->prev == NULL) {
        logger("prev null");
        head = c->next;
        c->next->prev = NULL;
        current = c->next;
      } else if(c->next == NULL) {
        logger("next null");
        c->prev->next = NULL;
        current = c->prev;
      } else {
        logger("none null");
        c->prev->next = c->next;
        c->next->prev = c->prev;
        current = c->prev;
      }

      free(c);
      return;
    }
  }
}

void ChangeDesk(int num) {
  Client *c;

  if (num == current_desktop) {
    logger("nvm we're already here");
    return;
  }

  // Unmap windows
  logger("unmapping windows");
  if (head != NULL) {
    for (c = head; c; c = c->next) {
      XUnmapWindow(display, c->window);
    }
  }

  desktops[current_desktop].head = head;
  desktops[current_desktop].current = current;

  head = desktops[num].head;
  current = desktops[num].current;
  current_desktop = num;
  
  logger("mapping new windows");
  // Map windows
  if (head != NULL) {
    for (c = head; c; c = c->next) {
      XMapWindow(display, c->window);
    }
  }

  UpdateCurrent();
}

static inline void OnButtonPress(XEvent e) {
  if (e.xbutton.subwindow == None) return;
  start = e.xbutton;
  
  // Hacky way to get current client
  Client *c;

  for (c = head; c; c = c->next) {
    if (c->window == start.subwindow) {
      current = c;
      UpdateCurrent();
    }
  }

  XGetWindowAttributes(display, start.subwindow, &attr);
}

static inline void OnKeyPress(XEvent e) {
  key = e.xkey;

  for (int i = 0; i < num_keys; i ++) {
    if (key.keycode == XKeysymToKeycode(display, keymap[i].keysym)) {
      spawn(keymap[i].cmd);
      break;
    }
  }

  if (key.keycode == XKeysymToKeycode(display, kill_win)) {
    KillClient();
  }

  for (int i = 0; i < NUM_DESKTOPS; i ++) {
    if (key.keycode == XKeysymToKeycode(display, changedesktop[i].keysym)) {
      logger("Changing desktop");
      ChangeDesk(changedesktop[i].desktop);
    }
  }
  
  if (key.keycode == XKeysymToKeycode(display, die)) {
    logger("kill");
    XCloseDisplay(display);
    exit(EXIT_SUCCESS);
  }
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

static inline void OnMapRequest(XEvent e) {
  logger("Map request");
  static XWindowAttributes attributes;

  if (!XGetWindowAttributes(display, e.xmaprequest.window, &attributes) || attributes.override_redirect) 
    return;
  
  AddWin(e.xmaprequest.window);
  XMoveWindow(display, e.xmaprequest.window, width/2 - attributes.width/2, height/2 - attributes.height/2);
  XMapWindow(display, e.xmaprequest.window);

  UpdateCurrent();
}

static inline void OnConfigureRequest(XEvent e) {
  logger("Configure request");
  XWindowChanges changes;

  XConfigureRequestEvent event = e.xconfigurerequest;
  changes.x = event.x;
  changes.y = event.y;
  changes.width = event.width;
  changes.height = event.height;
  changes.border_width = event.border_width;
  changes.sibling = event.above;
  changes.stack_mode = event.detail;

  XConfigureWindow(display, event.window, event.value_mask, &changes);
}

static inline void OnDestroyNotify(XEvent e) {
  logger("Destroy notify");
  Client *c;

  int i = 0;
  for(c = head; c; c = c->next) {
    if (e.xdestroywindow.window == c->window) {
      i ++;
    } 
  }

  if (i == 0) {
    logger("Nothing here");
    return;
  } 

  logger("time to remove window");
  RemoveWindow(e.xdestroywindow.window);
  logger("time to update");
  UpdateCurrent();
  logger("Removed window");
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
      case MapRequest: OnMapRequest(e); break;
      case ConfigureRequest: OnConfigureRequest(e); break;
      case DestroyNotify: OnDestroyNotify(e); break;
    }
  }
}

void init() {
  display = XOpenDisplay(NULL);

  if (display == NULL) {
    err("Failed to open display");
  } else {
    root = DefaultRootWindow(display);
    screen = DefaultScreenOfDisplay(display);
    scr_num = XDefaultScreen(display);

    height = XDisplayHeight(display, scr_num);
    width = XDisplayWidth(display, scr_num);

    // Grab
    for (int i = 0; i < num_keys; i ++) {
      XGrabKey(display, XKeysymToKeycode(display, keymap[i].keysym), Mod4Mask, root, True, GrabModeAsync, GrabModeAsync);
    }

    for (int i = 0; i < NUM_DESKTOPS; i ++) {
      XGrabKey(display, XKeysymToKeycode(display, changedesktop[i].keysym), Mod4Mask, root, True, GrabModeAsync, GrabModeAsync);
      desktops[i].head = head;
      desktops[i].current = current;
    }

    ChangeDesk(0);

    XGrabKey(display, XKeysymToKeycode(display, kill_win), Mod4Mask, root, True, GrabModeAsync, GrabModeAsync);
    XGrabKey(display, XKeysymToKeycode(display, die), Mod4Mask, root, True, GrabModeAsync, GrabModeAsync);

    XGrabButton(display, AnyButton, Mod4Mask, root, True, ButtonPressMask | ButtonReleaseMask | PointerMotionMask | OwnerGrabButtonMask, 
        GrabModeAsync, GrabModeAsync, None, None);

    XSelectInput(display, root, SubstructureNotifyMask | SubstructureRedirectMask);
    //system("~/.config/weem/weemrc");
  }
}

int main(void) {
  init(); 
  loop();

  XCloseDisplay(display);
  return 0;
}
