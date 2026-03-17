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

int process_search_word(char *search, int *word_count, int fd, int fd_out) {

  char buffer[BUF_SIZE];
  char *word = malloc(1);
  if (!word) {
    perror("malloc");
    return -1;
  }

  int word_cap = 4;
  int wpos = 0;
  ssize_t byte_read;

  while ((byte_read = read(fd, buffer, BUF_SIZE)) > 0) {
    for (int i = 0; i < byte_read; i++) {
      char caracter = buffer[i];
      if (isEndOfSentence(caracter)) {
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
            break;
          }
          word = tmp;
        }
        if (wpos < WORD_SIZE - 1)
          word[wpos++] = caracter;
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

int main(int argc, char *argv[]) {
  if (argc < 3) {
    fprintf(stderr,
            "Usage: %s [-o <output>] <fichier> <mot_a_chercher>\n"
            "Options:\n"
            "  -o <output>   écrire le résultat dans un fichier\n"
            "Example:\n"
            "  %s out/toto.txt toto\n"
            "  %s -o result.txt out/toto.txt toto\n",
            argv[0], argv[0], argv[0]);
    return 1;
  }

  char *filename = NULL;
  char *search = NULL;
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
    } else if (search == NULL) {
      search = argv[i];
      i++;
    } else {
      i++;
    }
  }

  if (filename == NULL || search == NULL) {
    fprintf(stderr, "Erreur : fichier ou mot manquant\n");
    return 1;
  }

  int fd_in = open(filename, O_RDONLY);
  if (fd_in == -1) {
    perror("open input");
    return 1;
  }

  int fd_out = -1;

  if (output_file != NULL) {
    fd_out = open(output_file, O_WRONLY | O_CREAT | O_APPEND, 0644);
    if (fd_out == -1) {
      perror("open output");
      close(fd_in);
      return 1;
    }
  }

  int word_count = 0;

  if (process_search_word(search, &word_count, fd_in, fd_out) != 0) {
    close(fd_in);
    close(fd_out);
    exit(1);
  }

  char line_buf[256];
  snprintf(line_buf, sizeof(line_buf),
           "Mot : %s ,fichier : %s, occurence : %d\n ", search, filename,
           word_count);

  if (fd_out != -1)
    write(fd_out, line_buf, strlen(line_buf));

  close(fd_in);

  if (fd_out != -1)
    close(fd_out);
  return 0;
}
