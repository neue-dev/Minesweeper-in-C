/**
 * @ Author: Mo David
 * @ Create Time: 2024-02-24 14:04:52
 * @ Modified time: 2024-02-24 23:29:08
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

#include <stdio.h>

typedef void (*p_event_handler)(char);  // Creates a template for event handlers

/**
 * Listens for key presses.
 * 
 * @param   { p_obj }   pArgs   The event manager responsible for dispatching events in this context.
*/
void EventListener_keyPressed(p_obj pArgs) {
  EventManager *this = (EventManager *) pArgs;

  EventManager_createEvent(this, EVENT_KEY, IO_readChar());
}

/**
 * Handles key presses.
 * Honestly, this is just a wrapper function for event handlers.
 * This just guides the progammer using this library on how to go about 
 *    implementing event handlers. This function is not needed for the library
 *    to work, but makes it easier to understand how everything else ties
 *    together.
 * 
 * @param   { p_event_handler }   eventHandler  A callback to be executed when resolving an event.
 * @param   { char }              cState        The input to the callback function.
*/
void EventHandler_keyPressed(p_event_handler eventHandler, char cState) {
  eventHandler(cState);
}


#endif

