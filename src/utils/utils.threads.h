/**
 * A utility library for implementing threads.
*/

#ifndef UTILS_THREADS
#define UTILS_THREADS

#include "../utils.types.h"

// Windows for now
#ifdef _WIN32
#include "./win/utils.threads.win.h"

// Not in Windows
#else
#include "./unix/utils.threads.unix.h"
#endif

#endif