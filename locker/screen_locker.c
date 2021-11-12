/* See LICENSE file for license details. */
#define _XOPEN_SOURCE 500
#if HAVE_SHADOW_H
#include <shadow.h>
#endif

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

#include <util.h>
#include <config.h>
#include <helpers.h>

/* char *argv0; */

/* void */
/* readpw(Display *dpy, struct xrandr *rr, struct lock **locks, int nscreens, */
       /* const char *hash) */
/* { */
	/* XRRScreenChangeNotifyEvent *rre; */
	/* char buf[32], passwd[256], *inputhash; */
	/* int num, screen, running, failure, oldc; */
	/* unsigned int len, color; */
	/* KeySym ksym; */
	/* XEvent ev; */

	/* len = 0; */
	/* running = 1; */
	/* failure = 0; */
	/* oldc = INIT; */

	/* while (running && !XNextEvent(dpy, &ev)) { */
		/* if (ev.type == KeyPress) { */
			/* explicit_bzero(&buf, sizeof(buf)); */
			/* num = XLookupString(&ev.xkey, buf, sizeof(buf), &ksym, 0); */
			/* if (IsKeypadKey(ksym)) { */
				/* if (ksym == XK_KP_Enter) */
					/* ksym = XK_Return; */
				/* else if (ksym >= XK_KP_0 && ksym <= XK_KP_9) */
					/* ksym = (ksym - XK_KP_0) + XK_0; */
			/* } */
			/* if (IsFunctionKey(ksym) || */
					/* IsKeypadKey(ksym) || */
					/* IsMiscFunctionKey(ksym) || */
					/* IsPFKey(ksym) || */
					/* IsPrivateKeypadKey(ksym)) */
				/* continue; */
      /* if(ev.xkey.state & ControlMask) { */
        /* switch(ksym) { */
          /* case XK_u: ksym = XK_Escape; break; */
          /* case XK_m: ksym = XK_Return; break; */
        /* } */
      /* } */
      /* if(ev.xkey.state & (ControlMask|Mod1Mask)) { */
        /* switch(ksym) { */
          /* case XK_l: system("/bin/.dwm/change_keyboard_layout.sh"); break; */
        /* } */
      /* } */

			/* switch (ksym) { */
			/* case XK_Return: */
				/* passwd[len] = '\0'; */
				/* errno = 0; */
				/* if (!(inputhash = crypt(passwd, hash))) */
					/* fprintf(stderr, "slock: crypt: %s\n", strerror(errno)); */
				/* else */
					/* running = !!strcmp(inputhash, hash); */
				/* if (running) { */
					/* XBell(dpy, 100); */
					/* failure = 1; */
				/* } */
				/* explicit_bzero(&passwd, sizeof(passwd)); */
				/* len = 0; */
				/* break; */
			/* case XK_Escape: */
				/* explicit_bzero(&passwd, sizeof(passwd)); */
				/* len = 0; */
				/* break; */
			/* case XK_BackSpace: */
				/* if (len) */
					/* passwd[--len] = '\0'; */
				/* break; */
			/* default: */
				/* if (num && !iscntrl((int)buf[0]) && */
						/* (len + num < sizeof(passwd))) { */
					/* memcpy(passwd + len, buf, num); */
					/* len += num; */
				/* } */
				/* break; */
			/* } */
			/* color = len ? INPUT : ((failure || failonclear) ? FAILED : INIT); */
			/* if (running && oldc != color) { */
				/* for (screen = 0; screen < nscreens; screen++) { */
					/* XSetWindowBackground(dpy, */
															 /* locks[screen]->win, */
															 /* locks[screen]->colors[color]); */
					/* XClearWindow(dpy, locks[screen]->win); */
				/* } */
				/* oldc = color; */
			/* } */
		/* } else if (rr->active && ev.type == rr->evbase + RRScreenChangeNotify) { */
			/* rre = (XRRScreenChangeNotifyEvent*)&ev; */
			/* for (screen = 0; screen < nscreens; screen++) { */
				/* if (locks[screen]->win == rre->window) { */
					/* if (rre->rotation == RR_Rotate_90 || */
							/* rre->rotation == RR_Rotate_270) */
						/* XResizeWindow(dpy, locks[screen]->win, */
													/* rre->height, rre->width); */
					/* else */
						/* XResizeWindow(dpy, locks[screen]->win, */
													/* rre->width, rre->height); */
					/* XClearWindow(dpy, locks[screen]->win); */
					/* break; */
				/* } */
			/* } */
		/* } else { */
			/* for (screen = 0; screen < nscreens; screen++) */
				/* XRaiseWindow(dpy, locks[screen]->win); */
		/* } */
	/* } */
/* } */

/* struct lock * */
/* lockscreen(Display *dpy, struct xrandr *rr, int screen) */
/* { */
	/* char curs[] = {0, 0, 0, 0, 0, 0, 0, 0}; */
	/* int i, ptgrab, kbgrab; */
	/* struct lock *lock; */
	/* XColor color, dummy; */
	/* XSetWindowAttributes wa; */
	/* Cursor invisible; */

	/* if (dpy == NULL || screen < 0 || !(lock = malloc(sizeof(struct lock)))) */
		/* return NULL; */

	/* lock->screen = screen; */
	/* lock->root = RootWindow(dpy, lock->screen); */

	/* for (i = 0; i < NUMCOLS; i++) { */
		/* XAllocNamedColor(dpy, DefaultColormap(dpy, lock->screen), */
										 /* colorname[i], &color, &dummy); */
		/* lock->colors[i] = color.pixel; */
	/* } */

	/* [> init <] */
	/* wa.override_redirect = 1; */
	/* wa.background_pixel = lock->colors[INIT]; */
	/* lock->win = XCreateWindow(dpy, lock->root, 0, 0, */
														/* DisplayWidth(dpy, lock->screen), */
														/* DisplayHeight(dpy, lock->screen), */
														/* 0, DefaultDepth(dpy, lock->screen), */
														/* CopyFromParent, */
														/* DefaultVisual(dpy, lock->screen), */
														/* CWOverrideRedirect | CWBackPixel, &wa); */
	/* lock->pmap = XCreateBitmapFromData(dpy, lock->win, curs, 8, 8); */
	/* invisible = XCreatePixmapCursor(dpy, lock->pmap, lock->pmap, */
																	/* &color, &color, 0, 0); */
	/* XDefineCursor(dpy, lock->win, invisible); */

	/* [> Try to grab mouse pointer *and* keyboard for 600ms, else fail the lock <] */
	/* for (i = 0, ptgrab = kbgrab = -1; i < 6; i++) { */
		/* if (ptgrab != GrabSuccess) { */
			/* ptgrab = XGrabPointer(dpy, lock->root, False, */
														/* ButtonPressMask | ButtonReleaseMask | */
														/* PointerMotionMask, GrabModeAsync, */
														/* GrabModeAsync, None, None, CurrentTime); */
		/* } */
		/* if (kbgrab != GrabSuccess) { */
			/* kbgrab = XGrabKeyboard(dpy, lock->root, True, */
														 /* GrabModeAsync, GrabModeAsync, CurrentTime); */
		/* } */

		/* [> input is grabbed: we can lock the screen <] */
		/* if (ptgrab == GrabSuccess && kbgrab == GrabSuccess) { */
			/* if (rr->active) */
				/* XRRSelectInput(dpy, lock->win, RRScreenChangeNotifyMask); */

			/* XSelectInput(dpy, lock->root, SubstructureNotifyMask); */
			/* return lock; */
		/* } */

		/* [> retry on AlreadyGrabbed but fail on other errors <] */
		/* if ((ptgrab != AlreadyGrabbed && ptgrab != GrabSuccess) || */
				/* (kbgrab != AlreadyGrabbed && kbgrab != GrabSuccess)) */
			/* break; */

		/* usleep(100000); */
	/* } */

	/* [> we couldn't grab all input: fail out <] */
	/* if (ptgrab != GrabSuccess) */
		/* fprintf(stderr, "slock: unable to grab mouse pointer for screen %d\n", */
						/* screen); */
	/* if (kbgrab != GrabSuccess) */
		/* fprintf(stderr, "slock: unable to grab keyboard for screen %d\n", */
						/* screen); */
	/* return NULL; */
/* } */

