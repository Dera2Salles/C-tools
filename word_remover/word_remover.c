#include <dirent.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#define BUF_SIZE 64
#define WORD_SIZE 64

int isSeparator(char c) {
  return (c == ' ' || c == '\n' || c == '\t' || c == '.' || c == ',');
}

char **findFile(DIR *directory, int *outSize) {
  struct dirent *entry;
  int size = 0, capacity = 8;

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

int process_file(const char *filepath, const char *word_to_remove) {
  int fd = open(filepath, O_RDONLY);
  if (fd == -1) {
    perror("open input");
    return -1;
  }

  int tmp = open(".__temp__.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);
  if (tmp == -1) {
    perror("open temp");
    close(fd);
    return -1;
  }

  char buffer[BUF_SIZE];
  char word[WORD_SIZE];
  int wpos = 0;
  ssize_t bytes;
  int count = 0;

  while ((bytes = read(fd, buffer, BUF_SIZE)) > 0) {
    for (int i = 0; i < bytes; i++) {
      char c = buffer[i];
      if (isSeparator(c)) {
        if (wpos > 0) {
          word[wpos] = '\0';
          if (strcmp(word, word_to_remove) != 0) {
            write(tmp, word, strlen(word));
          } else {
            count++;
          }
          wpos = 0;
        }
        write(tmp, &c, 1);
      } else {
        if (wpos < WORD_SIZE - 1)
          word[wpos++] = c;
      }
    }
  }

  if (wpos > 0) {
    word[wpos] = '\0';
    if (strcmp(word, word_to_remove) != 0) {
      write(tmp, word, strlen(word));
    } else {
      count++;
    }
  }

  close(fd);
  close(tmp);
  unlink(filepath);
  rename(".__temp__.txt", filepath);

  return count;
}

int main(int argc, char *argv[]) {

  if (argc < 3) {
    fprintf(stderr,
            "Usage: %s <fichier|dossier> <mot_a_supprimer>\n"
            "Exemples:\n"
            "  %s file.txt dera       (un fichier)\n"
            "  %s ./data dera         (dossier)\n",
            argv[0], argv[0], argv[0]);
    return 1;
  }

  const char *target = argv[1];
  const char *word = argv[2];

  struct stat st;
  if (stat(target, &st) == -1) {
    perror("stat");
    return 1;
  }

  if (S_ISREG(st.st_mode)) {
    int count = process_file(target, word);
    printf("%s : %d occurrences supprimées\n", target, count);
  } else if (S_ISDIR(st.st_mode)) {
    DIR *directory = opendir(target);
    if (!directory) {
      perror("opendir");
      return 1;
    }

    int fileCount = 0;
    char **fileList = findFile(directory, &fileCount);
    closedir(directory);

    if (!fileList)
      return 1;

    printf("Fichiers trouvés: %d\n", fileCount);

    for (int i = 0; i < fileCount; i++) {
      char fullpath[512];
      snprintf(fullpath, sizeof(fullpath), "%s/%s", target, fileList[i]);

      int count = process_file(fullpath, word);
      printf("%s : %d occurrences supprimées\n", fullpath, count);

      free(fileList[i]);
    }
    free(fileList);
  } else {
    fprintf(stderr, "%s n'est pas un fichier ni un dossier\n", target);
    return 1;
  }

  return 0;
}
