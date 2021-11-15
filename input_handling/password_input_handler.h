#pragma once

struct password_input_handler {
	char* input;
	unsigned short inserted_chars;
	const char* approved_hash;
	const unsigned short max_input_len;
};

void free_password_input(struct password_input_handler* pih);
void reset_password_input(struct password_input_handler* pih);
void update_password_input(struct password_input_handler* pih, char input_char);
int password_input_match(struct password_input_handler* pih);
struct password_input_handler* init_password_input_handler(const char* hash);
