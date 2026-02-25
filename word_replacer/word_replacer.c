#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define BUF_SIZE 4096

static void write_all(int fd, const char *buf, size_t n) {
  while (n > 0) {
    ssize_t w = write(fd, buf, n);
    if (w <= 0) {
      perror("write");
      exit(1);
    }
    n -= w;
    buf += w;
  }
}

static int *build_failure(const char *s, int len) {
  int *fail = calloc(len, sizeof(int));
  int k = 0;
  for (int i = 1; i < len; i++) {
    while (k > 0 && s[k] != s[i])
      k = fail[k - 1];
    if (s[k] == s[i])
      k++;
    fail[i] = k;
  }
  return fail;
}

int main(int argc, char *argv[]) {
  if (argc < 4) {
    fprintf(stderr,
            "Usage: %s <fichier> <mot_a_chercher> <mot_de_remplacement>\n"
            "Example:\n  %s file.txt dera ZAZA\n",
            argv[0], argv[0]);
    return 1;
  }

  char *filename = argv[1];
  char *search = argv[2];
  char *replace = argv[3];
  int slen = strlen(search);
  int rlen = strlen(replace);
  int *fail = build_failure(search, slen);

  int fd = open(filename, O_RDONLY);
  int tmp = open("temp.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);
  if (fd == -1 || tmp == -1) {
    perror("open");
    free(fail);
    return 1;
  }

  int mpos = 0;
  char buffer[BUF_SIZE];
  ssize_t byte_read;

  while ((byte_read = read(fd, buffer, BUF_SIZE)) > 0) {
    for (int i = 0; i < byte_read; i++) {
      char c = buffer[i];

      while (mpos > 0 && c != search[mpos])
        mpos = fail[mpos - 1];

      if (c == search[mpos])
        mpos++;

      if (mpos == slen) {
        write_all(tmp, replace, rlen);
        mpos = fail[slen - 1];
      } else if (mpos == 0) {
        write_all(tmp, &c, 1);
      }
    }
  }

  // Flush les caractères en attente (match incomplet en fin de fichier)
  if (mpos > 0)
    write_all(tmp, search, mpos);

  free(fail);
  close(fd);
  close(tmp);
  unlink(filename);
  rename("temp.txt", filename);
  return 0;
}
