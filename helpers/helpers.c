#include <helpers.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

void die(const char *errstr, ...) {
	va_list ap;

	va_start(ap, errstr);
	vfprintf(stderr, errstr, ap);
	va_end(ap);
	exit(1);
}

void dontkillme(void) {
	FILE *f;
	const char oomfile[] = "/proc/self/oom_score_adj";

	if (!(f = fopen(oomfile, "w"))) {
		if (errno != ENOENT)
			die("fopen %s: %s\n", oomfile, strerror(errno));
	}

	fprintf(f, "%d", OOM_SCORE_ADJ_MIN);

	if (fclose(f)) {
		if (errno == EACCES)
			die("Unable to disable OOM killer. "
			    "Make sure to run with sudo.\n");
		else
			die("fclose %s: %s\n", oomfile, strerror(errno));
	}
}
