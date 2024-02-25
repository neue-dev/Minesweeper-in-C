/**
 * @ Author: Mo David
 * @ Create Time: 2024-02-24 14:26:01
 * @ Modified time: 2024-02-25 09:57:48
 * @ Description:
 * 
 * This combines the different utility function and manages the relationships between them.
 * This file is only meant to be included once, thus the lack of inclusion guards.
 */

#include "./utils/utils.event.h"
#include "./utils/utils.thread.h"
#include "./utils/utils.types.h"

#define ENGINE_EVENT_LISTENERS "engine-event-listeners"
#define ENGINE_EVENT_LISTENERS_MUTEX "engine-events-listeners-mutex"
#define ENGINE_EVENT_LISTENERS_THREAD "engine-event-listeners-thread"

#define ENGINE_EVENT_HANDLERS "engine-event-handlers"
#define ENGINE_EVENT_HANDLERS_MUTEX "engine-events-handlers-mutex"
#define ENGINE_EVENT_HANDLERS_THREAD "engine-event-handlers-thread"

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
 * Initializes the engine.
 * 
 * @param   { Engine * }  this  The engine object.
*/
void Engine_init(Engine *this) {

  // Initialize the managers
  EventManager_init(&this->eventManager);
  ThreadManager_init(&this->threadManager);

  // Create event handlers
  EventManager_createEventListener(&this->eventManager, EVENT_KEY, EventListener_keyPressed);
  EventManager_createEventHandler(&this->eventManager, EVENT_KEY, EventHandler_keyPressed);
  EventManager_createEventHandler(&this->eventManager, EVENT_KEY, EventHandler_keyPressed2);

  // Create a mutex for event listeners and event handlers
  ThreadManager_createMutex(&this->threadManager, ENGINE_EVENT_LISTENERS_MUTEX);              
  ThreadManager_createMutex(&this->threadManager, ENGINE_EVENT_HANDLERS_MUTEX);              

  // Create a thread for the event listeners
  ThreadManager_createThread(&this->threadManager, 
    ENGINE_EVENT_LISTENERS_THREAD,              // The name of the thread
    ENGINE_EVENT_LISTENERS_MUTEX,               // The name of the mutex
    
    EventManager_triggerEvent,                  // The callback to be executed by the thread
    &this->eventManager, EVENT_KEY);            // An input to that callback

  // Create a thread for the event handlers
  ThreadManager_createThread(&this->threadManager,
    ENGINE_EVENT_HANDLERS_THREAD,
    ENGINE_EVENT_HANDLERS_MUTEX,

    EventManager_resolveEvent,
    &this->eventManager, 0);
}

/**
 * Executes the engine.
 * 
 * @param   { Engine * }  this  The engine object.
*/
void Engine_run(Engine* this) {
  // ! WHEN RESOLVING EVENTS, MAKE SURE TO LOCK THE MUTEX TO THE EVENTS FIRST

  while(1) {
    // printf("%d", this->eventManager.dEventCount);
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