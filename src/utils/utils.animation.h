/**
 * @ Author: Mo David
 * @ Create Time: 2024-02-27 09:17:35
 * @ Modified time: 2024-02-27 10:30:30
 * @ Description:
 * 
 * An animation class.
 */

#ifndef UTILS_ANIMATION_
#define UTILS_ANIMATION_

#include "./utils.types.h"

#include <stdarg.h>
#include <stdlib.h>

#define ANIMATION_MAX_STATES 32

/**
 * A class that helps us create animations on the screen.
 * 
 * @class
*/
typedef struct Animation {

  unsigned long long dT;                // Tells us the current time state of the animation (not to be
                                        //    confused with differential calculus notation of small changes)

  f_animation_handler fHandler;         // The function to update the animation over time

  int dFloatStateCount;
  int dIntStateCount;

  int dStates[ANIMATION_MAX_STATES];    // The maximum number of integer states to be stored by the object
  float fStates[ANIMATION_MAX_STATES];  // The maximum number of float states to be stored by the object
  
} Animation;

/**
 * Allocates memory for an instance of the Animation class.
 * 
 * @return	{ Animation * }		A pointer to the allocated memory.
*/
Animation *Animation_new() {
  Animation *pAnimation = calloc(1, sizeof(*pAnimation));
  return pAnimation;
}

/**
 * Initializes an instance of the Animation class.
 * 
 * @param		{ Animation * }		        this	    A pointer to the instance to initialize.
 * @param   { f_animation_handler }   fHandler  The function to handle updating the animation over time.
 * @param   { int }                   dStates   How many states we're going to initialize at first.
 * @param   { va_list }   vArgs                 The type of states to initialize, as well as their initial values, stored in a vector.
 * @return	{ Animation * }					            A pointer to the initialized instance.
*/
Animation *Animation_init(Animation *this, f_animation_handler fHandler, int dStates, va_list vArgs) {
  int i;

  // Store the handler
  this->fHandler = fHandler;

  // Set the states to 0
  this->dT = 0ULL;
  this->dFloatStateCount = 0;
  this->dIntStateCount = 0;

  for(i = 0; i < dStates; i++) {
    char cType = va_arg(vArgs, int);

    // We're retrieving a float
    if(cType == 'f' && this->dFloatStateCount < ANIMATION_MAX_STATES)
      this->fStates[this->dFloatStateCount++] = (float) va_arg(vArgs, double);

    // We're retrieving an int
    if(cType == 'i' && this->dIntStateCount < ANIMATION_MAX_STATES)
      this->dStates[this->dIntStateCount++] = va_arg(vArgs, int);
  }

  return this;
}

/**
 * Creates an initialized instance of the Animation class.
 * 
 * @param   { f_animation_handler }   fHandler  The function to handle updating the animation over time.
 * @param   { int }                   dStates   How many states we're going to initialize at first.
 * @param   { (char, int & float) }   ...       The type of states to initialize, as well as their initial values.
 * @return	{ Animation * }		                  A pointer to the newly created initialized instance.
*/
Animation *Animation_create(f_animation_handler fHandler, int dStates, ...) {
  
  va_list vArgs;              // Create a pointer to the vector of arguments
  va_start(vArgs, dStates);   // Set the pointer to the first arg after dStates

  Animation *pAnimation = Animation_init(Animation_new(), fHandler, dStates, vArgs);
  va_end(vArgs);              // We need to call this for clean up

  return pAnimation;
}

/**
 * Deallocates the memory of an instance of the Animation class.
 * 
 * @param		{ Animation * }		this	A pointer to the instance to deallocate.
*/
void Animation_kill(Animation *this) {
  free(this);
}

/**
 * Updates an animation.
 * 
 * @param   { Animation * }   this  The animation to update.
*/
void Animation_update(Animation *this) {
  this->fHandler(this);
  this->dT++;
}


#endif