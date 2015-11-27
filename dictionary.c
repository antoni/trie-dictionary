#include <ctype.h>
#include <getopt.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "trie.h"
#include "parse.h"

int show_node_count = 0;

inline void ignore_command()
{
	show_node_count = 0;
	printf("ignored\n");
}

inline void find_print(int found)
{
	show_node_count = 0;
	printf("%s\n", found == -1 ? "NO" : "YES");
}

int main(int argc, char **argv)
{
	bool verbose = false;
	int c, ins = -1;
	ParseResult command;

	while ((c = getopt(argc, argv, "v")) != -1)
		if (c == 'v')
			verbose = true;

	while (true) {
		if (verbose)
			show_node_count = 1;

		command = parse_command(stdin);
		switch (command.result) {
		case IGNORE:
			ignore_command();
			break;
		case INSERT:
			ins = insert(command.arg);
			if (ins == -1)
				ignore_command();
			else
				printf("word number: %d\n", ins);
			break;
		case DELETE:
			ins = delete (command.args[0]);
			if (ins == -1)
				ignore_command();
			else
				printf("deleted: %d\n", ins);
			break;
		case PREV:
			ins = prev(command.args[0], command.args[1],
				   command.args[2]);
			if (ins == -1)
				ignore_command();
			else
				printf("word number: %d\n", ins);

			break;
		case FIND:
			ins = find(command.arg);
			find_print(ins);
			break;
		case CLEAR:
			clear();
			printf("cleared\n");
			break;
		case QUIT:
			clear();
			return 0;
			break;
		default:
			ignore_command();
			break;
		}

		if (show_node_count)
			print_node_count();
		if (command.arg != NULL)
			free(command.arg);
	}
	return 0;
}
