#include <shadow.h>
#include <string.h>
#include <unistd.h>
#include <user_data.h>
#include <stdlib.h>
#include <errno.h>
#include <helpers.h>

struct user_data* init_user_data(uid_t uid) {

	struct user_data* ud = (struct user_data*)calloc(1, sizeof(struct user_data));

	if(ud == NULL) {
		die("Cannot allocate user data...");
	}

	errno = 0;
	if (!(ud->pwd = getpwuid(uid)))
	{
		free_user_data(ud);
		die("getpwuid %d: %s\n", uid,
				errno ? strerror(errno) : "user entry not found");
	}

	ud->hash = ud->pwd->pw_passwd;

#if HAVE_SHADOW_H
	if (!strcmp(ud->hash, "x")) {
		struct spwd *sp;
		if (!(sp = getspnam(ud->pwd->pw_name))) {
			free_user_data(ud);
			die("getspnam: cannot retrieve shadow entry. "
					"Make sure to suid or sgid .\n");
		}
		ud->hash = sp->sp_pwdp;
	}
#else
	if (!strcmp(ud->hash, "*")) {
		free_user_data(ud);
		die("getpwuid: cannot retrieve shadow entry. "
		    "Make sure to suid or sgid .\n");
	}
#endif 

	ud->no_password = strlen(ud->hash) == 0;

	drop_privileges(ud);
	return ud;
}

void free_user_data(struct user_data* ud) {
	free(ud);
}

void drop_privileges(struct user_data* ud) {

	if (setgroups(0, NULL) < 0) {
		free_user_data(ud);
		die("setgroups: %s\n", strerror(errno));
	}

	if (setgid(ud->pwd->pw_gid) < 0) {
		free_user_data(ud);
		die("setgid: %s\n", strerror(errno));
	}

	if (setuid(ud->pwd->pw_uid) < 0) {
		free_user_data(ud);
		die("setuid: %s\n", strerror(errno));
	}
}

