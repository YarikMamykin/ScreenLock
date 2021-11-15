#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <locking_window.h>
#include <stdlib.h>
#include <string.h>
#include <user_data.h>

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

	lw->font_info = XLoadQueryFont(lw->dpy, "*-25-*");
	if(lw->font_info) {
		for(int i = 0; i < lw->nscreens; ++i) {
			XSetFont(lw->dpy, DefaultGC(lw->dpy, i), lw->font_info->fid);
		}
	}

	return NULL;
}

void free_xlib(struct locking_window* lw) {

	XUngrabPointer(lw->dpy, CurrentTime);
	XUngrabKeyboard(lw->dpy, CurrentTime);

	XFreeFont(lw->dpy, lw->font_info);

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
	XSync(lw->dpy, 0);

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

char get_input_char(XEvent* e) {

	const int buf_size = 32;
	char* buf = (char*)alloca(buf_size);
	KeySym ksym;
	XLookupString(&e->xkey, buf, buf_size, &ksym, NULL);
	
	return buf[0];
}

void show_windows(struct locking_window* lw) {

	for(int i = 0; i < lw->nscreens; ++i) {
		XSetWindowBackground(lw->dpy, lw->locks[i]->win, 0ul);
		XClearWindow(lw->dpy, lw->locks[i]->win);

		XRaiseWindow(lw->dpy, lw->locks[i]->win); 
		XMapWindow(lw->dpy, lw->locks[i]->win);
	}

}

struct password_input_handler {
	char* input;
	unsigned short inserted_chars;
	const char* approved_hash;
};

void free_password_input(struct password_input_handler* pih) {
	free(pih->input);
	free(pih);
}

void reset_password_input(struct password_input_handler* pih) {
	free(pih->input);
	pih->input = (char*)calloc(32, sizeof(char));
	pih->inserted_chars = 0;
}

void update_password_input(struct password_input_handler* pih, char input_char) {

	if(pih->inserted_chars < 32) {
		pih->input[pih->inserted_chars++] = input_char;
		return;
	}
}

int password_input_match(struct password_input_handler* pih) {
	const char* input_hash = crypt(pih->input, pih->approved_hash);
	return strcmp(input_hash, pih->approved_hash) == 0;
}

struct password_input_handler* init_password_input_handler(const char* hash) {
	struct password_input_handler* pih = (struct password_input_handler*)calloc(1, sizeof(struct password_input_handler));
	pih->input = (char*)calloc(32, sizeof(char));
	pih->inserted_chars = 0;
	pih->approved_hash = hash;
	return pih;
}

void draw_input_stars(struct locking_window* lw, struct password_input_handler* pih, unsigned long color) {

	for(int i = 0; i < lw->nscreens; ++i) {

		GC gc = DefaultGC(lw->dpy, i);

		XClearWindow(lw->dpy, lw->locks[i]->win);

		XSetForeground(lw->dpy, gc, color);

		char* input_to_display[pih->inserted_chars];
		memset(input_to_display, '*', pih->inserted_chars);

		const int text_width = XTextWidth(lw->font_info, (const char*)input_to_display, pih->inserted_chars);
		const int text_height = lw->font_info->ascent + lw->font_info->descent;
		const int screen_width = XDisplayWidth(lw->dpy, i); 
		const int screen_height = XDisplayHeight(lw->dpy, i);

		XDrawString(lw->dpy, lw->locks[i]->win, gc, screen_width/2 - text_width/2, screen_height/2 - text_height/2, (const char*)input_to_display, pih->inserted_chars);
	}

}

void clear_windows(struct locking_window* lw) {

	for(int i = 0; i < lw->nscreens; ++i) {
		XClearWindow(lw->dpy, lw->locks[i]->win);
	}

}

void process_events(struct locking_window* lw, struct user_data* ud) {

	struct password_input_handler* pih = init_password_input_handler(ud->hash);

	while(1) {
		XEvent* e = (XEvent*)alloca(sizeof(XEvent));
		XNextEvent(lw->dpy, e);

		switch(e->type) {
			case KeyPress:
				{
					const char* input_info = get_input_info(e);
					draw_info(lw, input_info);

					switch(XLookupKeysym(&e->xkey, 0)) {
						case XK_Return: free_password_input(pih); return;
						case XK_Escape: reset_password_input(pih); break;
						default: update_password_input(pih, get_input_char(e)); draw_hash_info(lw, crypt(pih->input, pih->approved_hash), pih->approved_hash, pih->input);
					}
				}
		}

	}
}

const char* run_ui(struct user_data* ud) {
	
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

	process_events(lw, ud);

	free_xlib(lw);
	return NULL;
}
