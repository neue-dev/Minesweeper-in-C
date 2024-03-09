/**
 * @ Author: MMMM
 * @ Create Time: 2024-02-24 18:10:41
 * @ Modified time: 2024-03-09 17:31:58
 * @ Description:
 * 
 * Some helper functions that can help us with strings.
 */

#ifndef UTILS_STRING_
#define UTILS_STRING_

#include <stdlib.h>
#include <string.h>

#define STRING_KEY_MAX_LENGTH (1 << 8)

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

/**
 * Checks whether or not the character represents the start of a "compound" character (We refer to a character as 
 *    compound if it needs more than one char instances to be represented, eg UTF-8 characters).
 * 
 * @param   { char }  c   The character we wish to check.
 * @return  { int }       Whether or not the character starts a series of char bytes that represents a single character.
*/
int String_isStartChar(char c) {
  return (unsigned char) c >> 6 == 3;
}

/**
 * Checks whether or not the character is part of a longer character encoding.
 * 
 * @param   { char }  c   The character we wish to check.
 * @return  { int }       Whether or not the character is an ASCII character or not.
*/
int String_isChar(char c) {
  return !((unsigned char) c >> 6 > 1);
}

/**
 * Gets the number of characters in a string, including multi-byte characters.
 * 
 * @param   { char * }  string  The string to get the length of.
 * @return  { int }             The length of the string.
*/
int String_charCount(char *string) {
  int dLen = 0;

  // While not null byte
  while(*string) {

    // Check if start of compound char or ASCII char
    if(String_isChar(*string) ||
      String_isStartChar(*string))
      dLen++;

    string++;
  }

  return dLen;
}

/**
 * Returns the last character in a string.
 * 
 * @param   { char * }  string  The string to read.
 * @return  { char }            The last char of the string.
*/
char String_getLast(char *string) {
  return string[strlen(string) - 1];
}

/**
 * Creates a key with the following format: <key_name>-<key_id>.
 * 
 * @param   { char * }  sKey      Where we want to store the key.
 * @param   { char * }  sKeyName  The name of the key.
 * @param   { int }     sKeyId    The id of the key.
*/
void String_keyAndId(char *sKey, char *sKeyName, int sKeyId) {
  snprintf(sKey, STRING_KEY_MAX_LENGTH, "%s-%d", sKeyName, sKeyId);
}

/**
 * Creates a key with the following format: <key_name>-<key_char>.
 * 
 * @param   { char * }  sKey      Where we want to store the key.
 * @param   { char * }  sKeyName  The name of the key.
 * @param   { int }     sKeyChar  The char of associated with the key.
*/
void String_keyAndChar(char *sKey, char *sKeyName, char sKeyChar) {
  snprintf(sKey, STRING_KEY_MAX_LENGTH, "%s-%c", sKeyName, sKeyChar);
}

/**
 * Creates a key with the following format: <key_name>-<key_char>.
 * 
 * @param   { char * }  sKey      Where we want to store the key.
 * @param   { char * }  sKeyName  The name of the key.
 * @param   { char * }  sKeyStr   The string associated with the key.
*/
void String_keyAndStr(char *sKey, char *sKeyName, char *sKeyStr) {
  snprintf(sKey, STRING_KEY_MAX_LENGTH, "%s-%s", sKeyName, sKeyStr);
}

// You are in Windows
#ifdef _WIN32
#include "./win/utils.string.win.h"

// Not in Windows
#else
#include "./unix/utils.string.unix.h"
#endif

#endif

