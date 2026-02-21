#include <stdio.h>
#include <string.h>

int main(int argc, char *argv[]) {
  if (argc < 2) {
    printf("Usage: %s <nom_fichier>\n", argv[0]);
    return 1;
  }

  if (strcmp(argv[1], "-r") == 0) {
    FILE *file;
    file = fopen(argv[2], "r");
    if (!file) {
      perror("Erreur ouverture fichier");
      return 1;
    }

    int c;
    while ((c = fgetc(file)) != EOF) {
      putchar(c);
    }
    fclose(file);
  }

  return 0;
}
