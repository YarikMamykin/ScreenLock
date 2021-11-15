#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <locking_window.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <user_data.h>
#include <password_input_handler.h>

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

void draw_input(struct locking_window* lw, struct password_input_handler* pih, unsigned long color) {

	for(int i = 0; i < lw->nscreens; ++i) {

		GC gc = DefaultGC(lw->dpy, i);

		XClearWindow(lw->dpy, lw->locks[i]->win);

		XSetForeground(lw->dpy, gc, color);

		const int text_width = XTextWidth(lw->font_info, (const char*)pih->input, pih->inserted_chars);
		const int text_height = lw->font_info->ascent + lw->font_info->descent;
		const int screen_width = XDisplayWidth(lw->dpy, i); 
		const int screen_height = XDisplayHeight(lw->dpy, i);

		XDrawString(lw->dpy, lw->locks[i]->win, gc, screen_width/2 - text_width/2, screen_height/2 - text_height/2, (const char*)pih->input, pih->inserted_chars);
	}

}

void draw_start_message(struct locking_window* lw, struct user_data* ud, unsigned long color) {

	for(int i = 0; i < lw->nscreens; ++i) {

		GC gc = DefaultGC(lw->dpy, i);

		XClearWindow(lw->dpy, lw->locks[i]->win);

		XSetForeground(lw->dpy, gc, color);

		const char* start_message = ud->no_password ? "Press any key to unlock..." : "Waiting for user input...";

		const int text_width = XTextWidth(lw->font_info, start_message, strlen(start_message));
		const int text_height = lw->font_info->ascent + lw->font_info->descent;
		const int screen_width = XDisplayWidth(lw->dpy, i); 
		const int screen_height = XDisplayHeight(lw->dpy, i);

		XDrawString(lw->dpy, lw->locks[i]->win, gc, screen_width/2 - text_width/2, screen_height/2 - text_height/2, start_message, strlen(start_message));
	}

}

void clear_windows(struct locking_window* lw) {

	for(int i = 0; i < lw->nscreens; ++i) {
		XClearWindow(lw->dpy, lw->locks[i]->win);
	}

}

void draw_greeting(struct locking_window* lw, const char* uname, unsigned long color) {

	for(int i = 0; i < lw->nscreens; ++i) {

		GC gc = DefaultGC(lw->dpy, i);

		XClearWindow(lw->dpy, lw->locks[i]->win);

		XSetForeground(lw->dpy, gc, color);

		const char* greeting_msg_base = "Hello there, "; // stands before uname
		const char* greeting_msg_end = "!"; // stands after uname

		const unsigned int greeting_msg_size = strlen(greeting_msg_base) + strlen(uname) + strlen(greeting_msg_end);
		char* greeting_msg = (char*)alloca(greeting_msg_size);
		memset(greeting_msg, 0, greeting_msg_size);

		sprintf(greeting_msg, "%s%s%s", greeting_msg_base, uname, greeting_msg_end);
		const int greeting_msg_len = strlen(greeting_msg);

		const int text_width = XTextWidth(lw->font_info, (const char*)greeting_msg, greeting_msg_len);
		const int text_height = lw->font_info->ascent + lw->font_info->descent;
		const int screen_width = XDisplayWidth(lw->dpy, i); 
		const int screen_height = XDisplayHeight(lw->dpy, i);

		XDrawString(lw->dpy, lw->locks[i]->win, gc, screen_width/2 - text_width/2, screen_height/2 - text_height/2, (const char*)greeting_msg, greeting_msg_len);
	}
}

void draw_input_info(struct locking_window* lw, struct password_input_handler* pih, unsigned long color) {
	if(pih->view_mode == NATIVE)
		draw_input_stars(lw, pih, color);
	else
		draw_input(lw, pih, color);
}

void process_events(struct locking_window* lw, struct user_data* ud) {

	struct password_input_handler* pih = init_password_input_handler(ud->hash);
	draw_start_message(lw, ud, 255ul);

	while(1) {
		XEvent* e = (XEvent*)alloca(sizeof(XEvent));
		XNextEvent(lw->dpy, e);

		switch(e->type) {
			case KeyPress:
				{
					if(ud->no_password) {
						free_password_input(pih);
						return;
					}

					switch(XLookupKeysym(&e->xkey, 0)) {
						case XK_Escape: 
							{
								reset_password_input(pih); 
								clear_windows(lw); 
								break;
							}

						case XK_v:
							{
									pih->view_mode = pih->view_mode == NATIVE ? SECURED : NATIVE;
									draw_input_info(lw, pih, 255ul);
									break;
							}

						default: 
							{
								update_password_input(pih, get_input_char(e)); 
								if(password_input_match(pih)) 
									draw_greeting(lw, ud->pwd->pw_name, 255ul << 8);
								else 
									draw_input_info(lw, pih, 255ul);
							}
					}
					break;
				}

			default:
				{
					if(!ud->no_password && password_input_match(pih)) {
						usleep(700 * 1000); 
						free_password_input(pih);
						return;
					}
				}
		}

	}
}

const char* run_ui(struct user_data* ud) {
	
	struct locking_window* lw = (struct locking_window*)calloc(1, sizeof(struct locking_window));
	lw->font_info = NULL;
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
