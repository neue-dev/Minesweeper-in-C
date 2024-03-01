/**
 * @ Author: MMMM
 * @ Create Time: 2024-02-17 20:09:01
 * @ Modified time: 2024-03-01 09:10:41
 * @ Description:
 * 
 * Low level handling of IO functionalities on Windows.
 */

#ifndef UTILS_IO_WIN_
#define UTILS_IO_WIN_

#include <conio.h>
#include <windows.h>
#define ENABLE_VIRTUAL_TERMINAL_PROCESSING 0x0004   // fOr sOmE ReASon Its nOt dEfInED?
                                                    // >:OO

typedef struct IO IO;

/**
 * A struct to hold some variables so we don't pollute the global namespace.
 * This has to be here because the Unix implementation of this struct actually has stuff in it.
 * For the sake of not breaking things, both implementations will have this struct defined.
*/
struct IO {
  
};

/**
 * Stuff to set up for Windows.
 * In this case, we want the Windows console to understand ANSI escape sequences.
 * 
 * @param   {struct IO *}  this  The IO object to initialize.
*/
void IO_init(struct IO *this) {
  SetConsoleMode(GetStdHandle(STD_OUTPUT_HANDLE), 
    ENABLE_PROCESSED_OUTPUT |                       // We have to enable this first if we want Virtual Terminal Processing
    ENABLE_VIRTUAL_TERMINAL_PROCESSING);            // Virtual Terminal Processing lets us use ANSI escape sequences

  // Enable Unicode character output
  SetConsoleOutputCP(CP_UTF8);
}


/**
 * Helper function that returns the width of the console.
 * Note that this function is responsive to resizing.
 * 
 * @return  {int}   The number of characters along the width of the console.
*/
int IO_getWidth() {

  // I must say this is a painfully long name for a data type
  CONSOLE_SCREEN_BUFFER_INFO consoleScreenBufferInfo;
  
  // Some library functions from windows.h that return the dimensions of the console
  GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &consoleScreenBufferInfo);
  
  // Note the plus one is needed to get the inclusive value of the difference
  return consoleScreenBufferInfo.srWindow.Right - consoleScreenBufferInfo.srWindow.Left + 1;
}

/**
 * Helper function that returns the height of the console.
 * Note that this function is responsive to resizing.
 * 
 * @return  {int}   The number of lines in the console.
*/
int IO_getHeight() {
  
  // I must say this is a painfully long name for a struct
  CONSOLE_SCREEN_BUFFER_INFO consoleScreenBufferInfo;
  
  // Some library functions from windows.h that return the dimensions of the console
  GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &consoleScreenBufferInfo);

  // Note the plus one is needed to get the inclusive value of the difference
  return consoleScreenBufferInfo.srWindow.Bottom - consoleScreenBufferInfo.srWindow.Top + 1;
}

/**
 * A helper function that resizes the console window.
 * This function only works on Windows (I could not find an implementation for Unix users).
 * 
 * Note that the function is implemented this way BECAUSE: 
 *    (1) the buffer size apparently cannot be smaller than the console size AND
 *    (2) the console size cannot be bigger than the screen buffer.
 * 
 * By shrinking the window size to the absolute minimum, we spare ourselves from "crushing" the buffer size into the window size when shrinking.
 * It also prevents the window size from "crashing" into the buffer when making it bigger.
 * When I tried using a rudimentary implementation that did without the minWIndowSize step, some nasty scrollbars appeared on the side.
 * 
 * @param   {int}   dWidth    The new width of the console.
 * @param   {int}   dHeight   The new height of the console.
 * @return  {int}             Boolean indicating whether or not change was successful.
*/
int IO_setSize(int dWidth, int dHeight) {

  // Create some objects to define console properties
  COORD const size = { dWidth, dHeight };

  // Set the console window to the smallest possible size first
  SMALL_RECT const minWindowSize = { 0, 0, 1, 1 };
  SetConsoleWindowInfo(GetStdHandle(STD_OUTPUT_HANDLE), TRUE, &minWindowSize);
  
  // Modify the buffer size 
  // This stores the text on the console and does not reflect the actual console size
  SetConsoleScreenBufferSize(GetStdHandle(STD_OUTPUT_HANDLE), size);
  
  // Update the actual console size into the new window size
  SMALL_RECT const newWindowSize = { 0, 0, dWidth - 1, dHeight - 1 };
  SetConsoleWindowInfo(GetStdHandle(STD_OUTPUT_HANDLE), TRUE, &newWindowSize);

  // The screen size was changed
  // Note that the unix version of the function returns 0 since it does nothing
  return 1;
} 

/**
 * Set the buffer size of the output stream.
 * 
 * @param   { int }   dSize   The size of the buffer in bytes.
*/
void IO_setBuffer(int dSize) {
  
  // This is another thing I found elsewhere which speeds  up
  // Console output by default is buffered per line, which means everytime we counter a \n things slow down
  // In other words, in only prints in chunks of lines
  // In order to circumvent that hindrance, we set the buffer size ourselves

  // _IOFBF means data is written to the output stream once the buffer is full
  // _IOLBF (the default) writes data once a newline is encountered
  // We don't need to do this for Unix anymore cuz it's already quite fast for some reason.
  setvbuf(stdout, NULL, _IOFBF, dSize);
}

/**
 * Resets the cursor position to the start of the terminal.
*/
void IO_resetCursor() {

  // In case the latter doesn't work
  // printf("\x1b[1K");
  // printf("\x1b[3J");
  // printf("\x1b[0;0H");

  // For some reason, this works on Windows conhost and printf("\x1b[0;0H"); SOMETIMES doesn't???
  // For Windows terminal, on the other (Windows 11), the previous solution works?????
  // I can't check for the Windows build version programatically, so Im kinda fcked but oh well
  system("ECHO \"^<ESC^>[0;0H\"");
}

/**
 * Helper function that clears the console.
*/
void IO_clear() {

  // I know system is bad but...
  system("cls");
}

/**
 * Helper function that gets a single character without return key.
 * 
 * @return  {char}  Returns the character read from the conaole.
*/
char IO_readChar() {
  return getch();
}

/**
 * This only exists mainly because I need to do some housekeeping for Unix-based OS's.
 * 
 * @param   {struct IO *}  this  The IO object to clean up.
*/
void IO_exit(struct IO *this) {

  // Reset colors and clear the screen
  printf("\x1b[38;5;255m");
  printf("\x1b[48;5;232m");
  
  for(int i = IO_getHeight(); --i;) 
    for(int j = IO_getWidth(); --j;) 
      printf(" ");
  
  IO_clear();
}

#endif