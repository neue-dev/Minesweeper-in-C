/**
 * @ Author: MMMM
 * @ Create Time: 2024-03-02 16:49:20
 * @ Modified time: 2024-03-18 11:58:24
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
  int i = 0;

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
 * This function overwrites the current data on a file.
 * 
 * @param   { File * }  this              The file we want to write to.
 * @param   { int }     n                 How many lines we want to write.
 * @param   { char * }  sContentBuffer[]  The lines we want to write.
*/
void File_writeText(File *this, int n, char *sContentBuffer[]) {
  int i;

  // Open the file
  this->pFile = fopen(this->sPath, "w");

  // The file could not be opened
  if(this->pFile == NULL)
    return;

  // Write the content to the file
  for(i = 0; i < n; i++) {
    fputs(sContentBuffer[i], this->pFile);
    fputs("\n", this->pFile);
  }

  fclose(this->pFile);
}

/**
 * Writes to a binary file.
 * This function overwrites the current data on a file.
 * If the given pointer to the data object is NULL, we just exit the function.
 * 
 * @param   { File * }              this      The file we want to write to.
 * @param   { int }                 n         The size of the object in bytes.
 * @param   { p_obj }               pObject   The data of the object we want to write.
*/
void File_writeBin(File *this, int n, p_obj pObject) {

  // Open the file
  this->pFile = fopen(this->sPath, "wb");

  // The file could not be opened
  if(this->pFile == NULL)
    return;

  // If the pointer is somehow NULL
  if(pObject == NULL)
    return;

  //Write the data to the file
  fwrite(pObject, n, 1, this->pFile);

  // Close the file
  fclose(this->pFile);
}


#endif
