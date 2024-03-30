/**
 * @ Author: MMMM
 * @ Create Time: 2024-01-29 18:08:09
 * @ Modified time: 2024-03-31 03:50:13
 * 
 *    ▀████▄     ▄███▀▀████▀▀███▄   ▀███▀▀███▀▀▀███ ███▀▀▀███         █      
 *      ████    ████    ██    ███▄    █    ██    ▀█ █▀   ███      ▀▄█████▄▀  
 *      █ ██   ▄█ ██    ██    █ ███   █    ██   █   ▀   ███       ██  █████  
 *      █  ██  █▀ ██    ██    █  ▀██▄ █    ██████      ███      ▀▀█████████▀▀
 *      █  ██▄█▀  ██    ██    █   ▀██▄█    ██   █  ▄  ███   ▄     ▀███████▀  
 *      █  ▀██▀   ██    ██    █     ███    ██     ▄█ ███   ▄█     ▀ ▀▀█▀▀ ▀  
 *    ▄███▄ ▀▀  ▄████▄▄████▄▄███▄    ██  ▄███████████████████         ▀      
 * 
 *                                            [ MMMM @2024 ]
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
    sprintf(command, "gcc -std=c99 -Wall src\\%s.c -o build\\minesweeper.win.exe 2> build\\.debug\\log.win.txt", filename);
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
      
      // Reset colors in case of crash
      printf("\x1b[38;5;255m");
      printf("\x1b[48;5;232m");

      // After playing
      system("cls");
      printf("\n\n\tThank you for playing!\n\n\n");
    } else {
      
      // Warning, in case conhost doesn't execute
      system("cls");
      printf("\x1b[0;0m");
      printf("\n\n\tThe game has been compiled! If you are on \x1b[1;34mWindows 10\x1b[0;0m, just do:\n\n");
      printf("\t\t(1) \x1b[38;5;111m\"main run\"\x1b[0;0m\n\n");
      printf("\n\n\tOtherwise, if you are currently on \x1b[1;34mWindows 11\x1b[0;0m:\n\n");
      printf("\t\t(1) Launch the \x1b[38;5;111mWindows Terminal.\x1b[0;0m\n");
      printf("\t\t(2) Type \x1b[38;5;111m\"conhost\"\x1b[0;0m.\n");
      printf("\t\t(3) Type \x1b[38;5;111m\"main run\"\x1b[0;0m after launching conhost.\n\n");
      printf("\t\tNote that we need conhost because the program kinda breaks on Windows 11.\n\n");
    }
  #else
    sprintf(command, "gcc -Wall -ggdb3 ./src/%s.c -o ./build/minesweeper.unix.o 2> ./build/.debug/log.unix.txt -lrt -lm", filename);
    system(command);

    // Just so the user doesn't get irritated
    system("tput civis");
    printf("\e[H\e[2J\e[3J");
    printf("\n\n\tThe game is loading...\n");
    
    // We have no choice but to launch a new window if we wanna resize
    // Also, this doesn't work on KDE so GG (idk how to do smth similar there)
    system("gnome-terminal --geometry=132x36 -- ./build/minesweeper.unix.o");
    system("./build/minesweeper.unix.o");

    // Reset colors and cursor in case of crash
    printf("\x1b[38;5;255m");
    printf("\x1b[48;5;232m");
    system("tput cnorm");

    // After the game exits
    printf("\e[H\e[2J\e[3J");
    printf("\n\n\tThank you for playing!\n\n\n");
  #endif

  return 0;
}