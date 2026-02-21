#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define BUF_SIZE 20
#define WORD_SIZE 64

int isEndOfSentence(char c) {
  return (c == ' ' || c == '\n' || c == '\t' || c == '.' || c == ',');
}

int main(int argc, char *argv[]) {

  if (argc < 3) {
    fprintf(stderr,
            "Usage: %s [-i] <fichier> <mot_a_chercher>\n"
            "Options:\n"
            "  -i  recherche par sous-chaîne\n"
            "Example:\n"
            "  %s out/toto.txt toto\n"
            "  %s -i out/toto.txt nom\n",
            argv[0], argv[0], argv[0]);
    return 1;
  }

  int substring = 0;
  char *filename;
  char *search;

  if (strcmp(argv[1], "-i") == 0) {
    substring = 1;
    if (argc < 4) {
      fprintf(stderr, "Usage avec -i : %s -i <fichier> <mot>\n", argv[0]);
      return 1;
    }
    filename = argv[2];
    search = argv[3];
  } else {
    filename = argv[1];
    search = argv[2];
  }

  int fd = open(filename, O_RDONLY);
  if (fd == -1) {
    perror("open");
    return 1;
  }

  char buffer[BUF_SIZE];
  char word[WORD_SIZE];
  char word_found[WORD_SIZE];
  int wpos = 0;
  ssize_t byte_read;
  int word_count = 0;
  int line_number = 1;

  while ((byte_read = read(fd, buffer, BUF_SIZE)) > 0) {
    for (int i = 0; i < byte_read; i++) {
      char caracter = buffer[i];

      if (caracter == '\n')
        line_number++;

      if (isEndOfSentence(caracter)) {
        if (wpos > 0) {
          word[wpos] = '\0';

          int found = 0;
          if (substring) {
            if (strstr(word, search) != NULL)
              found = 1;
          } else {
            if (strcmp(word, search) == 0)
              found = 1;
          }

          if (found) {
            word_count++;
            strcpy(word_found, word);
            printf("Mot trouvé : '%s' à la ligne %d\n", word, line_number);
          }

          wpos = 0;
        }
      } else {
        if (wpos < WORD_SIZE - 1) {
          word[wpos++] = caracter;
        }
      }
    }
  }

  if (wpos > 0) {
    word[wpos] = '\0';

    int found = 0;
    if (substring) {
      if (strstr(word, search) != NULL)
        found = 1;
    } else {
      if (strcmp(word, search) == 0)
        found = 1;
    }

    if (found) {
      word_count++;
      strcpy(word_found, word);
      printf("Mot trouvé : '%s' à la ligne %d\n", word, line_number);
    }
  }

  if (word_count > 0)
    printf("\nNombre total d'occurrences : %d\n", word_count);
  else
    printf("Mot '%s' non trouvé\n", search);

  close(fd);
  return 0;
}
