/**
 * @ Author: MMMM
 * @ Create Time: 2024-02-07 02:12:46
 * @ Modified time: 2024-02-25 14:54:33
 * @ Description:
 *    
 * A library that implements graphics-related functionality.
 * Allows coloring the output.
 */

#ifndef UTILS_GRAPHICS_
#define UTILS_GRAPHICS_

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define GRAPHICS_MAX_SEQ 32

/**
 * Color functions
*/
char *Graphics_getCodeFG(int color);

char *Graphics_getCodeBG(int color);

/**
 * //
 * ////
 * //////    Color functions
 * ////////
 * ////////// 
*/

/**
 * Returns the sequence to modify the current color of the terminal.
 * 
 * @param   { int }     color   An integer that stores the RGB information for a certain color (usually notated through hexadecimal).
 * @return  { char * }          A pointer to the string representing the escape sequence.
*/
char *Graphics_getCodeFG(int color) {
  char *sANSISequence = calloc(GRAPHICS_MAX_SEQ, sizeof(char));

  // Create the ANSI escape sequence and parse the RGB values from the int
  // Note that 
  //    1.) the 38; specifies we are changing the foreground color
  //    2.) the 2; specifies the format of the color value input (RGB)
  snprintf(sANSISequence, GRAPHICS_MAX_SEQ, "\x1b[38;2;%d;%d;%dm", 
    (color >> 16) % (1 << 8), 
    (color >> 8) % (1 << 8), 
    (color >> 0) % (1 << 8));

  return sANSISequence;
}

/**
 * Returns the sequence to modify the current color of the background of the terminal.
 * 
 * @param   { int }     color   An integer that stores the RGB information for a certain color (usually notated through hexadecimal).
 * @return  { char * }          A pointer to the string representing the escape sequence.
*/
char *Graphics_getCodeBG(int color) {
  char *sANSISequence = calloc(GRAPHICS_MAX_SEQ, sizeof(char));

  // Create the ANSI escape sequence and parse the RGB values from the int
  // Note that 
  //    1.) the 48; specifies we are changing the background color
  //    2.) the 2; specifies the format of the color value input (RGB)
  snprintf(sANSISequence, GRAPHICS_MAX_SEQ, "\x1b[48;2;%d;%d;%dm", 
    (color >> 16) % (1 << 8), 
    (color >> 8) % (1 << 8), 
    (color >> 0) % (1 << 8));

  return sANSISequence;
}

#endif