#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define BUF_SIZE 32

int isSeparator(char c) {
  return (c == ' ' || c == '\n' || c == '\t' || c == '.' || c == ',');
}

int main(int argc, char *argv[]) {
  if (argc < 4) {
    fprintf(stderr,
            "Usage: %s <fichier> <mot_a_chercher> <mot_de_remplacement>\n"
            "Example:\n  %s out/toto.txt toto niov\n",
            argv[0], argv[0]);
    return 1;
  }

  char *filename = argv[1];
  char *search = argv[2];
  char *replace = argv[3];

  int fd = open(filename, O_RDONLY);
  int tmp = open("temp.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);
  if (fd == -1 || tmp == -1) {
    perror("open");
    return 1;
  }

  char buffer[BUF_SIZE];
  char *word = malloc(1);
  int wpos = 0;
  int word_size = 1;
  ssize_t byte_read;
  int mpos = 0;
  int slen = strlen(search);

  while ((byte_read = read(fd, buffer, BUF_SIZE)) > 0) {
    for (int i = 0; i < byte_read; i++) {
      char caracter = buffer[i];

      if (isSeparator(caracter)) {
        if (wpos > 0) {
          word[wpos] = '\0';
          if (strcmp(word, search) == 0) {
            write(tmp, replace, strlen(replace));
          } else {
            write(tmp, word, strlen(word));
          }
          wpos = 0;
        }
        write(tmp, &caracter, 1);
      } else {
        if (caracter == search[mpos]) {
          word[wpos++] = caracter;
          mpos++;

          if (mpos == slen) {
            word[wpos - slen] = '\0';
            write(tmp, word, strlen(word));
            write(tmp, replace, strlen(replace));

            wpos = 0;
            mpos = 0;
          }
        } else {
          if (mpos > 0) {
            for (int j = 0; j < mpos; j++) {
              write(tmp, &search[j], 1);
            }
            mpos = 0;
          }
          write(tmp, &caracter, 1);
        }
      }
    }
  }

  if (wpos > 0) {
    word[wpos] = '\0';
    if (strcmp(word, search) == 0)
      write(tmp, replace, strlen(replace));
    else
      write(tmp, word, strlen(word));
  }

  free(word);
  close(fd);
  close(tmp);

  unlink(filename);
  rename("temp.txt", filename);
  return 0;
}
