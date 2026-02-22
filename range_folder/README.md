Here’s a clean, simple **English README** for your `range` project
(ready to drop into `README.md`)

---

# range

**range** is a lightweight C tool that automatically **organizes files into folders** based on their extensions.

It supports **extension groups**, allowing multiple file types to be grouped into the same folder, for example:

- `.mp4 | .mkv | .avi` → `Videos`
- `.mp3 | .wav | .flac` → `Music`
- `.c | .h` → `C_files`

---

## Features

- Scan current directory
- Organize files by extension
- Extension groups using `|`
- ️ Automatic folder creation
- ️ CLI tool
- System-level C (POSIX)
- Fast, lightweight, no dependencies

---

## ️ Build

```bash
gcc range.c -o range
```

---

## ️ Usage

```bash
./range " [path] c|h" [path]/C_files "js|ts" JS_files "mp4|mkv|avi" Videos "mp3|wav|flac" Music
```

---

## Example

Before:

```
main.c
video.mp4
song.mp3
script.js
doc.txt
```

After:

```
C_files/main.c
Videos/video.mp4
Music/song.mp3
JS_files/script.js
doc.txt
```

---

## Concept

```
(extension_group) → folder
```

Example:

```
"mp4|mkv|avi" → Videos
```

---

## Internals

- `opendir / readdir` → directory scanning
- `stat / mkdir` → folder management
- `rename()` → file moving
- CLI parsing (`argv`)
- safe memory handling
- POSIX filesystem API

---

## Current Limitations

- non-recursive
- Linux / Unix only
- no config file
- no dry-run mode
- no logging

---

## Future Improvements

- recursive mode
- Windows support
- config file support
- regex / glob patterns
- undo feature
- logs
- dry-run mode
- interactive mode
- multi-threading
