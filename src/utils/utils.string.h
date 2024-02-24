/**
 * @ Author: Mo David
 * @ Create Time: 2024-02-24 18:10:41
 * @ Modified time: 2024-02-24 18:18:00
 * @ Description:
 * 
 * Some helper functions that can help us with strings.
 */

#ifndef UTILS_STRING_
#define UTILS_STRING_

#include <stdlib.h>
#include <string.h>

/**
 * Allocates a piece of memory for a string.
 * 
 * @param   { char * }  string  The string to allocate memory for.
 * @return  { char * }          A pointer to the allocated string.
*/
char *String_new(char *string) {
  char *s = calloc(strlen(string) + 1, sizeof(char));

  strcpy(s, string);

  return s;
}

#endif

