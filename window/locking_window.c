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
	lw->locks[screen] = malloc(sizeof(struct lock));
	RootWindow(lw->dpy, lw->locks[screen]->screen);
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

	free_xlib(lw);
	return NULL;
}
