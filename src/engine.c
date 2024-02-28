/**
 * @ Author: MMMM
 * @ Create Time: 2024-02-24 14:26:01
 * @ Modified time: 2024-02-28 10:44:09
 * @ Description:
 * 
 * This combines the different utility function and manages the relationships between them.
 * This file is only meant to be included once, thus the lack of inclusion guards.
 */

#include "./utils/utils.animation.h"
#include "./utils/utils.buffer.h"
#include "./utils/utils.event.h"
#include "./utils/utils.thread.h"
#include "./utils/utils.types.h"

#include "./animations.c"
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

  // Animations
  Animation *pAnimation_Intro;

  EventManager eventManager;    // Deals with events
  ThreadManager threadManager;  // Manages the different threads of the program
  
  // Events
  KeyEvents keyEvents;          // Deals with key events
  TimeEvents timeEvents;        // Deals with timer related events

  int bState;                   // The state of the engine

} Engine;

void Engine_init(Engine *this);

void Engine_main(p_obj pArgs_Engine, int tArg_NULL);

void Engine_exit(Engine *this);

/**
 * Initializes the engine.
 * 
 * @param   { Engine * }  this      The engine object.
*/
void Engine_init(Engine *this) {
  
  // The engine is currently running
  this->bState = 1;

  // Initialize the animations
  this->pAnimation_Intro = Animation_create(
    "intro-animation",
    AnimationHandler_intro, 
    13,  // 13 states
    'f', 0.0, 'f', 0.0, 'i', 0, 'i', 0,
    'f', 0.0, 'f', 0.0, 'i', 0, 'i', 0,
    'f', -100.0, 'f', 10.0, 'i', 24, 'i', 12,

    'i', 0x000000
  );

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

  // Exit the event manager first, since it relies on the threads
  EventManager_exit(&this->eventManager);

  // Exit the thread manager last
  ThreadManager_exit(&this->threadManager);

}

/**
 * The main thread of the engine.
 * 
 * @param   { p_obj * }   pArgs_Engine  The engine object.
 * @param   { int }       tArg_NULL     A dummy value.
*/
void Engine_main(p_obj pArgs_Engine, int tArg_NULL) {

  // Get the engine
  Engine *this = (Engine *) pArgs_Engine;

  Animation_update(this->pAnimation_Intro);

  // Because IO operations are expensive, we want to do them only when a change occurs
  if(!this->keyEvents.bHasBeenRead || 1) {
    Buffer *pBuffer = Buffer_create(
      IO_getWidth(), 
      IO_getHeight(),
      0xffffff,
      0x000000);

    char *block[5] = {
      "    __  ________   ______________       __________________  __________ ",
      "   /  |/  /  _/ | / / ____/ ___/ |     / / ____/ ____/ __ \\/ ____/ __ \\",
      "  / /|_/ // //  |/ / __/  \\__ \\| | /| / / __/ / __/ / /_/ / __/ / /_/ /",
      " / /  / // // /|  / /___ ___/ /| |/ |/ / /___/ /___/ ____/ /___/ _, _/ ",
      "/_/  /_/___/_/ |_/_____//____/ |__/|__/_____/_____/_/   /_____/_/ |_|  ",
    };

    Buffer_context(pBuffer, 
      this->pAnimation_Intro->dRoundStates[0], 
      this->pAnimation_Intro->dRoundStates[1],
      10, 5, -1, 0x0000dd);

    Buffer_context(pBuffer, 
      this->pAnimation_Intro->dRoundStates[2], 
      this->pAnimation_Intro->dRoundStates[3],
      10, 5, -1, 0xdd0000);
    Buffer_write(pBuffer, 
      this->pAnimation_Intro->dRoundStates[4], 
      this->pAnimation_Intro->dRoundStates[5], 
      strlen(block[0]), 5, block);
    Buffer_context(pBuffer, 0, 11 - this->pAnimation_Intro->dStates[6] / 2, IO_getWidth(), this->pAnimation_Intro->dStates[6] + 8, 0x000000, 0xffffff);

    IO_clear();
    Buffer_print(pBuffer);
    Buffer_kill(pBuffer);

    KeyEvents_read(&this->keyEvents);
  }

  if(this->keyEvents.cLatest == 'q')
    this->bState = 0;
}

/**
 * Returns the state of the engine.
 * Returns a 1 when the engine is currently running.
 * Returns a 0 when all its processes have exited.
 * 
 * @param   { Engine * }  this  The engine object.
 * @return  { int }             Whether or not the engine is still running.
*/
int Engine_getState(Engine *this) {
  return this->bState;
}