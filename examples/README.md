# hbgl-c

## Installing on Windows

- MSYS2

Before installing the packages, make sure you're running the MinGW 64-bit shell.

   ```
   pacman -S mingw-w64-x86_64-glfw
   pacman -S mingw-w64-x86_64-glew
   ```

- To build the static library, execute:

   ```
   make
   ```

- To build and run samples and test code, navigate to the examples directory and execute:

   ```
   cd examples
   gcc main.c -o main -I ../include -L ../lib/win/mingw64  -lhbgl -lopengl32 -lglfw3 -lm
   ```

## Installing on Linux

- Debian

   ```
   sudo apt-get update
   sudo apt-get install libgl-dev libglfw3-dev
   ```
- To build the static library, execute:

   ```
   make
   ```

- To build and run samples and test code, navigate to the examples directory and execute:

   ```
   cd examples
   gcc main.c -o main -I ../include -L ../lib/linux/gcc -lhbgl -lGL -lglfw -lm
   ```
---
