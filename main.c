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

	uid_t uid = -1;
	printf("UID->");
	scanf("%u", &uid);
	struct user_data* ud = init_user_data(uid);

	const char* ui_errors = run_ui(ud);
	if(ui_errors != NULL) {
		printf("%s\n", ui_errors);
	}

	free_user_data(ud);
	return 0;
}

