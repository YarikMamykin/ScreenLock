#include <screen_locker.h>
#include <user_data.h>
#include <arg.h>
#include <config.h>
#include <unistd.h>
#include <util.h>
#include <helpers.h>

struct xrandr rr;
struct lock **locks;
Display *dpy;
int s, nlocks, nscreens;

void run_ui() {
	return;
	if (!(dpy = XOpenDisplay(NULL)))
		die("slock: cannot open display\n");

	/* check for Xrandr support */
	rr.active = XRRQueryExtension(dpy, &rr.evbase, &rr.errbase);

	/* get number of screens in display "dpy" and blank them */
	nscreens = ScreenCount(dpy);
	if (!(locks = calloc(nscreens, sizeof(struct lock *))))
		die("slock: out of memory\n");
	for (nlocks = 0, s = 0; s < nscreens; s++) {
		if ((locks[s] = lockscreen(dpy, &rr, s)) != NULL)
			nlocks++;
		else
			break;
	}
	XSync(dpy, 0);

	/* did we manage to lock everything? */
	if (nlocks != nscreens)
		exit(1);
}

int main(int argc, char **argv) {

#ifdef __linux__
	dontkillme();
#endif

	struct user_data* ud = init_user_data();

	/* drop_privileges(); */

	/* run_ui(); */

	/* everything is now blank. Wait for the correct password */
	/* readpw(dpy, &rr, locks, nscreens, hash); */

	free_user_data(ud);
	return 0;
}

