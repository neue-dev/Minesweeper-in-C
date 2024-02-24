/**
 * @ Author: Mo David
 * @ Create Time: 2024-02-24 14:04:52
 * @ Modified time: 2024-02-24 22:54:49
 * @ Description:
 * 
 * Some basic event handlers for Windows.
 */

#ifndef UTILS_EVENTS_WIN_
#define UTILS_EVENTS_WIN_

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

