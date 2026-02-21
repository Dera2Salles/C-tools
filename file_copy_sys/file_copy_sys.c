#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

int main(int argc, char *argv[]) {

  if (argc < 3) {
    perror("copy_low_level\nmissing arguments");
    return 1;
  }

  int file_desc = open(argv[1], O_RDONLY);
  int out_desc = open(argv[2], O_WRONLY | O_CREAT, 0644);

  if (file_desc == -1 || out_desc == -1) {
    perror("Error : ");
    return 1;
  }

  char buffer[256];
  ssize_t caracter;

  while ((caracter = read(file_desc, buffer, sizeof(buffer))) > 0) {
    write(out_desc, buffer, caracter);
  }

  close(file_desc);
  close(out_desc);
  return 0;
}
