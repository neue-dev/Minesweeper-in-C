/**
 * @ Author: Mo David
 * @ Create Time: 2024-02-24 14:26:01
 * @ Modified time: 2024-02-24 22:49:50
 * @ Description:
 * 
 * This combines the different utility function and manages the relationships between them.
 * This file is only meant to be included once, thus the lack of inclusion guards.
 */

#include "./utils/utils.event.h"
#include "./utils/utils.thread.h"
#include "./utils/utils.types.h"

#define ENGINE_EVENT_KEY_NAME "engine-event-key"
#define ENGINE_EVENT_KEY_MUTEX "engine-event-key-mutex"
#define ENGINE_EVENT_KEY_THREAD "engine-event-key-thread"

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

  // Create an event for key events
  // ! replace Engine_keyPressed with something from the events.h header
  // Event_init(&this->eventKey, ENGINE_EVENT_KEY_NAME, Engine_keyPressed);

  // Initialize the thread manager first
  ThreadManager_init(&this->threadManager);

  // Create a mutex and a thread for events
  ThreadManager_createMutex(&this->threadManager, ENGINE_EVENT_KEY_MUTEX);
  ThreadManager_createThread(&this->threadManager, 
    ENGINE_EVENT_KEY_THREAD, 
    ENGINE_EVENT_KEY_MUTEX, NULL, NULL);
}

/**
 * Executes the engine.
 * 
 * @param   { Engine * }  this  The engine object.
*/
void Engine_run(Engine* this) {
  
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