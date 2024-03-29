/**
 * @ Author: MMMM
 * @ Create Time: 2024-02-05 11:18:06
 * @ Modified time: 2024-03-30 03:43:47
 * @ Description:
 * 
 * A utility library for implementing some string related functionality
 *    on Unix.
 * 
 * The funny thing is I kinda only need this for printing strings, because
 *    for some damn reason puts() is more reliable on Windows and printf() is
 *    more reliable on Linux (at least based from my tests).
 */

#ifndef UTILS_STRING_UNIX_
#define UTILS_STRING_UNIX_

#include <stdio.h>
#include <string.h>
#include <ctype.h>

/**
 * Prints a string.
 * 
 * @param   { char * }  string  The string we want to print.
*/
void String_print(char *string) {
  printf("%s", string);
}

/**
 * Converts a string to upper case.
 * This thing leaks memory...
 * 
 * @param   { char * }  string  The string we want to convert.
 * @return  { char * }          The uppercased string.
*/
char *String_toUpper(char *string) {
  char *newString = calloc(1, strlen(string));
  
  // Return empty string if empty to begin with
  if(!strlen(string))
    return "";

  // Convert to uppercase
  do {
    newString[strlen(newString)] = toupper(*string);
  } while(*(++string));

  return newString;
}

#endif