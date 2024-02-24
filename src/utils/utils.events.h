/**
 * @ Author: Mo David
 * @ Create Time: 2024-02-24 13:43:39
 * @ Modified time: 2024-02-24 18:18:50
 * @ Description:
 * 
 * An event object struct. Note that this struct is not instantiable since we don't 
 *    really need to define events during runtime. 
 */

#ifndef UTILS_EVENTS_
#define UTILS_EVENTS_

#include "./utils.types.h"
#include "./utils.string.h"

#include <string.h>

#define EVENT_MAX_LOGS 16   // The maximum number of event states to store in a history

/**
 * A list of possible events states.
 * These may be subject to change as the development process ensues.
*/
typedef enum EventState { 
  EVENT_WAITING,    // Event is now waiting (possibly after EVENT_RESOLVED)
  EVENT_FIRED,      // Event was fired after the EVENT_WAITING state
  EVENT_RESOLVED,   // Event was resolved after the EVENT_FIRED state
  EVENT_ERROR,      // An error occured
} EventState;

/**
 * A struct that represents a tenplate for events.
 * 
 * @struct
*/
typedef struct Event {
  /**
   * * should have a name to identify the type of event
   * * should store data about previous events (?)
   * * should have an enum about different possible states
   * * 
   * * should have a method for closing the event (resolving it)
   * * should have a trigger method + variable to keep isTriggered
  */

  char *sName;                      // An identifier for the object.
  EventState eState;                // An integer value that stores the state of the event

  int dLogs;                        // How many logs have currently been made.
                                    // Note that this does not represent the length of the logs array.
                                    //    More logs could have been made than EVENT_MAX_LOGS, but only at most
                                    //    EVENT_MAX_LOGS are stored in the arrays.

  char *sLogArray[EVENT_MAX_LOGS];  // A history of string values describing the events that occured.
  int dLogArray[EVENT_MAX_LOGS];    // A history of integer values describing the events that occured.

  p_bool_func pTrigger;             // A function that returns true when the event occurs
  
} Event;

/**
 * Initializes a new instance of the event class.
 *
 * @param   { Event * }       this      A pointer to an event object.
 * @param   { char * }        sName     An identifier for the event object.
 * @param   { p_bool_func }   pTrigger  A pointer to the function that determines whether or not the event occured.
*/
void Event_init(Event *this, char *sName, p_bool_func pTrigger) {
  
  this->sName = sName;            // Set the name
  this->pTrigger = pTrigger;      // Set the callback function

  this->dLogs = 0;                // Currently no logs have been made
  this->eState = EVENT_WAITING;   // Currently waiting for an event trigger
}

/**
 * Triggers the event and stores a log of the event in its log arrays.
*/
void Event_trigger(Event *this, char *sEventLog, int dEventLog) {
  int i;

  // There's already too many logs, so we start removing older ones
  if(this->dLogs >= EVENT_MAX_LOGS) {
    
    // Shift everything in the array one space back
    for(i = 0; i < EVENT_MAX_LOGS - 1; i++) {

      // Deal with the strings
      free(this->sLogArray[i]);
      this->sLogArray[i] = String_new(this->sLogArray[i + 1]);

      // Easy int shift
      this->dLogArray[i] = this->dLogArray[i + 1];
    }

    // Save the new string
    free(this->sLogArray[EVENT_MAX_LOGS - 1]);
    this->sLogArray[EVENT_MAX_LOGS - 1] = String_new(sEventLog);
    this->dLogArray[EVENT_MAX_LOGS] = dEventLog;

  // We proceed to append new logs to the end of the array
  } else {
    this->sLogArray[this->dLogs] = String_new(sEventLog);
    this->dLogArray[this->dLogs] = dEventLog;
  }

  this->dLogs++;
  this->eState = EVENT_FIRED;
}

/**
 * Resolves an event after it occurs.
 * This helps revert the state associated with the event once it has been dealt with.
 * 
 * @param   { Event * }   this  The event object to resolve.
*/
void Event_resolve(Event *this) {
  this->eState = EVENT_RESOLVED;
}

/**
 * Cleans up any stuff set up by the event object.
 * 
 * @param   { Event * }   this  The event object to clean up.
*/
void Event_exit(Event *this) {
  
}

/**
 * A helper function to be passed as a callback to a thread instance.
 * Note that every time we have a callback to be passed onto a thread, we have to
 *    use the p_obj data type (which is just a void pointer). The passed object is
 *    then typecast into its respective type.
 * // ! for unix, make sure it returns a void *
 * 
 * @param   { p_obj }   pArgs   The event to pass into the handler.
*/
void EventHandler(p_obj pArgs) {
  Event *pEvent = (Event *) pArgs;

  // The event happened
  if(pEvent->pTrigger(pArgs)) {



  // The event didn't happen
  } else {
    
  }

}

// You are in Windows
#ifdef _WIN32
#include "./win/utils.events.win.h"

// Not in Windows
#else
#include "./unix/utils.events.unix.h"
#endif

#endif