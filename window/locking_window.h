#pragma once
#include <ctype.h>
#include <errno.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <X11/extensions/Xrandr.h>
#include <X11/keysym.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <config.h>

struct lock {
	int screen;
	Window root, win;
	Pixmap pmap;
	unsigned long colors[NUMCOLS];
};

struct xrandr {
	int active;
	int evbase;
	int errbase;
};

struct locking_window {

	struct xrandr rr;
	struct lock **locks;
	Display *dpy;
	int nscreens;
	XFontStruct* font_info;

};

const char* init_xlib(struct locking_window* lw);
void free_xlib(struct locking_window* lw);
const char* lock_all_screens(struct locking_window* lw);
const char* run_ui();
