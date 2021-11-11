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

void readpw(Display *dpy, struct xrandr *rr, struct lock **locks, int nscreens, const char *hash);
struct lock * lockscreen(Display *dpy, struct xrandr *rr, int screen);

