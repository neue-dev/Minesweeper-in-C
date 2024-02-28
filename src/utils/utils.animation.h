/**
 * @ Author: Mo David
 * @ Create Time: 2024-02-27 09:17:35
 * @ Modified time: 2024-02-28 12:11:27
 * @ Description:
 * 
 * An animation class.
 */

#ifndef UTILS_ANIMATION_
#define UTILS_ANIMATION_

#include "./utils.hashmap.h"
#include "./utils.types.h"

#include <math.h>
#include <stdarg.h>
#include <stdlib.h>

#define ANIMATION_MAX_STATES 32
#define ANIMATION_MAX_COUNT 256

typedef struct Animation Animation;
typedef struct AnimationManager AnimationManager;

/**
 * //
 * ////
 * //////    Animation class
 * ////////
 * ////////// 
*/

/**
 * A class that helps us create animations on the screen.
 * 
 * @class
*/
struct Animation {

  char *sName;                              // An identifier for the animation object

  unsigned long long dT;                    // Tells us the current time state of the animation (not to be
                                            //    confused with differential calculus notation of small changes)

  f_animation_handler fHandler;             // The function to update the animation over time

  int dFloatStateCount;
  int dIntStateCount;

  int dStates[ANIMATION_MAX_STATES];        // The integer states to be stored by the object

  float fStates[ANIMATION_MAX_STATES];      // The float states to be stored by the object
  int dRoundStates[ANIMATION_MAX_STATES];   // The float states (rounded off) to be stored by the object
  
};

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
 * @param   { char * }                sName     An identifier for the animation object.
 * @param   { f_animation_handler }   fHandler  The function to handle updating the animation over time.
 * @param   { int }                   dStates   How many states we're going to initialize at first.
 * @param   { va_list }   vArgs                 The type of states to initialize, as well as their initial values, stored in a vector.
 * @return	{ Animation * }					            A pointer to the initialized instance.
*/
Animation *Animation_init(Animation *this, char *sName, f_animation_handler fHandler, int dStates, va_list vArgs) {
  int i;

  // Store the name
  this->sName = sName;

  // Store the handler
  this->fHandler = fHandler;

  // Set the states to 0
  this->dT = 0ULL;
  this->dFloatStateCount = 0;
  this->dIntStateCount = 0;

  for(i = 0; i < dStates; i++) {
    char cType = va_arg(vArgs, int);

    // We're retrieving a float
    if(cType == 'f' && this->dFloatStateCount < ANIMATION_MAX_STATES) {
      float fState = (float) va_arg(vArgs, double);

      this->dRoundStates[this->dFloatStateCount] = round(fState);
      this->fStates[this->dFloatStateCount++] = fState;
    }

    // We're retrieving an int
    if(cType == 'i' && this->dIntStateCount < ANIMATION_MAX_STATES) {
      this->dStates[this->dIntStateCount++] = va_arg(vArgs, int);
    }
  }

  return this;
}

/**
 * Creates an initialized instance of the Animation class.
 * 
 * @param   { f_animation_handler }   fHandler  The function to handle updating the animation over time.
 * @param   { char * }                sName     An identifier for the animation object.
 * @param   { int }                   dStates   How many states we're going to initialize at first.
 * @param   { (char, int & float) }   ...       The type of states to initialize, as well as their initial values.
 * @return	{ Animation * }		                  A pointer to the newly created initialized instance.
*/
Animation *Animation_create(char *sName, f_animation_handler fHandler, int dStates, ...) {
  
  va_list vArgs;              // Create a pointer to the vector of arguments
  va_start(vArgs, dStates);   // Set the pointer to the first arg after dStates

  Animation *pAnimation = Animation_init(Animation_new(), sName, fHandler, dStates, vArgs);
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
  int i;

  // Execute the callback
  this->fHandler(this);

  // Perform some fixins
  for(i = 0; i < this->dFloatStateCount; i++)
    this->dRoundStates[i] = round(this->fStates[i]);
    
  // Increment time state
  this->dT++;
}

/**
 * //
 * ////
 * //////    AnimationManager struct
 * ////////
 * ////////// 
*/

/**
 * Handles all our APIs for creating and doing stuff with animations.
 * 
 * @struct
*/
struct AnimationManager {
  Animation *pAnimationArray[ANIMATION_MAX_COUNT];  // All the animations we'll ever need
};

void AnimationManager_createAnimation() {
  
}

#endif