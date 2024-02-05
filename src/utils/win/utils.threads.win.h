/**
 * A utility library for implementing threads in Windows.
*/

#ifndef UTILS_THREADS_WIN
#define UTILS_THREADS_WIN

#include <windows.h>
#include <conio.h>

struct Thread {
  // The output console
  handle hConsoleOut;
  
  // ! Define mutexes needed (what common information do they share)
  // handle hMutex;
  // handle h
};

#endif