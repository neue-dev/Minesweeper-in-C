/**
 * @ Author: Mo David
 * @ Create Time: 2024-02-24 14:04:52
 * @ Modified time: 2024-02-24 23:19:04
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

/**
 * Handles key presses.
*/
void EventListener_keyPressed(p_obj pArgs) {
  EventManager *this = (EventManager *) pArgs;

  EventManager_createEvent(this, EVENT_KEY, IO_readChar());
}

#endif

