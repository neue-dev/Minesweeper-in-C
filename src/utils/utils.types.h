/**
 * Typedefs some custom types.
 * We will be using lowercase letters and underscores to denote that something is a data type.
 * 
 * Note that some of these data types take inspiration from the Windows data types 
 *    (bad idea tho, Microsoft creates its own names for everything). 

 * They are reimplemented here for the sake of convenience.
 * Refer to the following documentation for more info: 
 *    https://learn.microsoft.com/en-us/windows/win32/winprog/windows-data-types
*/

#ifndef UTILS_TYPES
#define UTILS_TYPES

// Some primitives
// Gives us fixed-width data types
//    (1) uint8_t
//    (2) uint16_t
//    (3) uint32_t
#include <stdint.h>

// A handle is just a pointer
// It's a Windows convention (?)

// Pointers
typedef void *Handle;                     // A handle to any object we wish
typedef Handle HandleSTD;                 // IO handles
typedef Handle HandleThread;              // Distinguishes threads
typedef Handle HandleMutex;               // Just so we can distinguish mutexes

// FOR THE CODE BELOW (parameter object pointers):
// Note that we have to do this because some of the Windows API requires us to pass
//    both a callback and its arguments separately. The data types here are primarily
//    for representing the arguments to a callback function.

// Parameter object pointers
typedef void *Param;                      // A pointer to a collection of function parameters
typedef Param ParamInt;                   // An integer to be passed as an argument
typedef Param ParamStr;                   // A string to be passed as an argument
typedef Param ParamObj;                   // A pointer to a struct to be passed to a function
typedef void (*ParamFunc)(void *pArgs);   // A pointer to a callback function

#endif