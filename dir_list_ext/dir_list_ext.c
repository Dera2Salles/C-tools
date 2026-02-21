#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

int ensure_dir(const char *path) {
  struct stat st;

  if (stat(path, &st) == 0) {
    if (S_ISDIR(st.st_mode)) {
      return 1;
    } else {
      fprintf(stderr, "%s existe mais n'est pas un dossier\n", path);
      return -1;
    }
  }

  if (mkdir(path, 0755) == 0) {
    return 1;
  }

  perror("mkdir");
  return -1;
}

const char *get_extension(const char *filename) {
  const char *dot = strrchr(filename, '.');
  if (!dot || dot == filename)
    return NULL;
  return dot + 1;
}

char **findFile(DIR *directory, int *outSize) {

  struct dirent *entry;

  int size = 0;
  int capacity = 8;

  char **fileList = malloc(capacity * sizeof(char *));
  if (!fileList) {
    perror("malloc");
    return NULL;
  }

  while ((entry = readdir(directory)) != NULL) {

    if (entry->d_name[0] == '.' &&
        (entry->d_name[1] == '\0' ||
         (entry->d_name[1] == '.' && entry->d_name[2] == '\0')))
      continue;

    if (entry->d_type == DT_REG) {

      if (size == capacity) {
        capacity *= 2;

        char **tmp = realloc(fileList, capacity * sizeof(char *));
        if (!tmp) {
          perror("realloc");
          break;
        }
        fileList = tmp;
      }

      fileList[size] = strdup(entry->d_name);
      if (!fileList[size]) {
        perror("strdup");
        break;
      }

      size++;
    }
  }

  *outSize = size;
  return fileList;
}

int main(int argc, char *argv[]) {

  char *dirname = ".";
  if (argc > 1)
    dirname = argv[1];

  DIR *directory = opendir(dirname);
  if (!directory) {
    perror("Erreur ouverture dossier");
    return 1;
  }

  int count = 0;
  char **fileList = findFile(directory, &count);

  if (!fileList) {
    closedir(directory);
    return 1;
  }

  printf("Fichiers trouvés : %d\n\n", count);

  for (int i = 0; i < count; i++) {
    const char *ext = get_extension(fileList[i]);
    printf("[%d] %s  ->  %s\n", i, fileList[i], ext ? ext : "no extension");
  }

  for (int i = 0; i < count; i++) {
    free(fileList[i]);
  }
  free(fileList);

  closedir(directory);
  return 0;
}
