#include <dirent.h>
#include <fcntl.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

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

static void make_tmp_path(const char *filename, char *out, size_t out_size) {
  const char *slash = strrchr(filename, '/');
  if (slash) {
    size_t dirlen = slash - filename + 1;
    snprintf(out, out_size, "%.*s.srep_tmp", (int)dirlen, filename);
  } else {
    snprintf(out, out_size, ".srep_tmp");
  }
}

static int *build_failure(const char *s, int len) {
  int *fail = calloc(len, sizeof(int));
  if (!fail) {
    perror("calloc");
    exit(1);
  }
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

static void replace_in_file(const char *filename, const char *search,
                            const char *replace, const int *fail, int slen,
                            int rlen) {
  char tmp_path[PATH_MAX];
  make_tmp_path(filename, tmp_path, sizeof(tmp_path));

  int fd = open(filename, O_RDONLY);
  int tmp = open(tmp_path, O_WRONLY | O_CREAT | O_TRUNC, 0644);
  if (fd == -1 || tmp == -1) {
    perror("open");
    if (fd != -1)
      close(fd);
    if (tmp != -1)
      close(tmp);
    return;
  }

  int mpos = 0;
  char buffer[4096];
  ssize_t byte_read;

  while ((byte_read = read(fd, buffer, sizeof(buffer))) > 0) {
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

  if (mpos > 0)
    write_all(tmp, search, mpos);

  close(fd);
  close(tmp);
  unlink(filename);
  rename(tmp_path, filename);
}

static char **findFile(DIR *directory, const char *dirPath, int *outSize,
                       int recursive) {
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

    if (strcmp(entry->d_name, ".srep_tmp") == 0)
      continue;

    char fullPath[PATH_MAX];
    snprintf(fullPath, sizeof(fullPath), "%s/%s", dirPath, entry->d_name);

    int d_type = entry->d_type;
    if (d_type == DT_UNKNOWN) {
      struct stat st;
      if (stat(fullPath, &st) == 0) {
        if (S_ISREG(st.st_mode))
          d_type = DT_REG;
        else if (S_ISDIR(st.st_mode))
          d_type = DT_DIR;
      }
    }

    if (d_type == DT_REG) {
      if (size == capacity) {
        capacity *= 2;
        char **tmp = realloc(fileList, capacity * sizeof(char *));
        if (!tmp) {
          perror("realloc");
          break;
        }
        fileList = tmp;
      }
      fileList[size] = strdup(fullPath);
      if (!fileList[size]) {
        perror("strdup");
        break;
      }
      size++;

    } else if (recursive && d_type == DT_DIR) {
      DIR *subDir = opendir(fullPath);
      if (!subDir) {
        perror("opendir");
        continue;
      }

      int subSize = 0;
      char **subList = findFile(subDir, fullPath, &subSize, recursive);
      closedir(subDir);

      if (!subList)
        continue;

      if (size + subSize >= capacity) {
        while (size + subSize >= capacity)
          capacity *= 2;
        char **tmp = realloc(fileList, capacity * sizeof(char *));
        if (!tmp) {
          perror("realloc");
          for (int i = 0; i < subSize; i++)
            free(subList[i]);
          free(subList);
          break;
        }
        fileList = tmp;
      }
      for (int i = 0; i < subSize; i++)
        fileList[size++] = subList[i];
      free(subList);
    }
  }

  *outSize = size;
  return fileList;
}

int main(int argc, char *argv[]) {
  if (argc < 4) {
    fprintf(stderr,
            "Usage: %s [-r] <fichier_ou_dossier> <search> <replace>\n"
            "Examples:\n"
            "  %s file.txt dera ZAZA\n"
            "  %s -r ./docs dera ZAZA\n",
            argv[0], argv[0], argv[0]);
    return 1;
  }

  int recursive = 0;
  int argOffset = 0;

  if (strcmp(argv[1], "-r") == 0) {
    if (argc < 5) {
      fprintf(stderr, "Usage: %s -r <fichier_ou_dossier> <search> <replace>\n",
              argv[0]);
      return 1;
    }
    recursive = 1;
    argOffset = 1;
  }

  char *target = argv[1 + argOffset];
  char *search = argv[2 + argOffset];
  char *replace = argv[3 + argOffset];
  int slen = strlen(search);
  int rlen = strlen(replace);

  if (slen == 0) {
    fprintf(stderr, "Erreur : le mot a chercher ne peut pas etre vide.\n");
    return 1;
  }

  int *fail = build_failure(search, slen);

  DIR *dir = opendir(target);
  if (dir) {
    int size = 0;
    char **files = findFile(dir, target, &size, recursive);
    closedir(dir);

    if (files) {
      for (int i = 0; i < size; i++) {
        printf("Processing: %s\n", files[i]);
        replace_in_file(files[i], search, replace, fail, slen, rlen);
        free(files[i]);
      }
      free(files);
    }
  } else {
    replace_in_file(target, search, replace, fail, slen, rlen);
  }

  free(fail);
  return 0;
}
