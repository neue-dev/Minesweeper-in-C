/**
 * @ Author: MMMM
 * @ Create Time: 2024-03-02 16:49:20
 * @ Modified time: 2024-03-18 11:13:00
 * @ Description:
 * 
 * Sometimes, it's better to abstract the implementation of a service inside a class for the
 *    sake of convenience and consistency throughout the code. Also, I'm not sure if 
 *    I'll come across something that differs between Windows and Linux with regard to 
 *    file handling. 
 */

#ifndef UTILS_FILE_
#define UTILS_FILE_

#include <stdio.h>

#define FILE_MAX_LINE_LEN (1 << 12)
#define FILE_MAX_LINES (1 << 12)

typedef struct File File;

/**
 * //
 * ////
 * //////    File class
 * ////////
 * ////////// 
*/

/**
 * This file helps encapsulate  the functions and services offered by the stdio library
 *    with regard to file handling. This makes things more convenient and customizable
 *    for us developers. 
 * 
 * @class
*/
struct File {

  char *sPath;  // The path to the file
  FILE *pFile;  // A pointer to the file

};

/**
 * Allocates memory for an instance of the File class.
 * 
 * @return	{ File * }		A pointer to the allocated memory.
*/
File *File_new() {
  File *pFile = calloc(1, sizeof(*pFile));
  return pFile;
}

/**
 * Initializes an instance of the File class.
 * 
 * @param		{ File * }		this	  A pointer to the instance to initialize.
 * @param   { char * }    sPath   A path to the file we wish to use.
 * @return	{ File * }					  A pointer to the initialized instance.
*/
File *File_init(File *this, char *sPath) {
  this->sPath = sPath;

  return this;
}

/**
 * Creates an initialized instance of the File class.
 * 
 * @param   { char * }    sPath   A path to the file we wish to use.
 * @return	{ File * }		A pointer to the newly created initialized instance.
*/
File *File_create(char *sPath) {
  return File_init(File_new(), sPath);
}

/**
 * Deallocates the memory of an instance of the File class.
 * 
 * @param		{ File * }		this	A pointer to the instance to deallocate.
*/
void File_kill(File *this) {
  free(this);
}

/**
 * Reads the file stored at the path.
 * 
 * @param   { File * }    this            The file to read from.
 * @param   { int }       n               How many lines we have in our output buffer.
 * @param   { int * }     h               The actual height of the output buffer.
 * @param   { char ** }   sOutputBuffer   We're to store the file contents.
*/
void File_read(File *this, int n, int *h, char **sOutputBuffer) {
  int i = 0;
  char *s = String_alloc(FILE_MAX_LINE_LEN);

  // Open the file
  this->pFile = fopen(this->sPath, "r");

  // Exit the routine if it's not found or smth
  if(this->pFile == NULL)
    return;

  // While there are lines to read
  while(fgets(s, FILE_MAX_LINE_LEN, this->pFile) != NULL) {

    // Copy each line to the output buffer
    sOutputBuffer[i] = String_alloc(FILE_MAX_LINE_LEN);
    snprintf(sOutputBuffer[i++], FILE_MAX_LINE_LEN, "%s", s);

    // We shouldn't overflow our output buffer OR read too many lines
    if(i >= n || i >= FILE_MAX_LINES) {
      sOutputBuffer[i - 1] = "\0";

      fclose(this->pFile);
      return;
    }
  }

  // Always set the last line to a null string
  if(i + 1 < n) sOutputBuffer[i + 1] = "\0";
  else sOutputBuffer[i] = "\0";

  // Set the height
  *h = i;

  // Close the file
  fclose(this->pFile);
}


#endif
