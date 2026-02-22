#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

int isDirectory(const char *path) {
  struct stat st;
  if (stat(path, &st) == 0) {
    if (S_ISDIR(st.st_mode))
      return 1;
    return -1;
  }
  return mkdir(path, 0755);
}

const char *get_extension(const char *filename) {
  const char *dot = strrchr(filename, '.');
  if (!dot || dot == filename)
    return NULL;
  return dot + 1;
}

int isSameCategory(const char *ext, const char *group) {
  char buf[256];
  strncpy(buf, group, sizeof(buf));
  buf[sizeof(buf) - 1] = 0;

  char *token = strtok(buf, "|");
  while (token) {
    if (strcmp(ext, token) == 0)
      return 1;
    token = strtok(NULL, "|");
  }
  return 0;
}

int move_file(const char *base_path, const char *src, const char *dst_dir) {
  char src_path[512];
  char dst_path[512];

  snprintf(src_path, sizeof(src_path), "%s/%s", base_path, src);
  snprintf(dst_path, sizeof(dst_path), "%s/%s", dst_dir, src);

  return rename(src_path, dst_path);
}

int main(int argc, char *argv[]) {

  const char *base_path;
  int arg_start;

  if (argc >= 3 && argv[1][0] != '.') {
    base_path = argv[1];
    arg_start = 2;
  } else {
    base_path = ".";
    arg_start = 1;
  }

  if ((argc - arg_start) % 2 != 0 || argc - arg_start < 2) {
    printf("Usage:\n");
    printf("  %s [path] \"pdf|doc\" Docs \"js|ts\" JS_files\n", argv[0]);
    printf("Example:\n");
    printf("  %s Document \"pdf|doc\" Docs \"mp4|mkv|avi\" Videos\n", argv[0]);
    printf("  %s \"c|h\" C_files \"js|ts\" JS_files   (current dir)\n",
           argv[0]);
    return 1;
  }

  for (int i = arg_start; i < argc; i += 2) {
    if (isDirectory(argv[i + 1]) < 0) {
      printf("Error creating directory: %s\n", argv[i + 1]);
      return 1;
    }
  }

  DIR *d = opendir(base_path);
  if (!d) {
    perror("opendir");
    return 1;
  }

  struct dirent *entry;

  while ((entry = readdir(d)) != NULL) {

    if (entry->d_type != DT_REG)
      continue;

    const char *ext = get_extension(entry->d_name);
    if (!ext)
      continue;

    for (int i = arg_start; i < argc; i += 2) {
      if (isSameCategory(ext, argv[i])) {
        move_file(base_path, entry->d_name, argv[i + 1]);
        break;
      }
    }
  }

  closedir(d);
  return 0;
}
