#ifndef TRIE_H
#define TRIE_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define MIN(a, b) (((a) < (b)) ? (a) : (b))

#define ALPHABET_SIZE 26
// Estimated number of words (based on task assumptions)
#define MAX_WORDS 2 << 17

// Maximum length of the word
#define MAX_LEN 100000

// typedef int word_index_type;
typedef int number_type;

typedef struct node
{
	char *contents;
	number_type number; // Number == -1 means that the node has not been
			    // asigned an id
	struct node *parent;
	struct node *children[ALPHABET_SIZE]; // Child for every a-z character
} node, *nodep;

number_type insert(char *word);
int prev(int word_number, int start, int end);
int find(char *label);
int delete (number_type number);
int node_child_count(nodep nodeptr);

// Edge modification
void edge_merge(nodep parent, nodep current, nodep child, char edge);
void edge_change(nodep node, char *new_contents, nodep parent);
void edge_remove(nodep node, char letter);
void edge_add(nodep parent, nodep child);

void init_trie();
void clear();
void delete_root();
nodep node_alloc(nodep parent, char *parent_contents, number_type number);
void node_free(nodep nodeptr);
void node_free_children(nodep nodeptr);

void print_node_count();
number_type increase_node_number();

#endif /* TRIE_H */
