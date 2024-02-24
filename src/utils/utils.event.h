/**
 * @ Author: Mo David
 * @ Create Time: 2024-02-24 13:43:39
 * @ Modified time: 2024-02-24 23:05:29
 * @ Description:
 * 
 * An event object class. This object is instantiable and is created everytime
 *    a new event is fired. The events are stored as a chain (a queue / linked list)
 *    where only the reference to the head is stored and each event is resolved
 *    one after the other (FIFO).
 * Note that this library relies on the utils.thread.h implementation
 */

#ifndef UTILS_EVENTS_
#define UTILS_EVENTS_

#include "./utils.types.h"
#include "./utils.string.h"

#include <stdio.h>
#include <string.h>

typedef enum EventType {
  EVENT_KEY,
  EVENT_MOUSE,
} EventType;

/**
 * //
 * ////
 * //////    Event class
 * ////////
 * ////////// 
*/

/**
 * A class that represents a tenplate for events.
 * We have to forward the typedef because it uses a pointer of its type inside itself.
 * 
 * @class
*/
typedef struct Event Event;

struct Event {

  Event *pNextEvent;                // The next event in the event chain
  EventType eType;                  // What type of event it is

  char cState;                      // The information stored by the event
                                    // A character suffices to store any key press / a binary mouse press / smth else
                                    // We will not be dealing with more complicated events anyway
  
};

/**
 * //
 * ////
 * //////    Constructors and destructors
 * ////////
 * ////////// 
*/

/**
 * Allocates memory for a new event instance.
 * 
 * @return  { Event * }   The allocated memory for the instance.
*/
Event *Event_new() {
  Event *pEvent = calloc(1, sizeof(*pEvent));
  return pEvent;
}

/**
 * Initializes an event instance
 * 
 * @param   { Event * }     this        The event instance to be initialized.
 * @param   { Event * }     pNextEvent  The event after the current instance.
 * @param   { EventType }   eType       The type of the event.
 * @param   { char }        cState      What value is stored by the event.
 * @return  { Event * }                 A pointer to the initialized instance.
*/
Event *Event_init(Event *this, Event *pNextEvent, EventType eType, char cState) {
  
  // Store the reference to the next event
  this->pNextEvent = pNextEvent;
  this->eType = eType;

  // Set the data value stored by the event
  this->cState = cState;
  
  return this;
}

/**
 * Creates a new initialized event.
 * 
 * @param   { Event * }     pNextEvent  The event after the current instance.
 * @param   { EventType }   eType       The type of the event.
 * @param   { char }        cState      What value is stored by the event.
 * @return  { Event * }                 A pointer to the initialized instance.
*/
Event *Event_create(Event *pNextEvent, EventType eType, char cState) {
  return Event_init(Event_new(), pNextEvent, eType, cState);
}

/**
 * Releases an event instance from memory.
 * 
 * @param   { Event * }   this  An event instance.
*/
void Event_kill(Event *this) {
  free(this);
}

/**
 * //
 * ////
 * //////    Chaining and resolving methods
 * ////////
 * ////////// 
*/

/**
 * Chains a new event to an existing event.
 * 
 * @param   { Event * }   this        The current event.
 * @param   { Event * }   pNextEvent  The event to append to the current event.
*/
void Event_chain(Event *this, Event *pNextEvent) {
  this->pNextEvent = pNextEvent;
}

/**
 * Resolves an event.
 * 
 * @param   { Event * }   this  The current event.
*/
void Event_resolve(Event *this) {
  // ! do something here
}

/**
 * //
 * ////
 * //////    EventManager struct
 * ////////
 * ////////// 
*/

/**
 * A struct for managing events.
 * Note that all the methods defined for the Event class should not be accessed directly.
 * If we wish to deal with events, we have to use the EventManager below to interact with 
 *    that class; this is similar to how the thread implementation works too.
*/
typedef struct EventManager {
  
  Event *pHead;     // A reference to the head so we can resolve the oldest event
  Event *pTail;     // A reference so we know where to append new events

  int dEventCount;  // How many events there are at the moment

} EventManager;

/**
 * Initializes the event manager object.
 * 
 * @param   { EventManager * }  this  The EventManager to initialize.
*/
void EventManager_init(EventManager *this) {
  this->pHead = NULL;
  this->pTail = NULL;

  this->dEventCount = 0;
}

/**
 * Cleans up after the event manager object.
 * 
 * @param   { EventManager * }  this  The EventManager to close.
*/
void EventManager_exit() {
  // ! for each event, force resolve them then remove them from memory
}

/**
 * Creates an event and appends it to the event chain.
*/
void EventManager_createEvent(EventManager *this, EventType eType, char cState) {

  // The new event
  Event *pEvent = Event_create(NULL, eType, cState);

  // The queue of events is currently empty
  if(this->pHead == NULL) {
    this->pHead = pEvent;
    this->pTail = pEvent;
  
  // If its not empty, we append it to the end
  } else {
    Event_chain(this->pTail, pEvent);
    this->pTail = pEvent; 
  }

  this->dEventCount++;
}

/**
 * Resolves the head event of the chain.
*/
void EventManager_resolveEvent(EventManager *this) {
  
  // We don't have events at the moment
  if(!this->dEventCount)
    return;

  // Resolve the head
  Event_resolve(this->pHead);

  // If the tail and head were one, set them to NULL since we can't move the pointers forward
  if(this->dEventCount == 1) {

    // Destroy the last resolved event
    Event_kill(this->pHead);
    
    // Set them to NULL
    this->pHead = NULL;
    this->pTail = NULL; 

  // Otherwise, move the head pointer forward
  } else {
    
    // Save the next event first
    Event *pNextEvent = this->pHead->pNextEvent;
    
    // Destroy the resolved event
    Event_kill(this->pHead);

    // Move the pointer forward
    this->pHead = pNextEvent;
  }

  this->dEventCount--;
}

/**
 * //
 * ////
 * //////    Event listeners 
 * ////////
 * ////////// 
*/

// You are in Windows
#ifdef _WIN32
#include "./win/utils.event.handlers.win.h"

// Not in Windows
#else
#include "./unix/utils.event.handlers.unix.h"
#endif

#endif