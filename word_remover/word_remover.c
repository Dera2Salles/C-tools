#include <dirent.h>
#include <fcntl.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#define BUF_SIZE 4096


static int isSeparator(char c) {
  return (c == ' ' || c == '\n' || c == '\t' || c == '.' || c == ',');
}

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
    snprintf(out, out_size, "%.*s.swrem_tmp", (int)dirlen, filename);
  } else {
    snprintf(out, out_size, ".swrem_tmp");
  }
}


static int process_file(const char *filepath, const char *word_to_remove) {
  char tmp_path[PATH_MAX];
  make_tmp_path(filepath, tmp_path, sizeof(tmp_path));

  int fd = open(filepath, O_RDONLY);
  int tmp = open(tmp_path, O_WRONLY | O_CREAT | O_TRUNC, 0644);
  if (fd == -1 || tmp == -1) {
    perror("open");
    if (fd != -1)
      close(fd);
    if (tmp != -1)
      close(tmp);
    return -1;
  }

  int wlen = strlen(word_to_remove);
  char buffer[BUF_SIZE];
  int word_cap = wlen + 64;
  char *word = malloc(word_cap);
  if (!word) {
    perror("malloc");
    close(fd);
    close(tmp);
    return -1;
  }
  int wpos = 0;
  int count = 0;
  ssize_t bytes;

  while ((bytes = read(fd, buffer, BUF_SIZE)) > 0) {
    for (int i = 0; i < bytes; i++) {
      char c = buffer[i];
      if (isSeparator(c)) {
        if (wpos > 0) {
          word[wpos] = '\0';
          if (strcmp(word, word_to_remove) != 0)
            write_all(tmp, word, wpos);
          else
            count++;
          wpos = 0;
        }
        write_all(tmp, &c, 1);
      } else {
        if (wpos + 1 >= word_cap) {
          word_cap *= 2;
          char *tmp2 = realloc(word, word_cap);
          if (!tmp2) {
            perror("realloc");
            break;
          }
          word = tmp2;
        }
        word[wpos++] = c;
      }
    }
  }

  if (wpos > 0) {
    word[wpos] = '\0';
    if (strcmp(word, word_to_remove) != 0)
      write_all(tmp, word, wpos);
    else
      count++;
  }

  free(word);
  close(fd);
  close(tmp);
  unlink(filepath);
  rename(tmp_path, filepath);
  return count;
}


static void walk(const char *dirPath, const char *word, int recursive) {
  DIR *directory = opendir(dirPath);
  if (!directory) {
    perror("opendir");
    return;
  }

  struct dirent *entry;
  while ((entry = readdir(directory)) != NULL) {
    if (entry->d_name[0] == '.' &&
        (entry->d_name[1] == '\0' ||
         (entry->d_name[1] == '.' && entry->d_name[2] == '\0')))
      continue;

    if (strcmp(entry->d_name, ".swrem_tmp") == 0)
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
      int count = process_file(fullPath, word);
      if (count >= 0)
        printf("%s : %d occurrence(s) supprimee(s)\n", fullPath, count);
    } else if (recursive && d_type == DT_DIR) {
      walk(fullPath, word, recursive);
    }
  }

  closedir(directory);
}


int main(int argc, char *argv[]) {
  if (argc < 3) {
    fprintf(stderr,
            "Usage: %s [-r] <fichier|dossier> <mot_a_supprimer>\n"
            "Exemples:\n"
            "  %s file.txt dera\n"
            "  %s ./data  dera\n"
            "  %s -r ./data dera\n",
            argv[0], argv[0], argv[0], argv[0]);
    return 1;
  }

  int recursive = 0;
  int argOffset = 0;

  if (strcmp(argv[1], "-r") == 0) {
    if (argc < 4) {
      fprintf(stderr, "Usage: %s -r <fichier|dossier> <mot_a_supprimer>\n",
              argv[0]);
      return 1;
    }
    recursive = 1;
    argOffset = 1;
  }

  const char *target = argv[1 + argOffset];
  const char *word = argv[2 + argOffset];

  if (strlen(word) == 0) {
    fprintf(stderr, "Erreur : le mot a supprimer ne peut pas etre vide.\n");
    return 1;
  }

  struct stat st;
  if (stat(target, &st) == -1) {
    perror("stat");
    return 1;
  }

  if (S_ISREG(st.st_mode)) {
    int count = process_file(target, word);
    if (count >= 0)
      printf("%s : %d occurrence(s) supprimee(s)\n", target, count);
  } else if (S_ISDIR(st.st_mode)) {
    walk(target, word, recursive);
  } else {
    fprintf(stderr, "Erreur : '%s' n'est ni un fichier ni un dossier.\n",
            target);
    return 1;
  }

  return 0;
}
