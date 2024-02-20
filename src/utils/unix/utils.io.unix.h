/**
 * Low level handling of IO functionalities on Unix environments
*/

#ifndef UTILS_IO_UNIX
#define UTILS_IO_UNIX

#include <sys/ioctl.h>
#include <termios.h>
#include <unistd.h>

/**
 * A struct to hold some variables so we don't pollute the global namespace.
 * Stores the original settings of the terminal so we can revert them back after the program exuts.
*/
typedef struct IO {
  struct termios defaultSettings;
  struct termios overrideSettings;
} IO;

/**
 * Sets up some stuff for IO handling.
 * Overrides default terminal settings so I can replicate getch behaviour on Unix-based OS's.
 * 
 * @param   {struct IO *}  this  The IO object to initialize.
*/
void IO_init(struct IO *this) {

  // Save the default settings of the terminal before overriding them
  // This function is from termios.h
  tcgetattr(STDIN_FILENO, &this->defaultSettings);

  // Create the override
  this->overrideSettings = this->defaultSettings;

  // ICANON usually restricts the terminal to read a single line at a time
  // ICANON also terminates input when \n is encountered
  // ECHO spits the user's keystrokes back at the terminal (like when typing)
  // By disabling both we can mimic getch behaviour in a Unix environment!
  this->overrideSettings.c_lflag &= ~(ICANON | ECHO);

  tcsetattr(0, TCSANOW, &this->overrideSettings);
}

/**
 * Helper function that returns the width of the console.
 * Note that this function is responsive to resizing.
 * 
 * @return  {int}   The number of characters along the width of the console.
*/
int IO_getWidth() {
  
  // A library function from ioctl.h that gets the current terminal size
  struct winsize windowSize;
  ioctl(0, TIOCGWINSZ, &windowSize);

  return windowSize.ws_col;
}

/**
 * Helper function that returns the height of the console.
 * Note that this function is responsive to resizing.
 * 
 * @return  {int}   The number of lines in the console.
*/
int IO_getHeight() {
  
  // A library function from ioctl.h that gets the current terminal size
  struct winsize windowSize;
  ioctl(0, TIOCGWINSZ, &windowSize);

  return windowSize.ws_row;
}

/**
 * A helper function that resizes the console window.
 * Unfortunately, I could not find a POSIX-compliant implementation.
 * This is just here as a dummy function.
 * 
 * @param   {int}   dWidth    The new width of the console.
 * @param   {int}   dHeight   The new height of the console.
 * @return  {int}             Boolean indicating whether change was successful.
*/
int IO_setSize(int dWidth, int dHeight) {
  return 0;
} 

/**
 * Helper function that clears the console.
*/
void IO_clear() {

  // A neat regex expression that clears the current console and moves the cursor
  // \e[H   Puts the cursor at the home position 
  // \e[2J  Erases entire screen
  // \e[3J  Erases saved lines
  printf("\e[H\e[2J\e[3J");
}

/**
 * Helper function that gets a single character without return key.
 * 
 * @return  {char}  Returns the character read from the conaole.
*/
char IO_readChar() {
  return getchar();
}

/**
 * Clean up the stuff I used.
 * 
 * @param   {struct IO *}  this  The IO object to clean up.
*/
void IO_exit(struct IO *this) {

  // Reset the colors 
  printf("\x1b[38;5;255m");
  printf("\x1b[48;5;232m");
  
  for(int i = IO_getHeight(); --i;) 
    for(int j = IO_getWidth(); --j;) 
      printf(" ");

  IO_clear();

  // Return the terminal to its default state
  // Again, the function is from termios.h
  tcsetattr(STDIN_FILENO, TCSANOW, &this->defaultSettings);
}

#endif