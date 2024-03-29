/**
 * @ Author: MMMM
 * @ Create Time: 2024-03-02 16:49:20
 * @ Modified time: 2024-03-29 21:44:19
 * @ Description:
 * 
 * Sometimes, it's better to abstract the implementation of a service inside a class for the
 *    sake of convenience and consistency throughout the code. Also, I'm not sure if 
 *    I'll come across something that differs between Windows and Linux with regard to 
 *    file handling. 
 */

#ifndef UTILS_FILE_
#define UTILS_FILE_

#include "./utils.string.h"
#include "./utils.types.h"

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
 * Reads the text file stored at the path.
 * 
 * @param   { File * }    this            The file to read from.
 * @param   { int }       n               How many lines we have in our output buffer.
 * @param   { int * }     h               The actual height of the output buffer.
 * @param   { char ** }   sOutputBuffer   We're to store the file contents.
*/
void File_readText(File *this, int n, int *h, char **sOutputBuffer) {
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

/**
 * Reads a single entry from the binary file stored at the path.
 * 
 * @param   { File * }    this            The file to read from.
 * @param   { int }       dOffset         The offset from which to begin reading.
 * @param   { int }       n               The size of the object to write to.
 * @param   { p_obj }     pOutputObject   The object to write the data to.
*/
void File_readBinObject(File *this, int dOffset, int n, p_obj pOutputObject) {

  // Open the file
  this->pFile = fopen(this->sPath, "rb");

  // Exit the routine if it's not found or smth
  if(this->pFile == NULL)
    return;

  // Set the offset first
  fseek(this->pFile, dOffset, SEEK_SET);

  // Write the data and spit it into the object
  fread(pOutputObject, n, 1, this->pFile);

  // Close the file
  fclose(this->pFile);
}

/**
 * Writes to a text file.
 * This function only appends data to the existing content of the file.
 * 
 * @param   { File * }  this              The file we want to write to.
 * @param   { int }     n                 How many lines we want to write.
 * @param   { char * }  sContentBuffer[]  The lines we want to write.
*/
void File_writeText(File *this, int n, char *sContentBuffer[]) {
  int i;

  // Open the file
  this->pFile = fopen(this->sPath, "a");

  // The file could not be opened
  if(this->pFile == NULL)
    return;

  // Write the content to the file
  for(i = 0; i < n; i++)
    fputs(sContentBuffer[i], this->pFile);

  fclose(this->pFile);
}

/**
 * Writes to a binary file.
 * This function only appends data to the existing content of the file.
 * If the given pointer to the data object is NULL, we just exit the function.
 * 
 * @param   { File * }              this      The file we want to write to.
 * @param   { int }                 n         The size of the object in bytes.
 * @param   { p_obj }               pObject   The data of the object we want to write.
 * @return  { int }                           The starting point of the written data. -1 if the function exited prematurely.
*/
int File_writeBin(File *this, int n, p_obj pObject) {
  int fpos = -1;

  // Open the file
  this->pFile = fopen(this->sPath, "ab");

  // The file could not be opened
  if(this->pFile == NULL)
    return -1;

  // If the pointer is somehow NULL
  if(pObject == NULL) {
    fclose(this->pFile);
    return -1;
  }

  // Get the position of the "cursor" before writing
  fpos = ftell(this->pFile);

  //Write the data to the file
  fwrite(pObject, n, 1, this->pFile);

  // Close the file
  fclose(this->pFile);

  // Return the starting point of the data
  return fpos;
}

/**
 * Clears the contents of a file.
 * Literally just opens it in write mode.
 * 
 * @param   { File * }  this  The file to clear.
*/
void File_clear(File *this) {
  FILE *pFile = fopen(this->sPath, "w");

  if(pFile == NULL)
    return;

  fclose(pFile);
}

/**
 * Checks if a file exists or not.
 * 
 * @param   { char * }    sFilename     The list of filenames in the directory.
 * @return  { int }                     Whether or not the file exists.
*/
int File_exists(char *sFilename) {
  FILE *pFile = fopen(sFilename, "r");

  // File does not exist LMAO
  if(pFile == NULL)
    return 0;

  // If file does exist, return 1
  fclose(pFile);
  return 1;
}

/**
 * Creates a new file with the specified name.
 * If the file already exists, it leaves it untouched.
 * Returns 1 on success, 0 on failure.
 * 
 * @param   { char * }    sFilename     The list of filenames in the directory.
 * @return  { int }                     Whether or not the operation was successful.
*/
int File_newFile(char *sFilename) {
  FILE *pFile = fopen(sFilename, "a+");

  // Something went wrong
  if(pFile == NULL)
    return 0;

  // Success
  fclose(pFile);
  return 1;
}


#endif
