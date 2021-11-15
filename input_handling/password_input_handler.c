#include <password_input_handler.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

void free_password_input(struct password_input_handler* pih) {
	free(pih->input);
	free(pih);
}

void reset_password_input(struct password_input_handler* pih) {
	free(pih->input);
	pih->input = (char*)calloc(pih->max_input_len, sizeof(char));
	pih->inserted_chars = 0;
}

void update_password_input(struct password_input_handler* pih, char input_char) {

	if(pih->inserted_chars < pih->max_input_len) {
		pih->input[pih->inserted_chars++] = input_char;
		return;
	}
}

int password_input_match(struct password_input_handler* pih) {
	const char* input_hash = crypt(pih->input, pih->approved_hash);
	return strcmp(input_hash, pih->approved_hash) == 0;
}

struct password_input_handler* init_password_input_handler(const char* hash) {
	struct password_input_handler* pih = (struct password_input_handler*)calloc(1, sizeof(struct password_input_handler));
	pih->max_input_len = 32;
	pih->input = (char*)calloc(pih->max_input_len, sizeof(char));
	pih->inserted_chars = 0;
	pih->approved_hash = hash;
	return pih;
}

