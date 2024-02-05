/**
 * Typedefs some custom types.
 * We will be using lowercase letters and underscores to denote that something is a data type.
 * 
 * Note that some of these data types take inspiration from the Windows data types 
 *    (bad idea tho, Microsoft creates its own names for everything). 

 * They are reimplemented here for the sake of running the program on Unix-based devices.
 * Refer to the following documentation for more info: 
 *    https://learn.microsoft.com/en-us/windows/win32/winprog/windows-data-types
*/

#ifndef UTILS_TYPES
#define UTILS_TYPES

// Some primitives
// Gives us fixed width data types
#include <stdint.h>                                             // Useful types:
                                                                //    uint8_t
                                                                //    uint16_t
                                                                //    uint32_t

// Some primitive structs
typedef struct { int x; int y; } i_vec_2d;                      // 2D integer vectors
typedef struct { float x; float y; } f_vec_2d;                  // 2D float vectors

// Pointer types
typedef void *handle;                                           // A handle to any object we wish
typedef handle handle_io;                                       // IO handles
typedef handle handle_thread;                                   // Distinguishes threads
typedef handle handle_mutex;                                    // Just so we can distinguish mutexes

#endif