#include <X11/X.h>
#include <locking_window.h>
#include <stdlib.h>

const char* init_xlib(struct locking_window* lw) {

	if (!(lw->dpy = XOpenDisplay(NULL))) {
		return "slock: cannot open display\n";
	}

	/* check for Xrandr support */
	lw->rr.active = XRRQueryExtension(lw->dpy, &lw->rr.evbase, &lw->rr.errbase);

	/* get number of screens in display "dpy" and blank them */
	lw->nscreens = ScreenCount(lw->dpy);
	printf("Number of screens: %d\n", lw->nscreens);

	if (!(lw->locks = calloc(lw->nscreens, sizeof(struct lock *)))) {
		return "slock: out of memory\n";
	}

	return NULL;
}

void free_xlib(struct locking_window* lw) {

	XUngrabPointer(lw->dpy, CurrentTime);
	XUngrabKeyboard(lw->dpy, CurrentTime);

	for(int i = 0; i < lw->nscreens; ++i) {
		XDestroyWindow(lw->dpy, lw->locks[i]->win);
		free(lw->locks[i]);
	}

	XCloseDisplay(lw->dpy);

	free(lw->locks);
	free(lw);
}

const char* lockscreen(struct locking_window* lw, int screen) {
	lw->locks[screen] = (struct lock*)calloc(1, sizeof(struct lock));
	lw->locks[screen]->root = RootWindow(lw->dpy, lw->locks[screen]->screen);

	XSetWindowAttributes wa;
	wa.override_redirect = 1;
	wa.background_pixel = 0ul;
	lw->locks[screen]->win = XCreateWindow(
		lw->dpy,
		lw->locks[screen]->root,
		0,0, XDisplayWidth(lw->dpy, screen), XDisplayHeight(lw->dpy, screen),
		0, XDefaultDepth(lw->dpy, screen),
		CopyFromParent,
		DefaultVisual(lw->dpy, screen),
		CWOverrideRedirect | CWBackPixel, &wa);

	char curs[] = {0, 0, 0, 0, 0, 0, 0, 0};
	XColor color;
	color.red = color.blue = color.green = 0;

	lw->locks[screen]->pmap = XCreateBitmapFromData(lw->dpy, lw->locks[screen]->win, curs, 8, 8);
	Cursor invisible = XCreatePixmapCursor(lw->dpy, lw->locks[screen]->pmap, lw->locks[screen]->pmap, &color, &color, 0, 0);
	XDefineCursor(lw->dpy, lw->locks[screen]->win, invisible);

	if(GrabSuccess != XGrabPointer(lw->dpy, lw->locks[screen]->root, False, 
							ButtonPressMask | ButtonReleaseMask | PointerMotionMask, GrabModeAsync, GrabModeAsync, 
										None, None, CurrentTime)) {
		return "Error grabbing mouse";
	}
	
	if(XGrabKeyboard(lw->dpy, lw->locks[screen]->root, True, GrabModeAsync, GrabModeAsync, CurrentTime)) {
		return "Error grabbing keyboard";
	}

	if (lw->rr.active)
		XRRSelectInput(lw->dpy, lw->locks[screen]->win, RRScreenChangeNotifyMask);

	XSelectInput(lw->dpy, lw->locks[screen]->root, SubstructureNotifyMask);

	return NULL;
}

const char* lock_all_screens(struct locking_window* lw) {
	for(int i = 0; i < lw->nscreens; ++i) {
		const char* err_msg = lockscreen(lw, i);
		if(err_msg != NULL) {
			return err_msg;
		}
	}

	return NULL;
}

const char* update_password_input(XEvent* e) {

	char* buf = (char*)calloc(4, sizeof(char));
	KeySym ksym;
	XLookupString(&e->xkey, buf, 4, &ksym, NULL);
	printf("%s | %s\n", buf, XKeysymToString(ksym));
	free(buf);

	return NULL;
}

void show_windows(struct locking_window* lw) {

	for(int i = 0; i < lw->nscreens; ++i) {
		XSetWindowBackground(lw->dpy, lw->locks[i]->win, 0ul);
		XClearWindow(lw->dpy, lw->locks[i]->win);

		XRaiseWindow(lw->dpy, lw->locks[i]->win); 
		XMapWindow(lw->dpy, lw->locks[i]->win);
	}

	while(1) {
		XEvent e;
		XNextEvent(lw->dpy, &e);
		switch(e.type) {
			case KeyPress:
				if(XK_Return == XLookupKeysym(&e.xkey, 0)) 
					return;
		}

	}
}

const char* run_ui() {
	
	struct locking_window* lw = (struct locking_window*)calloc(1, sizeof(struct locking_window));
	const char* err_msg = init_xlib(lw);

	if(err_msg != NULL) {
		free_xlib(lw);
		return err_msg;
	}

	err_msg = lock_all_screens(lw);

	if(err_msg != NULL) {
		free_xlib(lw);
		return err_msg;
	}

	show_windows(lw);

	free_xlib(lw);
	return NULL;
}
