#include "search.h"

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define BUF_SIZE 20
#define WORD_SIZE 64

int isEndOfSentence(char c) {
  return (c == ' ' || c == '\n' || c == '\t' || c == '.' || c == ',');
}

int process_search_word(char *search, int *word_count, int fd) {
  int word_cap = 4;
  char buffer[BUF_SIZE];
  char *word = malloc(word_cap);
  if (!word) {
    perror("malloc");
    return -1;
  }

  int wpos = 0;
  ssize_t byte_read;
  *word_count = 0;

  while ((byte_read = read(fd, buffer, BUF_SIZE)) > 0) {
    for (int i = 0; i < byte_read; i++) {
      char c = buffer[i];

      if (isEndOfSentence(c)) {
        if (wpos > 0) {
          word[wpos] = '\0';
          if (strcmp(word, search) == 0)
            (*word_count)++;
          wpos = 0;
        }
      } else {
        if (wpos + 1 >= word_cap) {
          word_cap *= 2;
          char *tmp = realloc(word, word_cap);
          if (!tmp) {
            perror("realloc");
            free(word);
            return -1;
          }
          word = tmp;
        }

        if (wpos < WORD_SIZE - 1)
          word[wpos++] = c;
      }
    }
  }

  if (wpos > 0) {
    word[wpos] = '\0';
    if (strcmp(word, search) == 0)
      (*word_count)++;
  }

  free(word);
  return 0;
}
