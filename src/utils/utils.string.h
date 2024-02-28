/**
 * @ Author: MMMM
 * @ Create Time: 2024-02-24 18:10:41
 * @ Modified time: 2024-02-28 11:50:53
 * @ Description:
 * 
 * Some helper functions that can help us with strings.
 */

#ifndef UTILS_STRING_
#define UTILS_STRING_

#include <stdlib.h>
#include <string.h>

/**
 * //
 * ////
 * //////    String helper functions
 * ////////
 * ////////// 
*/

/**
 * Allocates a piece of memory for a string.
 * 
 * @param   { char * }  sString   The string to allocate memory for.
 * @return  { char * }            A pointer to the allocated string.
*/
char *String_create(char *sString) {
  char *s = calloc(strlen(sString) + 1, sizeof(char));

  strcpy(s, sString);

  return s;
}

/**
 * Allocates a piece of memory for a string GIVEN the size.
 * 
 * @param   { int }     dLength   The length of the memory to allocate.
 * @return  { char * }            A pointer to the allocated string.
*/
char *String_alloc(int dLength) {

  // Account for the null byte if ever
  return calloc(dLength + 1, sizeof(char));
}

/**
 * Deallocates a string.
 * This is here more for code readability.
 * 
 * @param   { char * }  sString   The string to deallocate memory from.
*/
void String_kill(char *sString) {
  free(sString);
}

#endif

