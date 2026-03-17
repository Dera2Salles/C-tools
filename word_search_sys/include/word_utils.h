#ifndef WORD_UTILS_H
#define WORD_UTILS_H

#include <fcntl.h>
#include <sys/types.h>

typedef struct {
  char *word;
  int occurence;
} WordStruc;

#define INITIAL_TABLE_SIZE 20

void clean_word(char *word);

char *find_word(int fd, off_t *cursor);

int find_or_add_word(WordStruc **word_counts, int *size, int *capacity,
                     char *word);

#endif
