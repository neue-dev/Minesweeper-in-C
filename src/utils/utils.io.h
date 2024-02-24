/**
 * @ Author: MMMM
 * @ Create Time: 2024-02-17 19:37:04
 * @ Modified time: 2024-02-24 13:44:35
 * @ Description:
 *    
 * A file containing some useful helper functions for low-level input and output operations.
 */

#ifndef UTILS_IO_
#define UTILS_IO_

// Some other important header files
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// Some useful constants
#define IO_BS 8       // Backspace
#define IO_LF 10      // Line feed
#define IO_CR 13      // Carriage return
#define IO_SPACE 32   // Space
#define IO_DEL 127    // Delete

#define IO_MAX_INPUT 1024

// You're on Windows
#ifdef _WIN32
#include "./win/utils.io.win.h"

// Wow, you're not on Windows!
#else
#include "./unix/utils.io.unix.h"
#endif

#endif