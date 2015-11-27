#ifndef PARSE_H
#define PARSE_H

#include <ctype.h>
#include <limits.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUF_LEN 100000

// Maximum length (digits) of a number
#define MAX_ARG_LEN 6

// Command parsing results
typedef enum { INSERT, DELETE, PREV, FIND, CLEAR, IGNORE, QUIT } CommandResult;

typedef struct
{
	CommandResult result;
	char *
	    arg; // Command string argument, if needed (insert or find commands)
	int args[3]; // Numeric arguments
} ParseResult;

inline void print_command_info(ParseResult *res)
{
	const char *command_strings[] = {"insert", "delete", "prev", "find",
					 "clear",  "ignore", "quit"};
	printf("Command:%s\narg:%s\n%d %d %d\n", command_strings[res->result],
	       res->arg != NULL ? res->arg : "EMPTY", res->args[0],
	       res->args[1], res->args[2]);
}

// Helper functions
int parse_word(char *input);
int parse_single_number(char *input, int index, ParseResult *result);
int parse_number_args(char *input, ParseResult *result);

// Parses single command
ParseResult parse_command(FILE *stream);

// Prints ignore message
void ignore_command();

#endif
