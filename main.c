#include <user_data.h>
#include <arg.h>
#include <unistd.h>
#include <util.h>
#include <helpers.h>
#include <locking_window.h>

int main(int argc, char **argv) {

	struct user_data* ud = init_user_data(getuid());

	const char* ui_errors = run_ui(ud);
	if(ui_errors != NULL) {
		printf("%s\n", ui_errors);
	}

	free_user_data(ud);
	return 0;
}

