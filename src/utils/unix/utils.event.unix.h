/**
 * @ Author: Mo David
 * @ Create Time: 2024-02-24 14:05:03
 * @ Modified time: 2024-02-25 09:58:57
 * @ Description:
 * 
 * Some basic event handlers for Unix.
 */

#ifndef UTILS_EVENTS_UNIX_
#define UTILS_EVENTS_UNIX_

#include "../utils.io.h"
#include "../utils.event.h"
#include "../utils.types.h"

#include <stdio.h>

/**
 * Key pressed listener and handler
*/
char EventListener_keyPressed(void);

void EventHandler_keyPressed(p_obj pArgs);

/**
 * Listens for key presses.
 * This function can wait for a key press or just return 0 when nothing happens.
 * In this case, it waits for a key press but the implementation can vary.
 * 
 * @return  { char }  The outcome of the event.
*/
char EventListener_keyPressed(void) {
  return IO_readChar();
}

/**
 * Handles key presses.
 * Basically, what happens after a key is pressed.
 * 
 * @param   { p_obj }   pArgs   The actual event object to be handled.
*/
void EventHandler_keyPressed(p_obj pArgs) {
  
  // This line is always required
  Event *pEvent = (Event *) pArgs;

  
  // ! make it be able to store this somewhere or idkk
  // ! give a way for the programmer to be able to add eventhandlers at runtime
  // ! maybe turn eventHandler into its own class?

  printf("%c\n", pEvent->cState);
}

/**
 * Handles key presses.
 * Basically, what happens after a key is pressed.
 * 
 * @param   { p_obj }   pArgs   The actual event object to be handled.
*/
void EventHandler_keyPressed2(p_obj pArgs) {
  
  // This line is always required
  Event *pEvent = (Event *) pArgs;

  
  // ! make it be able to store this somewhere or idkk
  // ! give a way for the programmer to be able to add eventhandlers at runtime
  // ! maybe turn eventHandler into its own class?

  printf("%c%c\n", pEvent->cState, pEvent->cState);
}

#endif