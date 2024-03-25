/**
 * @ Author: MMMM
 * @ Create Time: 2024-02-24 18:10:41
 * @ Modified time: 2024-03-25 21:34:42
 * @ Description:
 * 
 * Some helper functions that can help us with strings.
 */

#ifndef UTILS_STRING_
#define UTILS_STRING_

#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <stdio.h>

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
 * Creates a key with the following format: <key_name>-<key_str>.
 * 
 * @param   { char * }  sKey      Where we want to store the key.
 * @param   { char * }  sKeyName  The name of the key.
 * @param   { char * }  sKeyStr   The string associated with the key.
*/
void String_keyAndStr(char *sKey, char *sKeyName, char *sKeyStr) {
  snprintf(sKey, STRING_KEY_MAX_LENGTH, "%s-%s", sKeyName, sKeyStr);
}

/**
 * Joins multiple strings together with the specified sequence.
 * This function always assumes that the last argument is equal to the terminator.
 * 
 * @param   { char * }  sDelimeter    The joining sequence.
 * @param   { char * }  sTerminator   The last argument must be equal to this.
 * @param   { int }     dWrapLength   Length before a line becomes a "new line" (a line delimeted by sDelimeter).
 * @param   { char * }  ...           A bunch of strings to join.
 * @return  { char * }                The joined strings.    
*/
char *String_join(char *sDelimeter, char *sTerminator, int dWrapLength, ...) {

  // Variable string params
  va_list vStrings;
  va_start(vStrings, dWrapLength);
  char *string, *sOutput = calloc(1, 1);
  int dStartingPoint = 0, bIsDelimeter = 0, i;

  // We go out of the loop once we hit the last argument
  for(;;) {
    
    // Reallocate string each time
    // Note the + length of the delimeter
    string = va_arg(vStrings, char *);
    sOutput = realloc(sOutput, (
      strlen(sOutput) + 
      strlen(string) + 
      strlen(sDelimeter) + 1) * sizeof(char));

    // We've hit the terminator
    if(!strcmp(string, sTerminator)) {
      va_end(vStrings);
      return sOutput;
    }
    
    // If the text has to wrap around
    if(String_charCount(string) > dWrapLength && dWrapLength) {
      
      // Resize sOutput to account for new size
      sOutput = realloc(sOutput, (
        strlen(sOutput) + 
        strlen(string) + 
        strlen(sDelimeter) * 
        (String_charCount(string) / dWrapLength + 4) + 4) * sizeof(char));

      // Starting point for that line
      dStartingPoint = 0;

      // Start copying the string line by line
      while(dStartingPoint < strlen(string)) {

        // Copy string until wrap length
        strncat(sOutput, string + dStartingPoint, dWrapLength);
        dStartingPoint += dWrapLength;

        // Remove end characters until we hit a space IF we're not on the last line
        if(dStartingPoint < strlen(string)) {
          while(sOutput[strlen(sOutput) - 1] != 32) {

            // Otherwise, keep shrinking the string
            sOutput[strlen(sOutput) - 1] = 0;
            dStartingPoint--;
            
            // Output got too short
            if(strlen(sOutput) < strlen(sDelimeter)) {
              if(strlen(sOutput) < 1) {
                strncat(sOutput, string + dStartingPoint, dWrapLength);
                sOutput[strlen(sOutput) - 1] = 32;
                dStartingPoint += dWrapLength - 1;
              }

            // We've possibly hit the end of the previous line
            } else {
              bIsDelimeter = 1;
              
              // Check if we've hit the delimeter
              for(i = strlen(sDelimeter) - 1; i >= 0; i--) {
                if(sOutput[strlen(sOutput) - i - 1] != sDelimeter[i])
                  bIsDelimeter = 0;
              }


              // Just fck it and copy the next line cuz the token length was too long
              if(bIsDelimeter) {
                strncat(sOutput, string + dStartingPoint, dWrapLength);
                sOutput[strlen(sOutput) - 1] = 32;
                dStartingPoint += dWrapLength - 1;
              }
            }
          }
        }

        // Copy the delimeter
        strcat(sOutput, sDelimeter);
      }

    // Otherwise, just copy the line
    } else {
      strcat(sOutput, string);
      strcat(sOutput, sDelimeter);
    }
  }
}

/**
 * Repeat a certain string pattern before a certain length is reached.
 * 
 * @param   { char * }  sUnit     The unit to repeat.
 * @param   { int }     dLength   Length up until to keep repeating; note that this is number of chars not number of bytes.
 * @return  { char * }            The output string.
*/
char *String_repeat(char *sUnit, int dLength) { 

  // It's *4 because we have to account for Unicode chars
  char *sOutput = String_alloc(dLength * 4 + 4);

  // Create the pattern string
  while(String_charCount(sOutput) + String_charCount(sUnit) < dLength)
    strcat(sOutput, sUnit);

  return sOutput;
}

/**
 * Converts an escape character into an escaped sequence of characters.
 * If the character is not an escape character, the function returns the original char.
 * 
 * @param   { char }    c   The escape character we want to convert.
 * @return  { char * }      The escaped sequence representing the character.
*/
char *String_renderEscChar(char c) {
  char *sDefaultOut;

  switch(c) {
    case '\a': return "\\a";
    case '\b': return "\\b";
    case '\e': return "\\e";
    case '\f': return "\\f";
    case '\n': return "\\n";
    case '\r': return "\\n";    // Fck windows, we're calling '\r' as '\n'
    case '\t': return "\\t";
    case '\v': return "\\v";
    
    // Just return the original character
    default: 
      sDefaultOut = String_alloc(1); 
      sDefaultOut[0] = c;
      
      return sDefaultOut; 
  }
}

// You are in Windows
#ifdef _WIN32
#include "./win/utils.string.win.h"

// Not in Windows
#else
#include "./unix/utils.string.unix.h"
#endif

#endif

