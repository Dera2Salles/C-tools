#include <stdio.h>

int main(int argc, char *argv[]) {

  if (argc < 2) {
    return 0;
  }

  FILE *file = fopen(argv[1], "r");
  FILE *dest = fopen(argv[2], "w");
  int c;

  while ((c = getc(file)) != EOF) {
    putc(c, dest);
  }
  fclose(file);
  fclose(dest);
  return 0;
}
