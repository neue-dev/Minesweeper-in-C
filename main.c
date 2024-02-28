/**
 * @ Author: MMMM
 * @ Create Time: 2024-01-29 18:08:09
 * @ Modified time: 2024-02-28 22:21:55
 * 
 *               _                                                   
 *    ____ ___  (_)___  ___  ______      _____  ___  ____  ___  _____
 *   / __ `__ \/ / __ \/ _ \/ ___/ | /| / / _ \/ _ \/ __ \/ _ \/ ___/
 *  / / / / / / / / / /  __(__  )| |/ |/ /  __/  __/ /_/ /  __/ /    
 * /_/ /_/ /_/_/_/ /_/\___/____/ |__/|__/\___/\___/ .___/\___/_/     
 *                                               /_/                 
 * 
 *                                                      [ MMMM @2024 ]
 */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

int main(int argc, char *argv[]) {

  // The filename of the main file and the compilation command
  char filename[32];
  char command[128];

  // In case we're developing or smth
  strcpy(filename, argc > 2 ? (!strcmp(argv[2], "dev") ? "minesweeper.dev" : "minesweeper") : "minesweeper");

  // Compile and run the game
  #ifdef _WIN32
    sprintf(command, "gcc -std=c99 -Wall src\\%s.c -o build\\minesweeper.win.exe 2> build\\.log.win.txt", filename);
    system(command);

    // Execute using conhost or just execute with the system default shell
    if(argc > 1 ? !strcmp(argv[1], "force") : 0) {
      system("%windiw%\\SysNative\\conhost.exe build\\minesweeper.win.exe");
      system("%windiw%\\System32\\conhost.exe build\\minesweeper.win.exe");
    } else {
      system("build\\minesweeper.win.exe");
    }

    // Warning, in case conhost doesn't execute
    system("cls");
    printf("\n\nIf you do not see the program running, do either of the following:\n\n");
    printf("    (1) Run the program in the C:\\ drive.\n");
    printf("    (2) Run the program inside conhost (ie. type \"conhost\" first then run main.exe)\n\n");
    printf("If you select the second option, the program display might not fit\nwithin the console window.\n\n");
  #else
    sprintf(command, "gcc -Wall ./src/%s.c -o ./build/minesweeper.unix.o 2> ./build/.log.unix.txt -lrt -lm", filename);
    system(command);
    system("./build/minesweeper.unix.o");
  #endif

  return 0;
}