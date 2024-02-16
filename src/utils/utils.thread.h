/**
 * A utility library for implementing threads.
*/

#ifndef UTILS_THREAD
#define UTILS_THREAD

// Windows for now
#ifdef _WIN32
#include "./win/utils.thread.win.h"

// Not in Windows
#else
#include "./unix/utils.thread.unix.h"
#endif

#endif