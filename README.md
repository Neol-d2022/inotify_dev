# inotify-dev

## A repo that is set for learning inotify API

### Update 2017/10/01
- Tested in Ubuntu VM.
  - There are no required packages needed to be installed first in order to compile.
  - `gcc main.c` will produce an executable file. Default file name is `a.out`
  - Arguments for the executable will be the file(s) to be monitored.
    - Example: `a.out main.c`
      - The program will monitor the file `main.c`.
      - Try to open the file / edit it / rename it or remove it, and see what the program outputs.

### Note:
- Source code files are edited using VS Code.
  - `"files.insertFinalNewline": true` is set, meaning there are EOLs.
  - `C/C++` Module by Microsoft is installed. The `format document` function in it is used before saving in this repo.
    - `CTRL + SHIFT + I` for Ubuntu
    - `ALT + SHIFT + F` for Windows
