/**
 * @ Author: Mo David
 * @ Create Time: 2024-02-24 14:26:01
 * @ Modified time: 2024-02-25 08:23:59
 * @ Description:
 * 
 * This combines the different utility function and manages the relationships between them.
 * This file is only meant to be included once, thus the lack of inclusion guards.
 */

#include "./utils/utils.event.h"
#include "./utils/utils.thread.h"
#include "./utils/utils.types.h"

#define ENGINE_KEY_EVENTS "engine-key-events"
#define ENGINE_KEY_EVENTS_MUTEX "engine-key-events-mutex"
#define ENGINE_KEY_EVENTS_THREAD "engine-key-events-thread"

/**
 * The engine struct handles the interactions between the different utility libraries.
 * It only deals with the libraries that have backend functionality.
 * 
 * @struct 
*/
typedef struct Engine {

  EventManager eventManager;    // Deals with events
  ThreadManager threadManager;  // Manages the different threads of the program
  
} Engine;

/**
 * ! move this functionality over to the events file (implement it for windows and unix)
*/
int Engine_keyPressed(p_obj pArgs) {
  return 1;
}

/**
 * Initializes the engine.
 * 
 * @param   { Engine * }  this  The engine object.
*/
void Engine_init(Engine *this) {

  // Initialize the managers
  EventManager_init(&this->eventManager);
  ThreadManager_init(&this->threadManager);

  // Create a mutex
  ThreadManager_createMutex(&this->threadManager, 
    ENGINE_KEY_EVENTS_MUTEX);             // The name of the mutex

  // Create a thread for the events
  ThreadManager_createThread(&this->threadManager, 
    ENGINE_KEY_EVENTS_THREAD,             // The name of the thread
    ENGINE_KEY_EVENTS_MUTEX,              // The name of the mutex
    
    EventListener_keyPressed,             // The callback to be executed by the thread
    &this->eventManager);                 // An input to that callback
}

/**
 * Executes the engine.
 * 
 * @param   { Engine * }  this  The engine object.
*/
void Engine_run(Engine* this) {
  // ! WHEN RESOLVING EVENTS, MAKE SURE TO LOCK THE MUTEX TO THE EVENTS FIRST

  while(1) {
    printf("%d", this->eventManager.dEventCount);
  }
}

/**
 * Do some clean up after the entire program runs.
 * Frees whatever was allocated.
 * 
 * @param   { Engine * }  this  The engine object.
*/
void Engine_exit(Engine *this) {

  ThreadManager_exit(&this->threadManager);

}