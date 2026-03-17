#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "word_utils.h"

int main(int argc, char *argv[]) {
  if (argc < 2) {
    fprintf(stderr,
            "Usage: %s [-o <output>] <filename>\n"
            "Options:\n"
            "  -o <output>   écrire le résultat dans un fichier\n"
            "Example:\n"
            "  %s out/toto.txt\n"
            "  %s -o result.txt out/toto.txt\n",
            argv[0], argv[0], argv[0]);
    return 1;
  }

  char *filename = NULL;
  char *output_file = NULL;

  int i = 1;
  while (i < argc) {
    if (strcmp(argv[i], "-o") == 0) {
      if (i + 1 >= argc) {
        fprintf(stderr, "Erreur : -o nécessite un nom de fichier\n");
        return 1;
      }
      output_file = argv[i + 1];
      i += 2;
    } else if (filename == NULL) {
      filename = argv[i];
      i++;
    } else {
      i++;
    }
  }

  if (filename == NULL) {
    fprintf(stderr, "Erreur : fichier manquant\n");
    return 1;
  }

  int fd_in = open(filename, O_RDONLY);
  if (fd_in == -1) {
    perror("open input");
    return 1;
  }

  int table_capacity = INITIAL_TABLE_SIZE;
  int table_size = 0;
  WordStruc *word_counts = malloc(table_capacity * sizeof(WordStruc));
  if (!word_counts) {
    perror("malloc");
    close(fd_in);
    return 1;
  }

  char *word;
  off_t cursor = lseek(fd_in, 0, SEEK_CUR);

  while ((word = find_word(fd_in, &cursor)) != NULL) {
    int index =
        find_or_add_word(&word_counts, &table_size, &table_capacity, word);
    word_counts[index].occurence++;
    free(word);
  }

  if (table_size == 0) {
    printf("Aucun mot trouvé dans le fichier.\n");
    free(word_counts);
    close(fd_in);
    return 0;
  }

  int max_index = 0;
  for (int j = 1; j < table_size; j++) {
    if (word_counts[j].occurence > word_counts[max_index].occurence) {
      max_index = j;
    }
  }

  char result_buf[256];
  snprintf(result_buf, sizeof(result_buf),
           "Le mot le plus fréquent : '%s' avec %d occurrence(s) dans le "
           "fichier %s\n",
           word_counts[max_index].word, word_counts[max_index].occurence,
           filename);

  if (output_file != NULL) {
    int fd_out = open(output_file, O_WRONLY | O_CREAT | O_APPEND, 0644);
    if (fd_out == -1) {
      perror("open output");
    } else {
      write(fd_out, result_buf, strlen(result_buf));
      close(fd_out);
    }
  } else {
    printf("%s", result_buf);
  }

  for (int j = 0; j < table_size; j++) {
    free(word_counts[j].word);
  }
  free(word_counts);
  close(fd_in);

  return 0;
}
