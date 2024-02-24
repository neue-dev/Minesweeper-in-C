/**
 * @ Author: Mo David
 * @ Create Time: 2024-02-24 14:05:03
 * @ Modified time: 2024-02-24 23:05:49
 * @ Description:
 * 
 * Some basic event handlers for Unix.
 */

#ifndef UTILS_EVENTS_UNIX_
#define UTILS_EVENTS_UNIX_

#include "../utils.io.h"
#include "../utils.event.h"

#include <stdio.h>

/**
 * Handles key presses.
*/
void *EventListener_keyPressed(p_obj pArgs) {
  IO_readChar();

  return 1;
}

#endif