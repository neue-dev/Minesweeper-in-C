/**
 * @ Author: MMMM
 * @ Create Time: 2024-02-25 10:46:20
 * @ Modified time: 2024-03-04 12:32:06
 * @ Description:
 * 
 * This file contains definitions for event listeners and event handlers.
 * They implement functionalities based on the templates provided by the 
 *    utils.event.h file.
 * 
 * Also, it has a state manager KeyEvents, the implementation of which is independent
 *    of utils.event.h (its up to the programmer how to define the state stored after
 *    events are fired and handled).
 */

#ifndef EVENTS_
#define EVENTS_

#include "./utils/utils.io.h"
#include "./utils/utils.event.h"
#include "./utils/utils.types.h"

/**
 * //
 * ////
 * //////    Event listener and handler implementations
 * ////////
 * ////////// 
*/

/**
 * Key pressed listener and handler
*/
char EventListener_keyPressed(void);

void EventHandler_keyPressed(p_obj pArgs_Event, p_obj pArgs2_EventStore);

/**
 * Timer listener and handler
*/
char EventListener_timerTick(void);

void EventHandler_timerTick(p_obj pArgs_Event, p_obj pArgs2_EventStore);

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
 * @param   { p_obj }   pArgs_Event   The actual event object to be handled.
*/
void EventHandler_keyPressed(p_obj pArgs_Event, p_obj pArgs2_EventStore) {
  
  // This line is always required for all event handlers
  Event *this = (Event *) pArgs_Event;
  
  // This line can vary based on what object the programmer wants to use to store event states
  EventStore *pEventStore = (EventStore *) pArgs2_EventStore;

  // Update the state handler
  EventStore_set(pEventStore, "key-pressed", this->cState);
}

#endif