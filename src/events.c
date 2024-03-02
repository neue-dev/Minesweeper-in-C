/**
 * @ Author: MMMM
 * @ Create Time: 2024-02-25 10:46:20
 * @ Modified time: 2024-03-02 16:57:53
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

#include "./utils/utils.io.h"
#include "./utils/utils.event.h"
#include "./utils/utils.types.h"

#define EVENTS_MAX_HISTORY 32

typedef struct KeyEvents KeyEvents;
typedef struct TimeEvents TimeEvents;

/**
 * //
 * ////
 * //////    KeyEvents struct
 * ////////
 * ////////// 
*/

/**
 * A struct that stores key events and other pertinent information
 * 
 * @struct 
*/
struct KeyEvents {

  int bHasBeenRead;                   // A boolean indicating whether or not the latest press has been read

  char cLatest;                       // The latest character press
  char cHistory[EVENTS_MAX_HISTORY];  // The history of key presses
  int dHistoryLength;                 // How long the history currently is

};

/**
 * Initializes the key events struct.
 * 
 * @param   { KeyEvents * }   this  The KeyEvents struct.
*/
void KeyEvents_init(KeyEvents *this) {
  int i;

  this->bHasBeenRead = 1;
  this->cLatest = 0;
  this->dHistoryLength = 0;

  for(i = 0; i < EVENTS_MAX_HISTORY; i++)
    this->cHistory[i] = 0;
}

/**
 * Sets the bHasBeenRead boolean to true.
 * 
 * @param   { keyEvents * }   this  The state handler to modify.
*/
void KeyEvents_read(KeyEvents *this) {
  this->bHasBeenRead = 1;
}

/**
 * //
 * ////
 * //////    Time events struct
 * ////////
 * ////////// 
*/

/**
 * A struct that stores key events and other pertinent information
 * 
 * @struct 
*/
struct TimeEvents {

  int bHasBeenRead;                   // A boolean indicating whether or not the latest tick has been read
  int dCurrentTime;                   // Number of seconds elapsed since thread launch

};

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

void EventHandler_keyPressed(p_obj pArgs_Event, p_obj pArgs2_KeyEvents);

/**
 * Timer listener and handler
*/
char EventListener_timerTick(void);

void EventHandler_timerTick(p_obj pArgs_Event, p_obj pArgs2_TimeEvents);

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
void EventHandler_keyPressed(p_obj pArgs_Event, p_obj pArgs2_KeyEvents) {
  int i;

  // This line is always required for all event handlers
  Event *pEvent = (Event *) pArgs_Event;
  
  // This line can vary based on what object the programmer wants to use to store event states
  KeyEvents *pKeyEvents = (KeyEvents *) pArgs2_KeyEvents;

  // Update the state handler
  pKeyEvents->cLatest = pEvent->cState;

  // We've capped out on event history
  // We start overwriting old values
  if(pKeyEvents->dHistoryLength >= EVENTS_MAX_HISTORY) {

    // Shift the entire array backwards
    for(i = 0; i < EVENTS_MAX_HISTORY - 1; i++)
      pKeyEvents->cHistory[i] = pKeyEvents->cHistory[i + 1];

    // Store the latest at the end of the buffer
    pKeyEvents->cHistory[EVENTS_MAX_HISTORY - 1] = pKeyEvents->cLatest;

  // We can still increment our history length
  } else {
    pKeyEvents->cHistory[pKeyEvents->dHistoryLength] = pKeyEvents->cLatest;
    pKeyEvents->dHistoryLength++;
  }

  // Change the boolean to say it hasnt been read
  pKeyEvents->bHasBeenRead = 0;
}