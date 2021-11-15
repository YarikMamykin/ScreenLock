/* #include <screen_locker.h> */
#include <user_data.h>
#include <arg.h>
#include <config.h>
#include <unistd.h>
#include <util.h>
#include <helpers.h>
#include <locking_window.h>

int main(int argc, char **argv) {

#ifdef __linux__
	dontkillme();
#endif

	struct user_data* ud = init_user_data();

	const char* ui_errors = run_ui(ud);
	if(ui_errors != NULL) {
		printf("%s\n", ui_errors);
	}

	/* everything is now blank. Wait for the correct password */
	/* readpw(dpy, &rr, locks, nscreens, hash); */

	free_user_data(ud);
	return 0;
}

