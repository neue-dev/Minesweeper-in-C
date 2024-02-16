/**
 * A utility library for implementing threads in Windows.
*/

#ifndef UTILS_THREAD_WIN
#define UTILS_THREAD_WIN

#include "../utils.types.h"

#include <windows.h>
#include <conio.h>

typedef struct Thread {
  
  // The output console
  HandleSTD hConsoleOut;
  HandleMutex hStateMutex;    // Whether or not the thread should still keep running
                              // Also tells the thread whether or not it can update
  
  
  // ! Define mutexes needed (what common information do they share)
  // handle hMutex;
  // handle h
} Thread;

#endif