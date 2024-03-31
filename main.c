/** 
 * 
 *    ▀████▄     ▄███▀▀████▀▀███▄   ▀███▀▀███▀▀▀███ ███▀▀▀███         █      
 *      ████    ████    ██    ███▄    █    ██    ▀█ █▀   ███      ▀▄█████▄▀  
 *      █ ██   ▄█ ██    ██    █ ███   █    ██   █   ▀   ███       ██  █████  
 *      █  ██  █▀ ██    ██    █  ▀██▄ █    ██████      ███      ▀▀█████████▀▀
 *      █  ██▄█▀  ██    ██    █   ▀██▄█    ██   █  ▄  ███   ▄     ▀███████▀  
 *      █  ▀██▀   ██    ██    █     ███    ██     ▄█ ███   ▄█     ▀ ▀▀█▀▀ ▀  
 *    ▄███▄ ▀▀  ▄████▄▄████▄▄███▄    ██  ▄███████████████████         ▀      
 * 
 * 
 * Description:       Minesweeper... in C
 * Author:            Malks Mogen M. David, Mariella Jeanne A. Dellosa
 * Section:           S17B
 * Last Modified:     2024-04-01
 * Acknowledgments:
 * 
 * To keep things succint, I present my utmost gratitude to the following martyrs who have guided the development of this program
 *  
 * [_beginthreadex vs CreateThread]          https://stackoverflow.com/questions/331536/windows-threading-beginthread-vs-beginthreadex-vs-createthread-c
 * [ANSI Escape Sequences]                   https://gist.github.com/fnky/458719343aabd01cfb17a3a4f7296797
 * [C runtime library]                       https://stackoverflow.com/questions/2766233/what-is-the-c-runtime-library
 * [Checking for Windows 11]                 https://stackoverflow.com/questions/74645458/how-to-detect-windows-11-programmatically
 * [Differences of unicode and utf8]         https://stackoverflow.com/questions/50067263/how-to-printf-a-unicode-string-with-s-specifier
 * [Enable Virtual Terminal Processing]      https://stackoverflow.com/questions/52015837/trouble-enable-virtual-terminal-processing-for-windows-in-c
 * [FuncA vs FuncW in Windows libs]          https://comp.os.ms-windows.programmer.narkive.com/4ADwdQBR/regarding-createmutexw-and-createmutexa
 * [Function Pointers]                       https://stackoverflow.com/questions/840501/how-do-function-pointers-in-c-work
 * [Interesting Hash Functions]              http://www.cse.yorku.ca/~oz/hash.html
 * [Invisible cursor in Linux]               https://www.reddit.com/r/linuxquestions/comments/rl1jai/how_to_hide_terminal_cursorcaret_when_not_in/
 * [Make Windows Use ANSI Esc Sequences]     https://stackoverflow.com/questions/16755142/how-to-make-win32-console-recognize-ansi-vt100-escape-sequences-in-c
 * [Multithreading in C Linux]               https://linux.die.net/man/2/clone
 * [Multithreading in C Linux Demo]          https://www.evanjones.ca/software/threading.html
 * [Multithreading in C Windows Demo]        https://learn.microsoft.com/en-us/cpp/parallel/sample-multithread-c-program
 * [Multithreading in Linux without Posix]   https://stackoverflow.com/questions/13283294/how-to-make-thread-in-c-without-using-posix-library-pthread-h
 * [Neat text generator]                     https://patorjk.com/software/taag/#p=display&f=ANSI%20Shadow  
 * [Passing Arguments to _beginthread]       https://stackoverflow.com/questions/20412633/calling-beginthreadx-with-passing-function-pointers
 * [Printing Unicode in C]                   https://stackoverflow.com/questions/43834315/printing-a-unicode-symbol-in-c
 * [puts() is Apparerntly So Fast]           https://cboard.cprogramming.com/c-programming/179502-best-way-print-large-amount-text.html
 * [PVOID vs. LPVOID]                        https://forums.codeguru.com/showthread.php?490459-what-is-difference-between-PVOID-and-LPVOID
 * [Resizing gnome terminal]                 https://askubuntu.com/questions/1037463/how-to-resize-terminal-window-permanently-using-cli
 * [Scientific notation in C]                https://stackoverflow.com/questions/16905988/how-to-represent-scientific-notation-in-c
 * [Some stuff about file handling]          https://stackoverflow.com/questions/8175827/what-happens-if-i-dont-call-fclose-in-a-c-program
 * [Timeouts in C Unix]                      https://stackoverflow.com/questions/46365448/pthread-mutex-timedlock-exiting-prematurely-without-waiting-for-timeout
 * [Using swprintf_s]                        https://www.educative.io/answers/what-is-swprintfs-in-c
 * [Using setvbuf() Can Boost Efficiency]    https://stackoverflow.com/questions/1832489/printf-slows-down-my-program
 * [Variadic Functions in C]                 https://stackoverflow.com/questions/205529/passing-variable-number-of-arguments-around
 * [Valgrind!!!!!]                           https://stackoverflow.com/questions/5134891/how-do-i-use-valgrind-to-find-memory-leaks
 * [Windows Data Types]                      https://learn.microsoft.com/en-us/windows/win32/winprog/windows-data-types
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

/**
 * This is to certify that this project is my own work , based on my personal
 * efforts in studying and applying the concepts learned . I have constructed
 * the functions and their respective algorithms and corresponding code by
 * myself . The program was run , tested , and debugged by my own efforts .
 * I further certify that I have not copied in part or whole or otherwise
 * plagiarized the work of other students and / or persons .
 * 
 * Malks Mogen M. David, DLSU ID #12306991
 * Mariella Jeanne A. Dellosa, DLSU ID #12323434 
*/