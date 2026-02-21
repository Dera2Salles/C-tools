#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

char **getText() {

  char **tabChar = malloc(3 * sizeof(char *));
  tabChar[0] = strdup("Dera");
  tabChar[1] = strdup("Test");
  tabChar[2] = strdup("Encore");

  return tabChar;
}

int main() {

  char **tabChar = realloc(getText(), 4 * sizeof(char *));
  for (int i = 0; i <= 2; i++) {
    printf("%s\n", tabChar[i]);
  }

  tabChar[4] = strdup("Dera.txt");
  printf("%s", *tabChar);

  for (int i = 0; i <= 4; i++) {
    free(tabChar[i]);
    tabChar[i] = NULL;
  }

  free(tabChar);
  tabChar = NULL;

  return 0;
}
