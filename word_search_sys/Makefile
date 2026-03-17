CC      = gcc
CFLAGS  = -Wall -Wextra -std=c11 -g -Iinclude

SRC_DIR = src
OBJ_DIR = obj
BIN_DIR = bin

SHARED_OBJS = $(OBJ_DIR)/search.o $(OBJ_DIR)/word_utils.o

.PHONY: all clean

all: $(BIN_DIR)/word_search $(BIN_DIR)/word_occurence



$(BIN_DIR)/word_search: $(OBJ_DIR)/word_search_sys.o $(SHARED_OBJS)
	$(CC) $(CFLAGS) -o $@ $^

$(BIN_DIR)/word_occurence: $(OBJ_DIR)/word_occurence.o $(SHARED_OBJS)
	$(CC) $(CFLAGS) -o $@ $^



$(OBJ_DIR)/word_search_sys.o: $(SRC_DIR)/word_search_sys.c include/search.h
	$(CC) $(CFLAGS) -c -o $@ $<

$(OBJ_DIR)/word_occurence.o: $(SRC_DIR)/word_occurence.c include/word_utils.h
	$(CC) $(CFLAGS) -c -o $@ $<

$(OBJ_DIR)/search.o: $(SRC_DIR)/search.c include/search.h
	$(CC) $(CFLAGS) -c -o $@ $<

$(OBJ_DIR)/word_utils.o: $(SRC_DIR)/word_utils.c include/word_utils.h
	$(CC) $(CFLAGS) -c -o $@ $<



clean:
	rm -rf $(OBJ_DIR)/*.o $(BIN_DIR)/*
