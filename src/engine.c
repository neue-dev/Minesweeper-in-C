/**
 * @ Author: MMMM
 * @ Create Time: 2024-02-24 14:26:01
 * @ Modified time: 2024-02-25 15:06:31
 * @ Description:
 * 
 * This combines the different utility function and manages the relationships between them.
 * This file is only meant to be included once, thus the lack of inclusion guards.
 */

#include "./utils/utils.graphics.h"
#include "./utils/utils.buffer.h"
#include "./utils/utils.event.h"
#include "./utils/utils.thread.h"
#include "./utils/utils.types.h"

#include "./events.c"

// Some definitions for identifiers
#define ENGINE_EVENT_LISTENERS "engine-event-listeners"
#define ENGINE_EVENT_LISTENERS_MUTEX "engine-events-listeners-mutex"
#define ENGINE_EVENT_LISTENERS_THREAD "engine-event-listeners-thread"

#define ENGINE_EVENT_HANDLERS "engine-event-handlers"
#define ENGINE_EVENT_HANDLERS_MUTEX "engine-events-handlers-mutex"
#define ENGINE_EVENT_HANDLERS_THREAD "engine-event-handlers-thread"

#define ENGINE_MAIN "engine-main"
#define ENGINE_MAIN_THREAD "engine-main-thread"

/**
 * The engine struct handles the interactions between the different utility libraries.
 * It only deals with the libraries that have backend functionality.
 * 
 * @struct 
*/
typedef struct Engine {

  EventManager eventManager;    // Deals with events
  ThreadManager threadManager;  // Manages the different threads of the program
  
  KeyEvents keyEvents;          // Deals with key events
  TimeEvents timeEvents;        // Deals with timer related events

  int dummy; // ! remov
  int dWidth;                   // The width of the console
  int dHeight;                  // The height of the console

} Engine;

void Engine_init(Engine *this, int dWidth, int dHeight);

void Engine_main(p_obj pArgs_Engine, int tArg_NULL);

void Engine_exit(Engine *this);

/**
 * Initializes the engine.
 * 
 * @param   { Engine * }  this      The engine object.
 * @param   { int }       dWidth    The width of the console.
 * @param   { int }       dHeight   The height of the console.
*/
void Engine_init(Engine *this, int dWidth, int dHeight) {

  this->dummy = 0;  // !remove
  this->dWidth = dWidth;
  this->dHeight = dHeight;

  // Initialize user-defined state manager
  KeyEvents_init(&this->keyEvents);

  // Initialize the managers
  EventManager_init(&this->eventManager, &this->keyEvents);
  ThreadManager_init(&this->threadManager);

  // Create event listeners and handlers
  EventManager_createEventListener(&this->eventManager, EVENT_KEY, EventListener_keyPressed);
  EventManager_createEventHandler(&this->eventManager, EVENT_KEY, EventHandler_keyPressed);

  // Create a mutex for event listeners and event handlers
  ThreadManager_createMutex(&this->threadManager, ENGINE_EVENT_LISTENERS_MUTEX);              
  ThreadManager_createMutex(&this->threadManager, ENGINE_EVENT_HANDLERS_MUTEX);              

  // Create a thread for the event listeners
  ThreadManager_createThread(
    &this->threadManager, 
    
    ENGINE_EVENT_LISTENERS_THREAD,                // The name of the thread
    ENGINE_EVENT_LISTENERS_MUTEX,                 // The name of the mutex
    
    EventManager_triggerEvent,                    // The routine that triggers events
    &this->eventManager,                          // The event manager
    EVENT_KEY);                                   // What type of event the thread triggers

  // Create a thread for the event handlers
  ThreadManager_createThread(
    &this->threadManager,

    ENGINE_EVENT_HANDLERS_THREAD,
    ENGINE_EVENT_HANDLERS_MUTEX,

    EventManager_resolveEvent,                    // The routine that resolves events
    &this->eventManager,                          // The event manager
    0);                                           // A dummy value

  // Create a thread for the main thread of the program
  ThreadManager_createThread(
    &this->threadManager,
    ENGINE_MAIN_THREAD,                           // The main thread
    ENGINE_EVENT_HANDLERS_MUTEX,                  // It will share a mutex with the event handlers to prevent it from
                                                  //    reading the shared state while the handlers are changing this.

    Engine_main,                                  // The main routine
    this,                                         // The engine itself
    0);                                           // A dummy value
}

/**
 * Do some clean up after the entire program runs.
 * Frees whatever was allocated.
 * 
 * @param   { Engine * }  this  The engine object.
*/
void Engine_exit(Engine *this) {

  ThreadManager_exit(&this->threadManager);
  // ! make sure to exit the event manager too

}

/**
 * The main thread of the engine.
 * 
 * @param   { p_obj * }   pArgs_Engine  The engine object.
 * @param   { int }       tArg_NULL     A dummy value.
*/
void Engine_main(p_obj pArgs_Engine, int tArg_NULL) {
  int i;

  // Get the engine
  Engine *this = (Engine *) pArgs_Engine;

  // Create a buffer
  Buffer *pBuffer = Buffer_create(this->dWidth, this->dHeight);
  
  char *s = calloc(64, sizeof(char));
  char *s2 = calloc(64, sizeof(char));

  if(!this->keyEvents.bHasBeenRead) {
    KeyEvents_read(&this->keyEvents);
  }

  for(i = 0; i < this->keyEvents.dHistoryLength; i++)
    s[i] = this->keyEvents.cHistory[i];
    
  Buffer_append(pBuffer, s);

  sprintf(s2, "%d", this->dummy % (1 << 24));
  Buffer_append(pBuffer, "%s%s", 
    Graphics_getCodeFG(0xffffff), 
    Graphics_getCodeBG(this->dummy % (1 << 24)));

  int speed = 5;

  this->dummy += speed;
  this->dummy += (this->dummy >> 8) ? (1 << 8) * speed : 0;
  this->dummy += (this->dummy >> 16) ? (1 << 16) * speed : 0;

  if(this->dummy >= (1 << 24)) 
    this->dummy = 0;

  IO_clear();
  Buffer_print(pBuffer);  
}