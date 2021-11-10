#pragma once
#include <ctype.h>
#include <errno.h>
#include <grp.h>
#include <pwd.h>
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

const char * gethash(void);
void dontkillme(void);
void die(const char *errstr, ...);
void readpw(Display *dpy, struct xrandr *rr, struct lock **locks, int nscreens, const char *hash);
struct lock * lockscreen(Display *dpy, struct xrandr *rr, int screen);
void usage(void);

