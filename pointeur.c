#include <stdio.h>

int main(int argc, const char *argv[]) {

  void *pointeur_null;
  int x = 2;
  pointeur_null = &x;
  printf("%p", pointeur_null);
  return 0;
}
