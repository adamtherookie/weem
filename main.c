#include "config.h"
#include "main.h"

Display *display;
Screen *screen;

int scr_num;

Window root;
Bar bar;

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

unsigned int error_occurred = 0;

static inline void logger(char *msg) {
  printf(ANSI_COLOR_GREEN " -> weem INFO:" ANSI_COLOR_RESET " %s\n", msg);
}

static inline void err(char *msg) {
  printf(ANSI_COLOR_RED " -> weem  ERR:" ANSI_COLOR_RESET " %s\n" , msg);
}

static inline int is_bar(Display *display, Window window) {
  if (window == bar.window) return true;
  
  XClassHint class_hint;
  if (XGetClassHint(display, window, &class_hint)) {
    int is_bar = strcmp(class_hint.res_name, "weembar") == 0;
    XFree(class_hint.res_name);
    XFree(class_hint.res_class);
    return is_bar;
  }

  return false;
}

static inline void kill(Window w) {
  if (w == None) return;
  // Since I have no idea how to handle this, I took (stole) it from:
  // https://github.com/dacousb/clarawm/blob/master/clarawm.c
  XEvent ke;
  ke.type = ClientMessage;
  ke.xclient.window = w;
  ke.xclient.message_type = XInternAtom(display, "WM_PROTOCOLS", True);
  ke.xclient.format = 32;
  ke.xclient.data.l[0] = XInternAtom(display, "WM_DELETE_WINDOW", True);
  ke.xclient.data.l[1] = CurrentTime;
  XSendEvent(display, w, False, NoEventMask, &ke);
}

static inline void KillClient() {
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

static inline void spawn(char **args) {
  if (fork() == 0) {
    setsid();
    execvp(args[0], args);
    exit(EXIT_SUCCESS);
  }
}

static inline void UpdateCurrent() {
  Client *c;

  for (c = head; c; c = c->next) {
    if (current == c) {
      XSetWindowBorderWidth(display, c->window, border_width);
      XSetWindowBorder(display, c->window, border_focus);
      XSetInputFocus(display, c->window, RevertToParent, CurrentTime);
      XRaiseWindow(display, c->window);
    } else {
      XSetWindowBorder(display, c->window, border_unfocus);
    }
  }

  for (c = head; c; c = c->next) {
    if (c->is_floating) {
      XRaiseWindow(display, c->window);
    }
  }
}

void TileWindows() {
  unsigned int tiled_windows = 0;
  unsigned int stack_windows = 0;
  Client *c;

  for (c = head; c; c = c->next) {
    if (c->desktop == current_desktop && c->is_tiled) {
      tiled_windows++;
    }
  }

  stack_windows = tiled_windows - 1;
  unsigned int win_height = height - bar_size - bar_padding_y;

  if (tiled_windows == 0) {
    return;
  }

  if (tiled_windows == 1) {
    unsigned int x, y, w, h;

    c = head;

    x = gap_width;
    y = (bar_position == top) ? (bar_size + gap_width + bar_padding_y) : (gap_width);
    w = width - (2 * gap_width) - (2 * border_width);
    h = win_height - (2 * gap_width) - (2 * border_width);

    XMoveResizeWindow(display, c->window, x, y, w, h);
  } else {
    unsigned int master_width = (width * desktops[current_desktop].master) - (gap_width * 2);
    unsigned int stack_width = (width * (1 - desktops[current_desktop].master)) - (gap_width * 2);

    unsigned int n = 0;
    for (c = head; c; c = c->next) {
      if (c->desktop == current_desktop && !c->is_fullscreen && !c->is_floating) {
        unsigned int x, y, w, h;

        if (n == 0) {
          x = gap_width;
          y = (bar_position == top) ? (bar_size + gap_width + bar_padding_y) : (gap_width);
          w = master_width - (border_width * 2);
          h = win_height - (border_width * 2) - (gap_width * 2);
        } else { 
          x = width * desktops[current_desktop].master;
          w = stack_width - (border_width * 2) + (gap_width);
          h = (win_height - (2 * border_width * stack_windows) - gap_width * (stack_windows + 1)) / stack_windows;
          y = ((bar_position == top) ? (bar_size + gap_width + bar_padding_y) : (gap_width)) + ((n - 1) * (h + gap_width + (2 * border_width)));
        }

        XMoveResizeWindow(display, c->window, x, y, w, h);
        n++;
      }
    }
  }
}

static inline void AddWin(Window w) {
  Client *c;

  if (!(c = (Client *)calloc(1, sizeof(Client))))
    err("calloc error");
  
  if (head == NULL) {
    // no wins in desktop, so this one will be head
    c->next = NULL;
    c->prev = NULL;
    c->window = w;
    
    head = c;
    head->desktop = current_desktop;
    head->is_tiled = 1;
  } else {
    Client *t;
    for(t = head; t->next; t = t->next);

    c->next = NULL;
    c->prev = t;
    c->window = w;

    t->next = c;

    t->next->desktop = current_desktop;
    t->next->is_tiled = 1;
  }

  current = c;
}

static inline void RemoveWindow(Window w) {
  Client *c;
  
  for(c = head; c; c = c->next) {
    if (c->window == w) {
      if (c->prev == NULL && c->next == NULL) {
        free(head);
        head = NULL;
        current = NULL;
        return;
      }
      if (c->prev == NULL) {
        head = c->next;
        c->next->prev = NULL;
        current = c->next;
      } else if(c->next == NULL) {
        c->prev->next = NULL;
        current = c->prev;
      } else {
        c->prev->next = c->next;
        c->next->prev = c->prev;
        current = c->prev;
      }

      free(c);
      TileWindows();
      return;
    }
  }
}

static inline void SendDesk(int num) {
  if (num == current_desktop || current == None) return;

  Client *target = current;
  XUnmapWindow(display, current->window);
  
  if (current->prev) current = current->prev;
  else current = current->next;

  if (target->prev) {
    target->prev->next = target->next;
  } else {
    head = target->next;
  }

  if (target->next) {
    target->next->prev = target->prev;
  }
  
  if (desktops[num].head) {
    Client *t;

    for (t = desktops[num].head; t->next; t = t->next);

    t->next = target;

    target->next = NULL;
    target->prev = t;
  } else {
    desktops[num].head = target;
    target->next = NULL;
    target->prev = NULL;
  }

  target->desktop = num;
  if (!desktops[num].current) desktops[num].current = target;

  UpdateCurrent();
  TileWindows();
}

static inline void ChangeDesk(int num) {
  Client *c;

  if (num == current_desktop) {
    return;
  }

  // Unmap windows
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
  
  // Map windows
  if (head != NULL) {
    for (c = head; c; c = c->next) {
      XMapWindow(display, c->window);
    }
  }

  UpdateCurrent();
  TileWindows();
}

static inline void TileWindow() {
  if(current == NULL) return;

  current->is_fullscreen = 0;
  current->is_floating = 0;
  current->is_tiled = 1;

  TileWindows();
  UpdateCurrent();
}

static inline void FloatWindow() {
  if(current == NULL) return;

  current->is_fullscreen = 0;
  current->is_tiled = 0;
  current->is_floating = 1;

  TileWindows();
  UpdateCurrent();
}

static inline void FullscreenWindow() {
  if(current == NULL) return;

  if(!current->is_fullscreen) {
    XWindowAttributes attributes;
    XGetWindowAttributes(display, current->window, &attributes);

    current->old_x = attributes.x;
    current->old_y = attributes.y;
    current->old_w = attributes.width;
    current->old_h = attributes.height;

    if(current->is_tiled) current->prev_state = 1;
    else current->prev_state = 2;

    current->is_fullscreen = 1;
    current->is_tiled = 0;
    current->is_floating = 0;

    int screen = DefaultScreen(display);
    int screenWidth = DisplayWidth(display, screen);
    int screenHeight = DisplayHeight(display, screen);

    XMoveResizeWindow(display, current->window, -border_width, -border_width, screenWidth + border_width, screenHeight + border_width);
    XMapWindow(display, current->window);

    UpdateCurrent();
  } else {
    current->is_fullscreen = 0;
    
    if(current->prev_state == 1) {
      current->is_tiled = 1;
      current->is_floating = 0;

      TileWindows();
    } else {
      current->is_tiled = 0;
      current->is_floating = 1;

      XMoveResizeWindow(display, current->window, current->old_x, current->old_y, current->old_w, current->old_h);
      XMapWindow(display, current->window);
    }
    UpdateCurrent();
  }
}

FontStruct* create_font(char *font_name, unsigned long color, Window window) {
  FontStruct* fs = (FontStruct*)malloc(sizeof(FontStruct));

  fs->font = XLoadQueryFont(display, font_name);
  if (fs->font == NULL) err("font not found");
  fs->gc = XCreateGC(display, window, 0, NULL);
  XSetForeground(display, fs->gc, color);
  fs->color = color;

  return fs;
}

static inline void DrawBarInfo() {
  unsigned int offset = icons_offset;

  for (unsigned int i = 0; i < NUM_DESKTOPS; i++) {
    XSetForeground(display, bar.font->gc, (i == current_desktop) ? desktop_focus : desktop_unfocus);
    XFillRectangle(display, bar.window, bar.font->gc, offset, 0, (int)icons_size * strlen(desktop_icons[i]) + icons_padding, bar_size);

    if (desktops[i].head) {
      XSetForeground(display, bar.font->gc, desktop_focus);
      if (bar_occu_type == overline) {
        XFillRectangle(display, bar.window, bar.font->gc, offset, 0, (int)icons_size * strlen(desktop_icons[i]) + icons_padding, bar_occu_size);
      } else {
        XFillRectangle(display, bar.window, bar.font->gc, offset, bar_size - bar_occu_size, (int)icons_size * strlen(desktop_icons[i]) + icons_padding, bar_occu_size);
      }
    }

    XSetForeground(display, bar.font->gc, (i == current_desktop) ? (text_focus) : (text_unfocus));
    XDrawString(display, bar.window, bar.font->gc, offset + (font_size / 2.0f), (bar_size / 2) + (font_size / 2), desktop_icons[i], strlen(desktop_icons[i]));    
  
    offset += icons_padding + (icons_size * strlen(desktop_icons[i]));
  }
}

static inline void UpdateBar() {
  Client *c;

  int found = 0;
  for (c = head; c; c = c->next) {
    if (c->is_fullscreen) found = 1; 
  }

  if (!found) XRaiseWindow(display, bar.window);

  DrawBarInfo();
}

static inline void CreateBar() {
  if (!show_bar) return;

  if (bar_position == top)
    bar.window = XCreateSimpleWindow(display, root, bar_padding_x, bar_padding_y, width - (bar_padding_x * 2), bar_size, bar_border_size, bar_border_color, bar_color);
  else
    bar.window = XCreateSimpleWindow(display, root, bar_padding_x, height - bar_size - bar_padding_y, width - (bar_padding_x * 2), bar_size, bar_border_size, bar_border_color, bar_color);
  
  XSelectInput(display, bar.window, SubstructureRedirectMask | SubstructureNotifyMask);
  XSetStandardProperties(display, bar.window, "weembar", "weembar", None, NULL, 0, NULL);
  XMapWindow(display, bar.window);
  bar.font = create_font(font_name, font_color, bar.window);
}

static inline void MoveUp() {
  if (!current || !current->prev) return;

  Client *new_next = current->prev;

  if (current->next) {
    current->next->prev = current->prev;
  }

  current->prev->next = current->next;
  current->prev = current->prev->prev;

  if (current->prev) {
    current->prev->next = current;
  } else {
    head = current;
  }

  current->next = new_next;
  current->next->prev = current;

  UpdateCurrent();
  TileWindows();
}

static inline void MoveDown() {
  if (!current || !current->next) return;

  Client *new_prev = current->next;

  if (current->prev) {
    current->prev->next = current->next;
  } else {
    head = current->next;
  }

  current->next->prev = current->prev;
  current->next = current->next->next;

  if (current->next) {
    current->next->prev = current;
  }

  current->prev = new_prev;
  current->prev->next = current;

  UpdateCurrent();
  TileWindows();
}

static inline void IncMaster() {
  desktops[current_desktop].master += master_tick;
  desktops[current_desktop].master = MIN(MAX(desktops[current_desktop].master, 0.2), 0.8);
  TileWindows();
}

static inline void DecMaster() {
  desktops[current_desktop].master -= master_tick;
  desktops[current_desktop].master = MIN(MAX(desktops[current_desktop].master, 0.2), 0.8);
  TileWindows();
}

static inline int ErrorHandler(Display *display, XErrorEvent *event) {
  char error_message[256];
  XGetErrorText(display, event->error_code, error_message, sizeof(error_message));
  err(error_message);
  error_occurred = 1;

  return 0;
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

  if (key.keycode == XKeysymToKeycode(display, kill_win.keysym)) {
    KillClient();
  }

  if (key.keycode == XKeysymToKeycode(display, fullscreen.keysym)) {
    FullscreenWindow();
  }

  if (key.keycode == XKeysymToKeycode(display, tile.keysym)) {
    TileWindow();
  }

  if (key.keycode == XKeysymToKeycode(display, floating.keysym)) {
    FloatWindow();
  }

  if (key.keycode == XKeysymToKeycode(display, up.keysym)) {
    MoveUp();
  }

  if (key.keycode == XKeysymToKeycode(display, down.keysym)) {
    MoveDown();
  }

  if (key.keycode == XKeysymToKeycode(display, right.keysym)) {
    IncMaster();
  }

  if (key.keycode == XKeysymToKeycode(display, left.keysym)) {
    DecMaster();
  }

  for (int i = 0; i < NUM_DESKTOPS; i ++) {
    if (key.keycode == XKeysymToKeycode(display, changedesktop[i].keysym)) {
      if (key.state & ShiftMask) {
        SendDesk(changedesktop[i].desktop);
      } else {
        ChangeDesk(changedesktop[i].desktop);
      }
    }
  }

  if (key.keycode == XKeysymToKeycode(display, die.keysym)) {
    XCloseDisplay(display);
    exit(EXIT_SUCCESS);
  }
}

static inline void OnMotionNotify(XEvent e) {
  if (start.subwindow == None) return;
  if (is_bar(display, start.subwindow)) return;

  int xdiff = e.xbutton.x_root - start.x_root;
  int ydiff = e.xbutton.y_root - start.y_root;

  if (start.button == 1 && !current->is_tiled && !current->is_fullscreen) {
    int new_x = attr.x + xdiff;
    int new_y = attr.y + ydiff;
    int max_x = width - attr.width - (2 * border_width);
    int max_y = height - attr.height - (2 * border_width);

    if (snap) {
      if (new_x < snap_threshold) {
        new_x = 0;
      }
      else if (new_x > max_x - snap_threshold) {
        new_x = max_x;
      }

      if (new_y < snap_threshold) {
        new_y = 0;
      }

      else if (new_y > max_y - snap_threshold) {
        new_y = max_y;
      }
    }

    XMoveWindow(display, start.subwindow, new_x, new_y);
  }

  if (start.button == 3 && !current->is_tiled && !current->is_fullscreen) {
    XMoveResizeWindow(display, start.subwindow,
      attr.x,
      attr.y,
      MAX(1, attr.width + xdiff),
      MAX(1, attr.height + ydiff));
  }

  if (start.button == 3 && current->is_tiled) {
    desktops[current_desktop].master = (float)(attr.width + xdiff) / width;
    desktops[current_desktop].master = MIN(MAX(desktops[current_desktop].master, 0.2), 0.8);
    TileWindows();
  }

  XFlush(display);
}

static inline void OnMapRequest(XEvent e) {
  if (is_bar(display, e.xmaprequest.window)) {
    logger("we have a bar");
    XWindowAttributes attributes;
    XGetWindowAttributes(display, e.xmaprequest.window, &attributes);

    XMapWindow(display, e.xmaprequest.window);
    XMoveResizeWindow(display, e.xmaprequest.window, 0, 0, width, attributes.height);
  } else {
    static XWindowAttributes attributes;

    if (!XGetWindowAttributes(display, e.xmaprequest.window, &attributes) || attributes.override_redirect) 
      return;
    
    AddWin(e.xmaprequest.window);
    XMoveWindow(display, e.xmaprequest.window, width/2 - attributes.width/2, height/2 - attributes.height/2);
    XMapWindow(display, e.xmaprequest.window);

    TileWindows();
    UpdateCurrent();
  }
}

static inline void OnConfigureRequest(XEvent e) {
  XWindowChanges changes;

  XConfigureRequestEvent event = e.xconfigurerequest;
  changes.x = event.x;
  changes.y = event.y;
  changes.width = event.width;
  changes.height = event.height;
  changes.border_width = event.border_width;
  changes.sibling = event.above;
  changes.stack_mode = event.detail;

  XWindowAttributes attributes;

  if(XGetWindowAttributes(display, event.window, &attributes)) {
    XConfigureWindow(display, event.window, event.value_mask, &changes);
  } else {
    err("configuration error");
  }
}

static inline void OnDestroyNotify(XEvent e) {
  Client *c;

  int i = 0;
  for(c = head; c; c = c->next) {
    if (e.xdestroywindow.window == c->window) {
      i ++;
    } 
  }

  if (i == 0) {
    return;
  } 

  RemoveWindow(e.xdestroywindow.window);
  UpdateCurrent();
}

void loop() {
  while(true) {
    XSetErrorHandler(ErrorHandler);

    if (XNextEvent(display, &e) != 0) {
      err("event error");
      break;
    }

    UpdateBar();
    XSetErrorHandler(NULL);

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
    err("failed to open display");
  } else {
    root = DefaultRootWindow(display);
    screen = DefaultScreenOfDisplay(display);
    scr_num = XDefaultScreen(display);

    height = XDisplayHeight(display, scr_num);
    width = XDisplayWidth(display, scr_num);

    // Grab
    for (int i = 0; i < num_keys; i ++) {
      XGrabKey(display, XKeysymToKeycode(display, keymap[i].keysym), MOD, root, True, GrabModeAsync, GrabModeAsync);
    }

    for (int i = 0; i < NUM_DESKTOPS; i ++) {
      XGrabKey(display, XKeysymToKeycode(display, changedesktop[i].keysym), MOD, root, True, GrabModeAsync, GrabModeAsync);
      XGrabKey(display, XKeysymToKeycode(display, changedesktop[i].keysym), MOD | ShiftMask, root, True, GrabModeAsync, GrabModeAsync);
      desktops[i].head = head;
      desktops[i].current = current;
      desktops[i].master = master_size;
    }

    ChangeDesk(0);

    XGrabKey(display, XKeysymToKeycode(display, kill_win.keysym), kill_win.mod, root, True, GrabModeAsync, GrabModeAsync);
    XGrabKey(display, XKeysymToKeycode(display, die.keysym), die.mod, root, True, GrabModeAsync, GrabModeAsync);
    XGrabKey(display, XKeysymToKeycode(display, fullscreen.keysym), fullscreen.mod, root, True, GrabModeAsync, GrabModeAsync);
    XGrabKey(display, XKeysymToKeycode(display, tile.keysym), tile.mod, root, True, GrabModeAsync, GrabModeAsync);
    XGrabKey(display, XKeysymToKeycode(display, floating.keysym), floating.mod, root, True, GrabModeAsync, GrabModeAsync);

    XGrabKey(display, XKeysymToKeycode(display, up.keysym), up.mod, root, True, GrabModeAsync, GrabModeAsync);
    XGrabKey(display, XKeysymToKeycode(display, down.keysym), down.mod, root, True, GrabModeAsync, GrabModeAsync);
    XGrabKey(display, XKeysymToKeycode(display, left.keysym), left.mod, root, True, GrabModeAsync, GrabModeAsync);
    XGrabKey(display, XKeysymToKeycode(display, right.keysym), right.mod, root, True, GrabModeAsync, GrabModeAsync);

    XGrabButton(display, AnyButton, MOD, root, True, ButtonPressMask | ButtonReleaseMask | PointerMotionMask | OwnerGrabButtonMask, GrabModeAsync, GrabModeAsync, None, None);

    XSelectInput(display, root, SubstructureNotifyMask | SubstructureRedirectMask);

    XSetWindowAttributes attributes;
    attributes.event_mask = ButtonPress | KeyPress | SubstructureRedirectMask | SubstructureNotifyMask | PropertyChangeMask;
    attributes.cursor = XCreateFontCursor(display, XC_left_ptr);
    XChangeWindowAttributes(display, root, CWEventMask | CWCursor, &attributes);
    
    if (!show_bar) bar_size = 0;
    CreateBar();
    // system("~/.config/weem/autostart.sh");
  }
}

int main(void) {
  init(); 
  loop();

  XCloseDisplay(display);
  return 0;
}
