#pragma once
#ifdef __linux__
#include <fcntl.h>
#include <linux/oom.h>
#endif

void die(const char *errstr, ...);
void dontkillme(void);
void usage(void);
