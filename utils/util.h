#pragma once
#undef explicit_bzero
void explicit_bzero(void *, size_t);

struct xrandr {
	int active;
	int evbase;
	int errbase;
};
