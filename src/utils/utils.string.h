/**
 * @ Author: MMMM
 * @ Create Time: 2024-02-24 18:10:41
 * @ Modified time: 2024-03-31 23:54:36
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
#define STRING_FILENAME_MAX_LENGTH (1 << 8)

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
 * Clears a string (sets everything to 0).
 * 
 * @param  { char * }  A pointer to the allocated string.
*/
void String_clear(char *string) {
  memset(string, 0, strlen(string));
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

/**
 * Returns a string that shows seconds in a more readable format.
 * Format is mins:secs. If mins > 60, it is shown as is.
 * 
 * @param   { int }     dSeconds  The number of seconds.
 * @return  { char * }            A string describing the time elapsed.
*/
char *String_formatSecs(int dSeconds) {
  char *sTimeString = String_alloc(16);

  // Create the time string
  sprintf(sTimeString, (dSeconds % 60) < 10 ? "%d:0%d" : "%d:%d", 
    dSeconds / 60, 
    dSeconds % 60);

  return sTimeString;
}

/**
 * Checks if a string has only valid characters. 
 * 
 * @param   { char * }  sFilename   The filename to check.
 * @return  { int }                 A 0 if the filename is invalid; otherwise, a 1 is returned.
*/
int String_isValidFilename(char *sFilename) {
  int i;
  
  // Checks if the name's length exceeds the max or is 0
  if(strlen(sFilename) > STRING_FILENAME_MAX_LENGTH || strlen(sFilename) == 0)
    return 0;

  // Checks if the name contains invalid characters
  for(i = 0; i < strlen(sFilename); i++) {
      
    if(!((sFilename[i] >= 'A' && sFilename[i] <= 'Z') ||
      (sFilename[i] >= 'a' && sFilename[i] <= 'z') ||
      (sFilename[i] >= '0' && sFilename[i] <= '9') ||
      sFilename[i] == '-' || sFilename[i] == '_' || 
      sFilename[i] == '.' || sFilename[i] == ' ')) {
        
        return 0;
    }
  }

  // There were no problems with the filename
  return 1;
}

/**
 * Returns whether or not a character is not an escape char.
 * 
 * @param   { char }  c   The character to check.
 * @return  { int }       Whether or not the character can be "typed" by the user.
*/
int String_isValidChar(char c) {
  switch(c) {

    // Check for escape chars
    case '\a': case '\b': case '\e': case '\f': 
    case '\n': case '\r': case '\t': case '\v': 
      return 0;
    
    // Just return the original character
    default: 
      if(c < 32 || c == 127) 
        return 0;
      return 1;
  }
}

/**
 * Leaves the original string untouched.
 * Replaces every instance of a character with another character.
 * 
 * @param   { char * }  string  The string to modify.
 * @param   { char }    cOld    The character to replace.
 * @param   { char }    cNew    The new character to use.
 * @return  { char * }          The modified string.
*/
char *String_replace(char *string, char cOld, char cNew) {
  char *outString = String_alloc(strlen(string) + 1);
  
  // Replace chars
  while(*string) {
    sprintf(outString, "%s%c", outString, *string == cOld ? cNew : *string);
    string++;
  }

  return outString;
}

// You are in Windows
#ifdef _WIN32
#include "./win/utils.string.win.h"

// Not in Windows
#else
#include "./unix/utils.string.unix.h"
#endif

#endif

