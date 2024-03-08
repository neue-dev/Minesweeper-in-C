/**
 * @ Author: MMMM
 * @ Create Time: 2024-02-07 02:12:46
 * @ Modified time: 2024-03-08 09:47:48
 * @ Description:
 *    
 * A library that implements graphics-related functionality.
 * Allows coloring the output.
 * YOU MUST CALL free() on the strings produced by these functions.
 */

#ifndef UTILS_GRAPHICS_
#define UTILS_GRAPHICS_

#include "./utils.string.h"
#include "./utils.math.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define GRAPHICS_STD_SEQ 32

/**
 * Color functions
*/
char *Graphics_getCodeFG(color color);

char *Graphics_getCodeBG(color color);

char *Graphics_getCodeFGBG(color colorFG, color colorBG);

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
 * @param   { color }   color   An integer that stores the RGB information for a certain color (usually notated through hexadecimal).
 * @return  { char * }          A pointer to the string representing the escape sequence.
*/
char *Graphics_getCodeFG(color color) {
  int i;
  char *sANSISequence = String_alloc(GRAPHICS_STD_SEQ);

  // Create the ANSI escape sequence and parse the RGB values from the int
  // Note that 
  //    1.) the 38; specifies we are changing the foreground color
  //    2.) the 2; specifies the format of the color value input (RGB)
  snprintf(sANSISequence, GRAPHICS_STD_SEQ, "\x1b[38;2;%d;%d;%dm", 
    (color >> 16) % (1 << 8), 
    (color >> 8) % (1 << 8), 
    (color >> 0) % (1 << 8));

  // This gives things a consistent width
  // Also for some reason it doesn't break the output??
  i = strlen(sANSISequence);
  while(i < sizeof(*sANSISequence) - 1)
    sANSISequence[i++] = 32;

  return sANSISequence;
}

/**
 * Returns the sequence to modify the current color of the background of the terminal.
 * 
 * @param   { color }   color   An integer that stores the RGB information for a certain color (usually notated through hexadecimal).
 * @return  { char * }          A pointer to the string representing the escape sequence.
*/
char *Graphics_getCodeBG(color color) {
  int i;
  char *sANSISequence = String_alloc(GRAPHICS_STD_SEQ);

  // Create the ANSI escape sequence and parse the RGB values from the int
  // Note that 
  //    1.) the 48; specifies we are changing the background color
  //    2.) the 2; specifies the format of the color value input (RGB)
  snprintf(sANSISequence, GRAPHICS_STD_SEQ, "\x1b[48;2;%d;%d;%dm", 
    (color >> 16) % (1 << 8), 
    (color >> 8) % (1 << 8), 
    (color >> 0) % (1 << 8));

  // This gives things a consistent width
  // Also for some reason it doesn't break the output??
  i = strlen(sANSISequence);
  while(i < sizeof(*sANSISequence) - 1)
    sANSISequence[i++] = 32;

  return sANSISequence;
}

/**
 * Returns the sequence to modify the current color of the foreground AND background of the terminal.
 * 
 * @param   { color }   colorFG   An integer that stores the RGB information for a certain color for the foreground.
 * @param   { color }   colorBG   An integer that stores the RGB information for a certain color for the background.
 * @return  { char * }            A pointer to the string representing the escape sequence.
*/
char *Graphics_getCodeFGBG(color colorFG, color colorBG) {
  int i;
  char *sANSISequence = String_alloc(GRAPHICS_STD_SEQ * 2);

  // A combination of the two methods above
  snprintf(sANSISequence, GRAPHICS_STD_SEQ * 2, "\x1b[38;2;%d;%d;%dm\x1b[48;2;%d;%d;%dm", 
    (colorFG >> 16) % (1 << 8), 
    (colorFG >> 8) % (1 << 8), 
    (colorFG >> 0) % (1 << 8),
    (colorBG >> 16) % (1 << 8), 
    (colorBG >> 8) % (1 << 8), 
    (colorBG >> 0) % (1 << 8));

  // This gives things a consistent width
  // Also for some reason it doesn't break the output??
  i = strlen(sANSISequence);
  while(i < sizeof(*sANSISequence) - 1)
    sANSISequence[i++] = 32;

  return sANSISequence;
}

/**
 * Returns the "pythagorean distance" between two colors.
 * 
 * @param   { color }   color1  The first color.
 * @param   { color }   color2  The second color.
 * @return  { float }           The pythagorean distance between the two colors, with respect to their RGB values.
*/
float Graphics_getColorDist(color color1, color color2) {
  return sqrtf(
    ((color1 >> 16) % (1 << 8) - (color2 >> 16) % (1 << 8)) * 
    (color1 >> 16) % (1 << 8) - (color2 >> 16) % (1 << 8) + 
    ((color1 >> 8) % (1 << 8) - (color2 >> 8) % (1 << 8) * 
    (color1 >> 8) % (1 << 8) - (color2 >> 8) % (1 << 8)) + 
    ((color1 >> 0) % (1 << 8) - (color2 >> 0) % (1 << 8) * 
    (color1 >> 0) % (1 << 8) - (color2 >> 0) % (1 << 8)) * 1.0);
}

/**
 * Returns the color associated with a set of rgb values.
 * 
 * @param   { int }     r   The red value of the color.
 * @param   { int }     g   The green value of the color.
 * @param   { int }     b   The blue value of the color.
 * @return  { color }       A color value with the rgb specified.  
*/
color Graphics_RGB(int r, int g, int b) {
  return (r << 16) + (g << 8) + b;
}

/**
 * Lerps between two colors by the specified amount.
 * 
 * @param   { color }   color1    The first (start) color.
 * @param   { color }   color2    The second (end) color.
 * @param   { float }   fAmount   How much to lerp between the two colors.
 * @return  { int }               The new color.
*/
int Graphics_lerp(color color1, color color2, float fAmount) {
  int r1 = (color1 >> 16) % (1 << 8), r2 = (color2 >> 16) % (1 << 8);
  int g1 = (color1 >> 8) % (1 << 8), g2 = (color2 >> 8) % (1 << 8);
  int b1 = (color1 >> 0) % (1 << 8), b2 = (color2 >> 0) % (1 << 8);

  return Graphics_RGB(
    round(Math_lerp(r1 * 1.0, r2 * 1.0, fAmount)),
    round(Math_lerp(g1 * 1.0, g2 * 1.0, fAmount)),
    round(Math_lerp(b1 * 1.0, b2 * 1.0, fAmount))
  );
}

/**
 * Releases allocated memory.
 * 
 * @param   { char * }  sCode   A pointer to the memory we're gonna free.
*/
void Graphics_delCode(char *sCode) {
  String_kill(sCode);
}

#endif