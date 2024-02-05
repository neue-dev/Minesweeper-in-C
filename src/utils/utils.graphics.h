/**
 * A library for rendering graphics onto the screen.
*/

#include "../utils.types.h"
#include <stdio.h>

/**
 * ########################################
 * #### Color Functions
 * ########################################
*/

/**
 * Sets the current color of the text of the terminal.
 * 
 * @param   {int}   color   An integer that stores the RGB information for a certain color (usually notated through hexadecimal).
*/
void Graphics_setForeground(int color) {
  char sANSISequence[32];

  // Create the ANSI escape sequence and parse the RGB values from the int
  // Note that 
  //    1.) the 38; specifies we are changing the foreground color
  //    2.) the 2; specifies the format of the color value input (RGB)
  sprintf(sANSISequence, "\x1b[38;2;%d;%d;%dm", 
    (color >> 16) % (1 << 8), 
    (color >> 8) % (1 << 8), 
    (color >> 0) % (1 << 8));

  printf("%s", sANSISequence);
}

/**
 * Sets the current color of the background of the terminal.
 * 
 * @param   {int}   color   An integer that stores the RGB information for a certain color (usually notated through hexadecimal).
*/
void Graphics_setBackground(int color) {
  char sANSISequence[32];

  // Create the ANSI escape sequence and parse the RGB values from the int
  // Note that 
  //    1.) the 48; specifies we are changing the background color
  //    2.) the 2; specifies the format of the color value input (RGB)
  sprintf(sANSISequence, "\x1b[48;2;%d;%d;%dm", 
    (color >> 16) % (1 << 8), 
    (color >> 8) % (1 << 8), 
    (color >> 0) % (1 << 8));

  printf("%s", sANSISequence);
}
