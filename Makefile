SHELL := /bin/bash

# Colors
OK_COLOR=\x1b[32;01m
NO_COLOR=\x1b[0m

# CXX=cc
CXX=gcc
CXX_FLAGS=-O3 -Wall -std=gnu99 
DEBUG_FLAGS=-g

ECHO=echo -e

all: clean dictionary

debug: clean parse.o trie.o dictionary.o
	$(CXX) $(CXX_FLAGS) $(DEBUG_FLAGS) parse.o trie.o dictionary.o -o dictionary.dbg
	@$(ECHO) "$(OK_COLOR)Created debug file: dictionary.dbg$(NO_COLOR)"

dictionary: parse.o trie.o dictionary.o
	$(CXX) $(CXX_FLAGS) parse.o trie.o dictionary.o -o dictionary 
	@$(ECHO) "$(OK_COLOR)Project compilation successful$(NO_COLOR)"

dictionary.o: dictionary.c
	$(CXX) $(CXX_FLAGS) -c dictionary.c

parse.o: parse.c parse.h
	$(CXX) $(CXX_FLAGS) -c parse.c

trie.o: trie.c trie.h
	$(CXX) $(CXX_FLAGS) -g -c trie.c

clean:
	@$(ECHO) "$(OK_COLOR)Project cleaned$(NO_COLOR)"
	@rm -f dictionary dictionary.dbg $(wildcard *.o) $(wildcard *.h.gch)

.PHONY: clean
