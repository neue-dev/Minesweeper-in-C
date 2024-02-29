/**
 * @ Author: MMMM
 * @ Create Time: 2024-01-29 18:08:09
 * @ Modified time: 2024-02-29 23:19:38
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
    // I don't think itll work
    if(argc > 1 ? !strcmp(argv[1], "force") : 0) {
      system("C: && C:\\Windows\\SysNative\\conhost.exe"); //build\\minesweeper.win.exe");
      system("C: && C:\\Windows\\System32\\conhost.exe");
    } else if (argc > 1 ? !strcmp(argv[1], "run") : 0) {

      // Just so the user doesn't get irritated
      system("cls");
      printf("\n\n\tThe game is loading...");
      system("build\\minesweeper.win.exe");

      // After playing
      system("cls");
      printf("\n\n\tThank you for playing!\n\n\n");
    } else {
      
      // Warning, in case conhost doesn't execute
      system("cls");
      printf("\n\n\tThe game has been compiled! If you are currently on Windows 11:\n\n");
      printf("\t\t(1) Launch the Windows Terminal.\n");
      printf("\t\t(2) Type \"conhost\".\n");
      printf("\t\t(3) Type \"main run\" after launching conhost.\n\n");
      printf("\tNote that we need conhost because the program kinda breaks on Windows 11.\n\n");
    }
  #else
    sprintf(command, "gcc -Wall ./src/%s.c -o ./build/minesweeper.unix.o 2> ./build/.log.unix.txt -lrt -lm", filename);
    system(command);

    // Just so the user doesn't get irritated
    printf("\e[H\e[2J\e[3J");
    printf("\n\n\tThe game is loading...\n");
    system("./build/minesweeper.unix.o");

    // After the game exits
    printf("\e[H\e[2J\e[3J");
    printf("\n\n\tThank you for playing!\n\n\n");
  #endif

  return 0;
}