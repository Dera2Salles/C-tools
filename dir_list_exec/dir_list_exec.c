#include <dirent.h>
#include <stdio.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
  char *dirname = ".";
  if (argc > 1) {
    dirname = argv[1];
  }

  DIR *d = opendir(dirname);
  if (!d) {
    perror("Erreur ouverture dossier");
    return 1;
  }

  struct dirent *entry;

  while ((entry = readdir(d)) != NULL) {
    if (entry->d_name[0] == '.' &&
        (entry->d_name[1] == '\0' ||
         (entry->d_name[1] == '.' && entry->d_name[2] == '\0')))
      continue;
    char *type;
    switch (entry->d_type) {
    case DT_REG:
      type = "fichier";
      break;
    case DT_DIR:
      type = "dossier";
      break;
    case DT_LNK:
      type = "lien";
      break;
    default:
      type = "autre";
      break;
    }
    printf("%s  [%s]\n", entry->d_name, type);
  }
  execl("/bin/ls", "ls", "-l", NULL);
  perror("execl");

  closedir(d);
  return 0;
}
