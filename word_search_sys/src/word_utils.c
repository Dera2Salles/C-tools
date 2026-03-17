#define _DEFAULT_SOURCE
#include "word_utils.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define WORD_SIZE 64



static int is_delimiter(char c) {
  return (c == ' ' || c == '\n' || c == '\t' || c == '.' || c == ',');
}


void clean_word(char *word) {
  int j = 0;
  for (int i = 0; word[i]; i++) {
    if (isalnum((unsigned char)word[i])) {
      word[j++] = tolower((unsigned char)word[i]);
    }
  }
  word[j] = '\0';
}

char *find_word(int fd, off_t *cursor) {
  char *word = malloc(WORD_SIZE);
  if (!word) {
    perror("malloc");
    exit(1);
  }

  int wpos = 0;
  int capacity = WORD_SIZE;
  char c;
  ssize_t byte_read;

  while ((byte_read = read(fd, &c, 1)) > 0) {
    if (is_delimiter(c)) {
      if (wpos > 0) {
        word[wpos] = '\0';
        clean_word(word);
        if (strlen(word) == 0) {
          wpos = 0;
          continue;
        }
        if (cursor)
          *cursor = lseek(fd, 0, SEEK_CUR);
        return word;
      }
    } else {
      if (wpos >= capacity - 1) {
        capacity *= 2;
        char *tmp = realloc(word, capacity);
        if (!tmp) {
          free(word);
          perror("realloc");
          exit(1);
        }
        word = tmp;
      }
      word[wpos++] = c;
    }
  }

  if (wpos > 0) {
    word[wpos] = '\0';
    clean_word(word);
    if (strlen(word) == 0) {
      free(word);
      return NULL;
    }
    if (cursor)
      *cursor = lseek(fd, 0, SEEK_CUR);
    return word;
  }

  free(word);
  return NULL;
}

int find_or_add_word(WordStruc **word_counts, int *size, int *capacity,
                     char *word) {
  for (int i = 0; i < *size; i++) {
    if (strcmp((*word_counts)[i].word, word) == 0) {
      return i;
    }
  }

  if (*size >= *capacity) {
    *capacity *= 2;
    *word_counts = realloc(*word_counts, (*capacity) * sizeof(WordStruc));
    if (!*word_counts) {
      perror("realloc");
      exit(1);
    }
  }

  (*word_counts)[*size].word = strdup(word);
  (*word_counts)[*size].occurence = 0;
  (*size)++;

  return *size - 1;
}
