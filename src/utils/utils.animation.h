/**
 * @ Author: MMMM
 * @ Create Time: 2024-02-27 09:17:35
 * @ Modified time: 2024-03-03 11:34:26
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

// Useful for passing arguments into animation constructors
#define ANIMATION_FLOAT_STATE 'f'
#define ANIMATION_INT_STATE 'i'

#define ANIMATION_MAX_STATES (1 << 8)
#define ANIMATION_MAX_COUNT (1 << 8)

typedef enum AnimationState AnimationState;

typedef struct Animation Animation;
typedef struct AnimationManager AnimationManager;

enum AnimationState {
  ANIMATION_INIT,     // The animation is currently initializing
  ANIMATION_RUNNING,  // The animation is running
  ANIMATION_EXIT,     // The animation has finished running
};

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
  AnimationState eAnimationState;           // This tells us what state the animation is currently in

  f_animation_handler fHandler;             // The function to update the animation over time
  p_obj pArgs2_ANY;                         // The argument to the handler

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
 * @param		{ Animation * }		        this	      A pointer to the instance to initialize.
 * @param   { char * }                sName       An identifier for the animation object.
 * @param   { f_animation_handler }   fHandler    The function to handle updating the animation over time.
 * @param   { p_obj }                 pArgs2_ANY  The input to the callback.
 * @param   { int }                   dStates     How many states we're going to initialize at first.
 * @param   { va_list }               vArgs       The type of states to initialize, as well as their initial values, stored in a vector.
 * @return	{ Animation * }					              A pointer to the initialized instance.
*/
Animation *Animation_init(Animation *this, char *sName, f_animation_handler fHandler, p_obj pArgs2_ANY, int dStates, va_list vArgs) {
  int i;
  char cType;
  float fState;

  // Store the name
  this->sName = sName;

  // For the first animation frame
  this->eAnimationState = ANIMATION_INIT;

  // Store the handler
  this->fHandler = fHandler;
  this->pArgs2_ANY = pArgs2_ANY;

  // Set the states to 0
  this->dT = 0ULL;
  this->dFloatStateCount = 0;
  this->dIntStateCount = 0;

  // Just so they dont have garbage values
  for(i = 0; i < ANIMATION_MAX_STATES; i++) {
    this->dStates[i] = 0;
    this->fStates[i] = 0.0;

    this->dRoundStates[i] = 0;
  }

  for(i = 0; i < dStates; i++) {
    cType = va_arg(vArgs, int);

    // We're retrieving a float
    if(cType == ANIMATION_FLOAT_STATE && this->dFloatStateCount < ANIMATION_MAX_STATES) {
      fState = (float) va_arg(vArgs, double);

      this->dRoundStates[this->dFloatStateCount] = round(fState);
      this->fStates[this->dFloatStateCount++] = fState;
    }

    // We're retrieving an int
    if(cType == ANIMATION_INT_STATE && this->dIntStateCount < ANIMATION_MAX_STATES) {
      this->dStates[this->dIntStateCount++] = va_arg(vArgs, int);
    }
  }

  return this;
}

/**
 * Creates an initialized instance of the Animation class.
 * 
 * @param   { f_animation_handler }   fHandler    The function to handle updating the animation over time.
 * @param   { char * }                sName       An identifier for the animation object.
 * @param   { p_obj }                 pArgs2_ANY  The input to the callback.
 * @param   { int }                   dStates     How many states we're going to initialize at first.
 * @param   { va_list }               vArgs       The type of states to initialize, as well as their initial values, stored in a vector.
 * @return	{ Animation * }		                    A pointer to the newly created initialized instance.
*/
Animation *Animation_create(char *sName, f_animation_handler fHandler, p_obj pArgs2_ANY, int dStates, va_list vArgs) {
  return Animation_init(Animation_new(), sName, fHandler, pArgs2_ANY, dStates, vArgs);
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
  this->fHandler(this, this->pArgs2_ANY);

  // If the animation has exited
  if(this->eAnimationState == ANIMATION_EXIT)
    return;

  // Perform some fixins
  for(i = 0; i < this->dFloatStateCount; i++)
    this->dRoundStates[i] = round(this->fStates[i]);

  // The first frame has been executed
  if(this->eAnimationState == ANIMATION_INIT)
    this->eAnimationState = ANIMATION_RUNNING;
    
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

  HashMap *pAnimationMap;                            // A hashmap to store all our animations
  char *sAnimationKeys[ANIMATION_MAX_COUNT];         // The keys that reference our animations
  int dAnimationCount;                               // How many animations we have

};

/**
 * Initializes the AnimationManager struct.
 * 
 * @param   { AnimationManager * }  this  The animation manager.
*/
void AnimationManager_init(AnimationManager *this) {
  int i;

  this->pAnimationMap = HashMap_create();
  this->dAnimationCount = 0;

  // Some set up
  for(i = 0; i < ANIMATION_MAX_COUNT; i++)
    this->sAnimationKeys[i] = NULL;
}

/**
 * Cleans up the AnimationManager struct.
 * 
 * @param   { AnimationManager * }  this  The animation manager.
*/
void AnimationManager_exit(AnimationManager *this) {
  // ! do this
}

/**
 * Creates an animation and stores it in the hashmap.
 * 
 * @param   { AnimationManager * }    this        The animation manager.
 * @param   { char * }                sKey        An identifier for the animation.
 * @param   { f_animation_handler }   fHandler    The callback of the animation.
 * @param   { p_obj }                 pArgs2_ANY  The input to the callback.
 * @param   { int }                   dStates     How many states we want to initialize.
 * @param   { (char, int & float) }   ...         The type of states to initialize, as well as their initial values.
*/
void AnimationManager_createAnimation(AnimationManager *this, char *sKey, f_animation_handler fHandler, p_obj pArgs2_ANY, int dStates, ...) {
  int i;
  Animation *pAnimation = (Animation *) HashMap_get(this->pAnimationMap, sKey);

  // Duplicate key
  if(pAnimation != NULL)
    return;

  // Can't have too many animations
  if(this->dAnimationCount >= ANIMATION_MAX_COUNT)
    return;
    
  // Create a pointer to the vector of arguments
  // Set the pointer to the first arg after dStates
  va_list vArgs;
  va_start(vArgs, dStates);

  // Create the animation and store it in the hashmap
  pAnimation = Animation_create(sKey, fHandler, pArgs2_ANY, dStates, vArgs);
  HashMap_add(this->pAnimationMap, sKey, pAnimation);

  // We need to call this for clean up
  va_end(vArgs);

  // Increment
  this->dAnimationCount++;

  // Recompute the keys
  for(i = 0; i < this->dAnimationCount; i++)
    if(this->sAnimationKeys[i] != NULL)
      String_kill(this->sAnimationKeys[i]); 

  // Sadly, we'll be using this function a lot  
  HashMap_getKeys(this->pAnimationMap, this->sAnimationKeys);
}

/**
 * Updates an animation with the given key.
 * 
 * @param   { AnimationManager * }    this      The animation manager.
 * @param   { char * }                sKey      An identifier for the animation.
*/
void AnimationManager_updateAnimation(AnimationManager *this, char *sKey) {
  Animation *pAnimation = (Animation *) HashMap_get(this->pAnimationMap, sKey);

  if(pAnimation != NULL)
    Animation_update(pAnimation);
}

/**
 * Updates all the animations.
 * 
 * @param   { AnimationManager * }  this  The animation manager.
*/
void AnimationManager_updateAll(AnimationManager *this) {
  int i;
  Animation *pAnimation;

  // Get the key for each animation and then update the corresponding anim
  for(i = 0; i < this->dAnimationCount; i++) {
    pAnimation = (Animation *) HashMap_get(this->pAnimationMap, this->sAnimationKeys[i]);
    
    if(pAnimation != NULL)
      Animation_update(pAnimation);
  }
}

/**
 * Deletes an animation with the given key.
 * 
 * @param   { AnimationManager * }    this      The animation manager.
 * @param   { char * }                sKey      An identifier for the animation.
*/
void AnimationManager_killAnimation(AnimationManager *this, char *sKey) {
  Animation *pAnimation = (Animation *) HashMap_get(this->pAnimationMap, sKey);

  // We should only kill the animation if it's not NULL
  if(pAnimation != NULL) {
    Animation_kill(pAnimation);
    this->dAnimationCount--;
  }
}

#endif