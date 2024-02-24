/**
 * @ Author: Mo David
 * @ Create Time: 2024-02-24 14:04:52
 * @ Modified time: 2024-02-24 23:45:42
 * @ Description:
 * 
 * Some basic event listeners for Windows.
 * Note that event listeners must call the createEvent() function inside of their implementations.
 * Also, they must return void on Windows and void * on Unix.
 */

#ifndef UTILS_EVENTS_WIN_
#define UTILS_EVENTS_WIN_

#include "../utils.io.h"
#include "../utils.event.h"
#include "../utils.types.h"

#include <stdio.h>

/**
 * Key pressed listener and handler
*/
void EventListener_keyPressed(p_obj pArgs);

void EventHandler_keyPressed(char cState);

/**
 * Listens for key presses.
 * 
 * @param   { p_obj }   pArgs   The event manager responsible for dispatching events in this context.
*/
void EventListener_keyPressed(p_obj pArgs) {
  EventManager *this = (EventManager *) pArgs;

  EventManager_createEvent(this, EVENT_KEY, EventHandler_keyPressed, IO_readChar());
}

/**
 * Handles key presses.
 * Basically, what happens after a key is pressed.
 * 
 * @param   { char }              cState        The input to the callback function.
*/
void EventHandler_keyPressed(char cState) {
  
  // ! make it be able to store this somewhere or idkk
  // ! give a way for the programmer to be able to add eventhandlers at runtime
  // ! maybe turn eventHandler into its own class?

  printf("%c", cState);
}


#endif

