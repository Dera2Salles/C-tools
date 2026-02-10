#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define BUF_SIZE 64
#define WORD_SIZE 64

int isSeparator(char c) {
  return (c == ' ' || c == '\n' || c == '\t' || c == '.' || c == ',');
}

int main(int argc, char *argv[]) {

  if (argc < 3) {
    fprintf(stderr,
            "Usage: %s <fichier> <mot_a_supprimer>\n"
            "Example:\n"
            "  %s out/toto.txt dera\n",
            argv[0], argv[0]);
    return 1;
  }

  int fd = open(argv[1], O_RDONLY);
  if (fd == -1) {
    perror("open input");
    return 1;
  }

  int tmp = open("temp.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);
  if (tmp == -1) {
    perror("open temp");
    close(fd);
    return 1;
  }

  char buffer[BUF_SIZE];
  char word[WORD_SIZE];
  int wpos = 0;
  ssize_t bytes;

  while ((bytes = read(fd, buffer, BUF_SIZE)) > 0) {
    for (int i = 0; i < bytes; i++) {
      char caracter = buffer[i];

      if (isSeparator(caracter)) {
        if (wpos > 0) {
          word[wpos] = '\0';

          if (strcmp(word, argv[2]) == 0) {
            write(tmp, "", 0);
          } else {
            write(tmp, word, strlen(word));
          }

          wpos = 0;
        }

        write(tmp, &caracter, 1);

      } else {
        if (wpos < WORD_SIZE - 1) {
          word[wpos++] = caracter;
        }
      }
    }
  }

  if (wpos > 0) {
    word[wpos] = '\0';
    if (strcmp(word, argv[2]) == 0) {
      write(tmp, " ", 1);
    } else {
      write(tmp, word, strlen(word));
    }
  }

  close(fd);
  close(tmp);

  unlink(argv[1]);
  rename("temp.txt", argv[1]);

  return 0;
}
