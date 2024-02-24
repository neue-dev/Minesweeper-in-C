/**
 * @ Author: MMMM
 * @ Create Time: 2024-02-05 11:21:11
 * @ Modified time: 2024-02-24 13:59:32
 * @ Description:
 *    
 * Typedefs some custom types.   
 * We will be using lowercase letters and underscores to denote that something is a data type.
 */

#ifndef UTILS_TYPES_
#define UTILS_TYPES_

// Some primitives
// Gives us fixed-width data types
//    (1) uint8_t
//    (2) uint16_t
//    (3) uint32_t
#include <stdint.h>

// FOR THE CODE BELOW (parameter object pointers):
// Note that we have to do this because some of the Windows API requires us to pass
//    both a callback and its arguments separately. The data types here are primarily
//    for representing the arguments to a callback function.

// Parameter object pointers
typedef void *p;                            // A pointer to a collection of function parameters
typedef p p_obj;                            // A pointer to a struct to be passed to a function
typedef void (*p_void_func)(void *pArgs);   // A pointer to a callback function that returns void
typedef int (*p_bool_func)(void *pArgs);    // A pointer to a callback function that returns a bool

#endif