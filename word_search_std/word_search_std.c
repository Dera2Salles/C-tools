#include <stdio.h>
#include <string.h>

int main(int argc, const char *argv[]) {

  if (argc < 3) {
    printf("Programme mandeha : %s\n", argv[0]);
    return 1;
  }

  FILE *file = fopen(argv[1], "r");
  if (!file) {
    perror("Erreur ouverture fichier");
    return 1;
  }

  char buffer[256];
  int found = 0;
  int line = 1;

  while (fgets(buffer, sizeof(buffer), file)) {

    buffer[strcspn(buffer, "\n")] = '\0';
    printf("%s - ligne %d \n", buffer, line);
    if (strcmp(buffer, argv[2]) == 0) {
      printf("Mot trouvé : %s\n", buffer);
      found = 1;
      break;
    }
    line++;
  }

  if (!found) {
    printf("Mot non trouvé\n");
  }

  fclose(file);
  return 0;
}
