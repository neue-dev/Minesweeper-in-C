/**
 * @ Author: Mo David
 * @ Create Time: 2024-02-24 14:05:03
 * @ Modified time: 2024-02-24 22:55:04
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
int EventHandler_keyPressed(p_obj pArgs) {
  return 1;
}

/**
 * Handles key releases.
*/
int EventHandler_keyReleased(p_obj pArgs) {
  return 1;
}

#endif