/**
 * @ Author: MMMM
 * @ Create Time: 2024-02-05 11:21:11
 * @ Modified time: 2024-03-02 23:29:45
 * @ Description:
 *    
 * Typedefs some custom types.   
 * We will be using lowercase letters and underscores to denote that something is a data type.
 */

#ifndef UTILS_TYPES_
#define UTILS_TYPES_

/**
 * //
 * ////
 * //////    Standard library types
 * ////////
 * ////////// 
*/

// Some primitives
// Gives us fixed-width data types
//    (1) uint8_t
//    (2) uint16_t
//    (3) uint32_t
//    (4) uint64_t
#include <inttypes.h>

/**
 * //
 * ////
 * //////    Custom type definitions
 * ////////
 * ////////// 
*/

// FOR THE CODE BELOW (parameter object pointers):
// Note that we have to do this because some of the Windows API requires us to pass
//    both a callback and its arguments separately. The data types here are primarily
//    for representing the arguments to a callback function.

// Parameter object pointers
typedef void *p;                                                  // A pointer to a collection of function parameters
typedef p p_obj;                                                  // A pointer to a struct to be passed to a function

typedef void (*f_void_callback)(p_obj pArgs, int tArg);           // A pointer to a callback function that returns void
                                                                  // Here, the following are:
                                                                  //    (1) pArgs   =>  The main object to pass into the callback
                                                                  //                    This is usually an instance of the class that called the callback
                                                                  //    (2) tArg    =>  An optional parameter that usually specifies an enum 

typedef void (*f_event_handler)(p_obj pArgs, p_obj pArgs2);       // Creates a template for event handlers
                                                                  // Here, the following are:
                                                                  //    (1) pArgs   =>  The event object to be handled
                                                                  //    (2) pArgs2  =>  An object we modify as a result of the event

typedef char (*f_event_listener)(void);                           // Creates a template for event listeners

typedef void (*f_animation_handler)(p_obj pArgs, p_obj pArgs2);   // Creates a template for animation handlers

typedef void (*f_page_configurer)(p_obj pArgs);                   // Creates a template for page configurers (they set up pages)

#endif