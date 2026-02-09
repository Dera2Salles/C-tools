#include <stdio.h>

int main(int argc, const char *argv[]) {
  FILE *file = fopen(argv[1], "a");
  fprintf(file, "%s\n", argv[2]);
  fclose(file);

  return 0;
}
