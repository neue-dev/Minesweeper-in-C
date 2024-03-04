/**
 * @ Author: MMMM
 * @ Create Time: 2024-02-27 09:17:35
 * @ Modified time: 2024-03-04 15:43:26
 * @ Description:
 * 
 * A runner class. 
 * This class not only handles animations but also anything related to updating the page class.
 */

#ifndef UTILS_RUNNER_
#define UTILS_RUNNER_

#include "./utils.hashmap.h"
#include "./utils.types.h"

#include <math.h>
#include <stdarg.h>
#include <stdlib.h>

// Useful for passing arguments into runner constructors
#define RUNNER_FLOAT_STATE 'f'
#define RUNNER_INT_STATE 'i'

#define RUNNER_MAX_STATES (1 << 8)
#define RUNNER_MAX_COUNT (1 << 8)

typedef enum RunnerState RunnerState;

typedef struct Runner Runner;
typedef struct RunnerManager RunnerManager;

enum RunnerState {
  RUNNER_INIT,     // The runner is currently initializing
  RUNNER_RUNNING,  // The runner is running
  RUNNER_EXIT,     // The runner has finished running
};

/**
 * //
 * ////
 * //////    Runner class
 * ////////
 * ////////// 
*/

/**
 * A class that helps us create runners on the screen.
 * 
 * @class
*/
struct Runner {

  char *sName;                              // An identifier for the runner object

  unsigned long long dT;                    // Tells us the current time state of the runner (not to be
                                            //    confused with differential calculus notation of small changes)
  unsigned int dStage;                      // What stage of the runner we're in
  RunnerState eRunnerState;                 // This tells us what state the runner is currently in

  f_runner_handler fHandler;                // The function to update the runner over time
  p_obj pArgs2_ANY;                         // The argument to the handler

  int dFloatStateCount;
  int dIntStateCount;

  int dStates[RUNNER_MAX_STATES];           // The integer states to be stored by the object
  float fStates[RUNNER_MAX_STATES];         // The float states to be stored by the object
  int dRoundStates[RUNNER_MAX_STATES];      // The float states (rounded off) to be stored by the object
  
};

/**
 * Allocates memory for an instance of the Runner class.
 * 
 * @return	{ Runner * }		A pointer to the allocated memory.
*/
Runner *Runner_new() {
  Runner *pRunner = calloc(1, sizeof(*pRunner));
  return pRunner;
}

/**
 * Initializes an instance of the Runner class.
 * 
 * @param		{ Runner * }		        this	      A pointer to the instance to initialize.
 * @param   { char * }              sName       An identifier for the runner object.
 * @param   { f_runner_handler }    fHandler    The function to handle updating the runner over time.
 * @param   { p_obj }               pArgs2_ANY  The input to the callback.
 * @param   { int }                 dStates     How many states we're going to initialize at first.
 * @param   { va_list }             vArgs       The type of states to initialize, as well as their initial values, stored in a vector.
 * @return	{ Runner * }					              A pointer to the initialized instance.
*/
Runner *Runner_init(Runner *this, char *sName, f_runner_handler fHandler, p_obj pArgs2_ANY, int dStates, va_list vArgs) {
  int i;
  char cType;
  float fState;

  // Store the name
  this->sName = sName;

  // For the first runner frame
  this->eRunnerState = RUNNER_INIT;

  // Store the handler
  this->fHandler = fHandler;
  this->pArgs2_ANY = pArgs2_ANY;

  // Set the states to 0
  this->dT = 0ULL;
  this->dStage = 0;
  this->dFloatStateCount = 0;
  this->dIntStateCount = 0;

  // Just so they dont have garbage values
  for(i = 0; i < RUNNER_MAX_STATES; i++) {
    this->dStates[i] = 0;
    this->fStates[i] = 0.0;

    this->dRoundStates[i] = 0;
  }

  for(i = 0; i < dStates; i++) {
    cType = va_arg(vArgs, int);

    // We're retrieving a float
    if(cType == RUNNER_FLOAT_STATE && this->dFloatStateCount < RUNNER_MAX_STATES) {
      fState = (float) va_arg(vArgs, double);

      this->dRoundStates[this->dFloatStateCount] = round(fState);
      this->fStates[this->dFloatStateCount++] = fState;
    }

    // We're retrieving an int
    if(cType == RUNNER_INT_STATE && this->dIntStateCount < RUNNER_MAX_STATES) {
      this->dStates[this->dIntStateCount++] = va_arg(vArgs, int);
    }
  }

  return this;
}

/**
 * Creates an initialized instance of the Runner class.
 * 
 * @param   { f_runner_handler }    fHandler    The function to handle updating the runner over time.
 * @param   { char * }              sName       An identifier for the runner object.
 * @param   { p_obj }               pArgs2_ANY  The input to the callback.
 * @param   { int }                 dStates     How many states we're going to initialize at first.
 * @param   { va_list }             vArgs       The type of states to initialize, as well as their initial values, stored in a vector.
 * @return	{ Runner * }		                    A pointer to the newly created initialized instance.
*/
Runner *Runner_create(char *sName, f_runner_handler fHandler, p_obj pArgs2_ANY, int dStates, va_list vArgs) {
  return Runner_init(Runner_new(), sName, fHandler, pArgs2_ANY, dStates, vArgs);
}

/**
 * Deallocates the memory of an instance of the Runner class.
 * 
 * @param		{ Runner * }		this	A pointer to the instance to deallocate.
*/
void Runner_kill(Runner *this) {
  free(this);
}

/**
 * Updates an runner.
 * 
 * @param   { Runner * }   this  The runner to update.
*/
void Runner_update(Runner *this) {
  int i;

  // Execute the callback
  this->fHandler(this, this->pArgs2_ANY);

  // If the runner has exited
  if(this->eRunnerState == RUNNER_EXIT)
    return;

  // Perform some fixins
  for(i = 0; i < this->dFloatStateCount; i++)
    this->dRoundStates[i] = round(this->fStates[i]);

  // The first frame has been executed
  if(this->eRunnerState == RUNNER_INIT)
    this->eRunnerState = RUNNER_RUNNING;
    
  // Increment time state
  this->dT++;
}

/**
 * //
 * ////
 * //////    RunnerManager struct
 * ////////
 * ////////// 
*/

/**
 * Handles all our APIs for creating and doing stuff with runners.
 * 
 * @struct
*/
struct RunnerManager {

  HashMap *pRunnerMap;                            // A hashmap to store all our runners
  char *sRunnerKeys[RUNNER_MAX_COUNT];            // The keys that reference our runners
  int dRunnerCount;                               // How many runners we have

};

/**
 * Initializes the RunnerManager struct.
 * 
 * @param   { RunnerManager * }  this  The runner manager.
*/
void RunnerManager_init(RunnerManager *this) {
  int i;

  this->pRunnerMap = HashMap_create();
  this->dRunnerCount = 0;

  // Some set up
  for(i = 0; i < RUNNER_MAX_COUNT; i++)
    this->sRunnerKeys[i] = NULL;
}

/**
 * Cleans up the RunnerManager struct.
 * 
 * @param   { RunnerManager * }  this  The runner manager.
*/
void RunnerManager_exit(RunnerManager *this) {
  // ! do this
}

/**
 * Creates an runner and stores it in the hashmap.
 * 
 * @param   { RunnerManager * }       this        The runner manager.
 * @param   { char * }                sKey        An identifier for the runner.
 * @param   { f_runner_handler }      fHandler    The callback of the runner.
 * @param   { p_obj }                 pArgs2_ANY  The input to the callback.
 * @param   { int }                   dStates     How many states we want to initialize.
 * @param   { (char, int & float) }   ...         The type of states to initialize, as well as their initial values.
*/
void RunnerManager_createRunner(RunnerManager *this, char *sKey, f_runner_handler fHandler, p_obj pArgs2_ANY, int dStates, ...) {
  int i;
  Runner *pRunner = (Runner *) HashMap_get(this->pRunnerMap, sKey);

  // Duplicate key
  if(pRunner != NULL)
    return;

  // Can't have too many runners
  if(this->dRunnerCount >= RUNNER_MAX_COUNT)
    return;
    
  // Create a pointer to the vector of arguments
  // Set the pointer to the first arg after dStates
  va_list vArgs;
  va_start(vArgs, dStates);

  // Create the runner and store it in the hashmap
  pRunner = Runner_create(sKey, fHandler, pArgs2_ANY, dStates, vArgs);
  HashMap_add(this->pRunnerMap, sKey, pRunner);

  // We need to call this for clean up
  va_end(vArgs);

  // Increment
  this->dRunnerCount++;

  // Recompute the keys
  for(i = 0; i < this->dRunnerCount; i++)
    if(this->sRunnerKeys[i] != NULL)
      String_kill(this->sRunnerKeys[i]); 

  // Sadly, we'll be using this function a lot  
  HashMap_getKeys(this->pRunnerMap, this->sRunnerKeys);
}

/**
 * Updates an runner with the given key.
 * 
 * @param   { RunnerManager * }   this      The runner manager.
 * @param   { char * }            sKey      An identifier for the runner.
*/
void RunnerManager_updateRunner(RunnerManager *this, char *sKey) {
  Runner *pRunner = (Runner *) HashMap_get(this->pRunnerMap, sKey);

  if(pRunner != NULL)
    Runner_update(pRunner);
}

/**
 * Updates all the runners.
 * 
 * @param   { RunnerManager * }  this  The runner manager.
*/
void RunnerManager_updateAll(RunnerManager *this) {
  int i;
  Runner *pRunner;

  // Get the key for each runner and then update the corresponding anim
  for(i = 0; i < this->dRunnerCount; i++) {
    pRunner = (Runner *) HashMap_get(this->pRunnerMap, this->sRunnerKeys[i]);
    
    if(pRunner != NULL)
      Runner_update(pRunner);
  }
}

/**
 * Deletes an runner with the given key.
 * 
 * @param   { RunnerManager * }   this      The runner manager.
 * @param   { char * }            sKey      An identifier for the runner.
*/
void RunnerManager_killRunner(RunnerManager *this, char *sKey) {
  Runner *pRunner = (Runner *) HashMap_get(this->pRunnerMap, sKey);

  // We should only kill the runner if it's not NULL
  if(pRunner != NULL) {
    Runner_kill(pRunner);
    this->dRunnerCount--;
  }
}

#endif