#pragma once
#include <grp.h>
#include <pwd.h>
#include <stdbool.h>

struct user_data {
	struct group *grp;
	struct passwd *pwd;
	const char* hash;
	bool no_password;
};

struct user_data* init_user_data(uid_t uid);
void free_user_data(struct user_data* ud);

void drop_privileges(struct user_data* ud);
