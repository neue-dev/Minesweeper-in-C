#include <stdlib.h>
#include <string.h>
#include <stdio.h>

int main(int argc, char *argv[]) {

  // The filename of the main file and the compilation command
  char filename[32];
  char command[128];

  // In case we're developing or smth
  strcpy(filename, argc > 1 ? (!strcmp(argv[1], "dev") ? "minesweeper.dev" : "minesweeper") : "minesweeper");

  // Compile and run the game
  #ifdef _WIN32
    sprintf(command, "gcc -std=c99 -Wall src\\%s.c -o build\\minesweeper.win.exe", filename);
    system(command);
    system("build\\minesweeper.win.exe");
  #else
    sprintf(command, "gcc -std=c99 -Wall ./src/%s.c -o ./build/minesweeper.unix.o", filename);
    system(command);
    system("./build/minesweeper.unix.o");
  #endif

  return 0;
}