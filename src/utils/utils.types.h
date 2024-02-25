/**
 * @ Author: MMMM
 * @ Create Time: 2024-02-05 11:21:11
 * @ Modified time: 2024-02-25 09:47:33
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
typedef void *p;                                          // A pointer to a collection of function parameters
typedef p p_obj;                                          // A pointer to a struct to be passed to a function

typedef void (*f_void_callback)(void *pArgs, int tArg);   // A pointer to a callback function that returns void
typedef void (*f_event_handler)(void *pArgs);             // Creates a template for event handlers
typedef char (*f_event_listener)(void);                   // Creates a template for event listeners

#endif