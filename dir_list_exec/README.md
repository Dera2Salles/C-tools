# Directory Lister (with exec)

A tool that lists directory contents and then executes the standard `ls -l` command.

## Usage
```bash
./dir_list_exec [directory_path]
```
Lists files, folders, and links in the specified directory using `dirent.h`, then calls `execl` to run `/bin/ls -l`.
