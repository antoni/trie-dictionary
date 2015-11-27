#include <stdlib.h>
#include <string.h>

#include "trie.h"

char buffer[MAX_LEN];

// Current count of nodes
number_type curr_count = 0;

nodep root = NULL;

// ID of current node
number_type curr_number = 0;

// Holds pointers to a label with id == curr_words index
nodep curr_words[MAX_WORDS];

// Helper functions

// Returns index at which child should be stored
int inline letter_index(char letter) { return letter - 'a'; }

// Reverse a string
inline void strrev(char *p)
{
	char *q = p;
	while (q && *q)
		++q;
	for (--q; p < q; ++p, --q)
		*p = *p ^ *q, *q = *p ^ *q, *p = *p ^ *q;
}

// Finds the whole reconstructed label to be inserted, saves it to the buffer
inline void find_label(number_type label_number)
{
	nodep start_node = curr_words[label_number];
	int pos = 0;
	while (start_node->parent != NULL) {
		// we will break the loop after getting to the root
		int len = strlen(start_node->contents) - 1;
		while (len >= 0) {
			buffer[pos] = start_node->contents[len];
			++pos;
			len--;
		}
		start_node = start_node->parent;
	}
	buffer[pos] = 0; // terminate the string
	// Reverse the string found
	strrev(buffer);
}
// Returns unincreased node number, then increases it
number_type increase_node_number()
{
	int current = curr_number;
	curr_number++;
	return current;
}

// Prints node count
void print_node_count() { fprintf(stderr, "nodes: %d\n", curr_count); }

// Allocated new node with the contents (its edge) defined by parent_contents,
// with a given number
nodep node_alloc(nodep parent, char *parent_contents, number_type number)
{
	nodep new_node = (nodep)malloc(sizeof(node));
	if (parent != NULL)
		new_node->contents = strdup(parent_contents);
	else
		new_node->contents = NULL;

	new_node->parent = parent;
	new_node->number = number; // Asign number
	curr_count++;		   // Increase node count

	// Set al children to NULL
	for (int i = 0; i < ALPHABET_SIZE; i++)
		new_node->children[i] = NULL;

	return new_node;
}

// Free node's contents, then the node. Decrease nodes count
void node_free(nodep nodeptr)
{
	if (nodeptr == NULL)
		return;

	free(nodeptr->contents);
	free(nodeptr);
	curr_count--;
}

// Frees the children (whole subtree)
void node_free_children(nodep nodeptr)
{
	if (nodeptr == NULL)
		return;

	for (int i = 0; i < ALPHABET_SIZE; i++)
		if (nodeptr->children[i] != NULL)
			node_free_children(nodeptr->children[i]);
	node_free(nodeptr);
}

// Inits root node, setting current node count to 1
void init_trie()
{
	if (root == NULL) {
		root = node_alloc(NULL, NULL, -1);
		curr_count = 1;
	}
}

// Deletes root node and all its children, sets all curr_words to NULL (no
// labels in trie)
void delete_root()
{
	for (int i = 0; i < ALPHABET_SIZE; i++)
		if (root->children[i] != NULL)
			curr_words[root->children[i]->number] = NULL;
	node_free_children(root);
	root = NULL;
}

// Clears the trie, without displaying info
void clear()
{
	node_free_children(root);
	root = NULL;
	curr_number = 0;
	for (int i = 0; i < MAX_WORDS; i++)
		curr_words[i] = NULL;
}

// Edge modification

// Adds an edge (i.e. child to a parent)
void edge_add(nodep parent, nodep child)
{
	char letter = child->contents[0];
	parent->children[letter_index(letter)] = child;
}

// Removes child at a give letter index
void edge_remove(nodep node, char letter)
{
	node->children[letter_index(letter)] = NULL;
}

// Changes edge to new_contents
void edge_change(nodep node, char *new_contents, nodep parent)
{
	free(node->contents);
	node->contents = strdup(new_contents);
	node->parent = parent;
}

// Finds the only child of current node, merges its contents, then deletes
// current node
void edge_merge(nodep parent, nodep current, nodep child, char edge)
{
	// Find the child
	for (int i = 0; i < ALPHABET_SIZE; ++i)
		if (current->children[i] != NULL) {
			child = current->children[i];
			break;
		}

	strcpy(buffer, current->contents);
	strcat(buffer, child->contents);
	edge_remove(parent, edge);
	edge_remove(current, child->contents[0]);
	edge_change(child, buffer, parent);
	edge_add(parent, child);
	node_free(current);
}

// Returns number of children a node has
int node_child_count(nodep nodeptr)
{
	int child_count = 0;
	for (int i = 0; i < ALPHABET_SIZE; i++)
		if (nodeptr->children[i] != NULL)
			child_count++;
	return child_count;
}

// Inserts a label into the trie, returns -1 if word was in the trie, new word
// number otherwise
int insert(char *label)
{
	// init trie if no nodes have been inserted
	init_trie();

	nodep curr_node = root;
	int index;
	int pos = 0;
	int len = strlen(label);
	char *contents;

	while (true) {
		if (pos == len) {
			if (curr_node->number !=
			    -1) { // Label already in the trie
				return -1;
			} else { // Mark node as a label
				number_type node_number =
				    increase_node_number();
				curr_node->number = node_number;
				curr_words[node_number] = curr_node;
				return curr_node->number;
			}
		} else {
			index = letter_index(label[pos]);
			if (curr_node->children[index] != NULL) {
				contents = curr_node->children[index]->contents;
			} else {
				contents = NULL;
			}
			if (contents == NULL) {
				// Add new child
				nodep new_node =
				    node_alloc(curr_node, label + pos,
					       increase_node_number());
				edge_add(curr_node, new_node);
				curr_words[new_node->number] = new_node;
				return new_node->number;
			} else {
				// Insert node inbetween (split nodes edge)
				int clen = strlen(contents);
				for (int i = 0; i < clen; ++i) {
					if (pos == len) {
						// Split *current* edge
						char *contents_new =
						    calloc(i + 1, sizeof(char));
						char *contents_old = calloc(
						    strlen(contents) - i + 1,
						    sizeof(char));

						strncpy(contents_new, contents,
							i);
						strcpy(contents_old,
						       contents + i);

						nodep next_node =
						    curr_node->children[index];
						nodep new_node = node_alloc(
						    curr_node, contents_new,
						    increase_node_number());
						curr_words[new_node->number] =
						    new_node;
						edge_change(next_node,
							    contents_old,
							    new_node);
						edge_add(new_node, next_node);
						curr_node->children[index] =
						    new_node;

						free(contents_new);
						free(contents_old);

						return new_node->number;
					} else if (label[pos] != contents[i]) {
						// Split node into two, leaving
						// a parent
						char *contents_new =
						    calloc(i + 1, sizeof(char));
						char *contents_old = calloc(
						    strlen(contents) - i + 1,
						    sizeof(char));

						strncpy(contents_new, contents,
							i);
						strcpy(contents_old,
						       contents + i);

						nodep next_node =
						    curr_node->children[index];
						nodep parent = node_alloc(
						    curr_node, contents_new,
						    -1);

						edge_change(next_node,
							    contents_old,
							    parent);
						edge_add(parent, next_node);
						curr_node->children[index] =
						    parent;

						nodep new_node = node_alloc(
						    parent, label + pos,
						    increase_node_number());
						curr_words[new_node->number] =
						    new_node;

						edge_add(parent, new_node);

						free(contents_new);
						free(contents_old);

						return new_node->number;
					} else {
						pos++;
					}
				}
			}
		}
		curr_node = curr_node->children[index];
	}
}

// Reconstructs the label  with given number and tries to insert label given
// by <start, end positions>
int prev(int label_number, int start, int end)
{
	if (curr_words[label_number] == NULL || start > end)
		return -1;

	// Save reconstructed (bottom to top) label to buffer
	find_label(label_number);

	// The part requested is longer than the label
	if (end >= (int)strlen(buffer))
		return -1;

	char *insert_label = calloc(end - start + 2, sizeof(char));
	strncpy(insert_label, buffer + start, end - start + 1);
	// Try to insert the new label
	int result = insert(insert_label);
	free(insert_label);
	return result;
}

// Finds a label in the trie
// Return -1 if label has not been found, 1 otherwise
int find(char *label)
{
	int pos = 0;
	nodep node = root;
	int len = strlen(label);
	while (node != NULL) {
		int index = letter_index(label[pos]);
		if (node->children[index] != NULL) {
			char *contents = node->children[index]->contents;
			if (contents == NULL)
				return -1;

			// Check what part of the contents is part of our string
			int ll = strlen(contents);
			for (int i = 0; i < ll; ++i) {
				if (label[pos] != contents[i])
					return -1;
				pos++;
				if (pos == len) // label found
					return 1;
			}
		}
		node = node->children[index];
	}
	// We have found NULL child (and still searching)
	return -1;
}

// Returns number of deleted label (== number), -1 otherwise
int delete (number_type number)
{
	if (number >= MAX_WORDS || curr_words[number] == NULL)
		return -1;

	if (curr_count == 2) {
		delete_root();
		return number;
	}

	nodep node = curr_words[number];
	node->number = -1;

	nodep parent = node->parent;
	nodep child = NULL;
	char edge = node->contents[0];
	curr_words[number] = NULL;

	int children = node_child_count(node);
	// Node with no children
	if (children == 0) {
		edge_remove(parent, edge);
		node_free(node);
	} else if (children == 1) {
		// Merge node contents with child contents
		edge_merge(parent, node, child, edge);
	}

	// Node has more children
	// Parent node is not a separte label -> merge with its parent
	if (parent->number == -1 && parent->parent != NULL &&
	    node_child_count(parent) < 2) {
		nodep pparent = parent->parent;
		edge = parent->contents[0];
		if (node_child_count(parent) == 0) {
			edge_remove(pparent, edge);
			node_free(parent);
			return number;
		}

		// Merge parent contents with node contents
		edge_merge(pparent, parent, node, edge);
	}
	return number;
}
