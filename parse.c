#include <stdbool.h>
#include "parse.h"

// Helper functions

// Checks if pre is prefix of str
bool prefix(const char *pre, const char *str)
{
	return strncmp(pre, str, strlen(pre)) == 0;
}

// Check is a char is space (and space only)
inline int is_sp(char a) { return a == ' '; }

// Parsing functions

// Parses number into an int
// Returns -1 in case of error, number otherwise
int parse_number(char *input)
{
	int i = 0;
	if (input[0] == '\n')
		return -1;

	// Accept '0', but not '000', etc.
	if (input[0] == '0') {
		i++;
		while (input[i] == ' ')
			i++;

		return input[i] == '\n' ? 0 : -1;
	}
	int parse_res = 0;
	int len = strlen(input);
	for (i = 0; i < len; ++i) {
		if (!isdigit(input[i]))
			break;

		parse_res *= 10;
		parse_res += input[i] - '0';
	}

	if (i > MAX_ARG_LEN)
		return -1;

	// Check rest of the string
	while (input[i] == ' ')
		i++;
	if (input[i] != '\n')
		return -1;

	return parse_res;
}

// Parses single number into result structure args parameter,
// at a given index
int parse_single_number(char *input, int index, ParseResult *result)
{
	// Accept '0', but not '000', etc.
	int i = 0;
	if (input[0] == '0') {
		if (isspace(input[1])) {
			result->args[index] = 0;
			return 1;
		}
		return -1;
	}
	if (!isdigit(input[i]))
		return -1;

	int parse_res = 0;
	int len = strlen(input);
	for (; i < len; ++i) {
		if (!isdigit(input[i]))
			break;
		parse_res *= 10;
		parse_res += input[i] - '0';
	}

	if (i > MAX_ARG_LEN)
		return -1;

	result->args[index] = parse_res;

	return i;
}

int parse_number_args(char *input, ParseResult *result)
{
	int i = 0;

	if (input[0] == '\n')
		return -1;

	// Try to parse all 3 args
	for (int j = 0; j < 3; j++) {
		input += i;
		i = parse_single_number(input, j, result);
		if (i == -1)
			return -1;

		// Check rest of the string
		while (input[i] == ' ')
			i++;
	}
	if (input[i] != '\n')
		return -1;

	return 1;
}

// Returns 0 if the input was ill-formed,
// end position of the insert/find word otherwise
int parse_word(char *input)
{
	if (input[0] == '\n') // Empty string
		return 0;

	// Check if word contains space
	int p = 0;
	while (islower(input[p])) {
		p++;
	}

	int rest = p;
	while (is_sp(input[rest]))
		rest++;

	if (input[rest] != '\n')
		return 0;

	return p;
}

// Parses command together with its arguments, returns ParseResult enum
ParseResult parse_command(FILE *stream)
{
	ParseResult command = {QUIT, NULL, {-1, -1, -1}};
	char *input = NULL;

	char line[BUF_LEN];
	if (stream == NULL)
		return command;

	char *readline = fgets(line, BUF_LEN, stream);
	if (readline == NULL)
		return command;

	command.result = IGNORE;

	// Skip spaces on the beginning
	input = &line[0];
	while (input[0] == ' ')
		input++;

	if (!isalpha(input[0]))
		return command; // No command supplied

	switch (input[0]) {
	case 'i': // insert
		if (prefix("insert", input)) {
			input += (strlen("insert"));
			if (!is_sp(input[0]))
				return command;

			while (input[0] == ' ')
				input++;

			int end_pos = parse_word(input);
			if (!end_pos)
				return command;

			command.result = INSERT;
			command.arg =
			    (char *)malloc(end_pos + 1 * sizeof(char));
			strncpy(command.arg, input, end_pos);
			command.arg[end_pos] = 0;
		}
		break;
	case 'f': // find
		if (prefix("find", input)) {
			input += (strlen("find"));
			if (!is_sp(input[0]))
				return command;

			while (input[0] == ' ')
				input++;

			int end_pos = parse_word(input);
			if (!end_pos)
				return command;

			command.result = FIND;
			command.arg =
			    (char *)malloc(end_pos + 1 * sizeof(char));
			strncpy(command.arg, input, end_pos);
			command.arg[end_pos] = 0;
		}
		break;
	case 'c': // clear
		if (prefix("clear", input)) {
			input += (strlen("clear"));
			while (is_sp(input[0]))
				input++;

			if (input[0] == '\n')
				command.result = CLEAR;
		}
		break;
	case 'd': // delete
		if (prefix("delete", input)) {
			input += (strlen("delete"));
			if (!is_sp(input[0]))
				return command; // Check for space delimiter
			while (is_sp(input[0]))
				input++;
			int arg = parse_number(input);
			command.result = DELETE;
			command.args[0] = arg;
		}
		break;
	case 'p': // prev
		if (prefix("prev", input)) {
			input += (strlen("prev"));
			while (is_sp(input[0]))
				input++;
			int res = parse_number_args(input, &command);
			if (res != -1) {
				command.result = PREV;
			}
		}
		break;
	default:
		break;
	}
	return command;
}
