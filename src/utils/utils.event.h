/**
 * @ Author: Mo David
 * @ Create Time: 2024-02-24 13:43:39
 * @ Modified time: 2024-02-25 08:21:43
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

// We'll have one chain per event type
// At most 8 in case we want to extend this in the future
#define EVENT_MAX_HANDLER_CHAINS 8

typedef enum EventType {
  EVENT_KEY,                // Key events
  EVENT_MOUSE,              // Not even sure if i'll implement mouse events
                            //    Consider it just a placeholder so the enum
                            //    doesn't have a single lonely type ahahaha
} EventType;

typedef struct Event Event;
typedef struct EventHandler EventHandler;

/**
 * //
 * ////
 * //////    EventHandler class
 * ////////
 * ////////// 
*/

/**
 * A class that represents how an event should resolve itself.
 * These can also be chained so that more than one event handler can be tacked onto
 *    a single event.
 * 
 * @class
*/
struct EventHandler {
  EventHandler *pNextHandler;     // The next event handler in the chain
  f_event_handler fEventHandler;  // The actual event handler function
                                  // The event instance will be passed to this function
};

/**
 * Allocates memory for a new event handler instance.
 * 
 * @return  { EventHandler * }    A new instance of the event handler.
*/
EventHandler *EventHandler_new() {
  EventHandler *pEventHandler = calloc(1, sizeof(*pEventHandler));
  return pEventHandler;
}

/**
 * Initializes an event handler instance.
 * 
 * @param   { EventHandler * }    this            The instance to initialize.
 * @param   { EventHandler * }    pNextHandler    The next handler in the chain.
 * @param   { f_event_handler }   fEventHandler   The actual callback to execute when resolving events.
 * @return  { EventHandler * }                    A new instance of the event handler.
*/
EventHandler *EventHandler_init(EventHandler *this, EventHandler *pNextHandler, f_event_handler fEventHandler) {
  this->pNextHandler = pNextHandler;
  this->fEventHandler = fEventHandler;

  return this;
}

/**
 * Creates an initialized event handler instance.
 * 
 * @param   { EventHandler * }    pNextHandler    The next handler in the chain.
 * @param   { f_event_handler }   fEventHandler   The actual callback to execute when resolving events.
 * @return  { EventHandler * }                    A new instance of the event handler.
*/
EventHandler *EventHandler_create(EventHandler *pNextHandler, f_event_handler fEventHandler) {
  return EventHandler_init(EventHandler_new(), pNextHandler, fEventHandler);
}

/**
 * Deallocates the memory associated with an event handler instance. 
 * 
 * @param   { EventHandler * }  this  The event handler to free.
*/
void EventHandler_kill(EventHandler *this) {
  free(this);
}

/**
 * Appends an event handler after another one.
 * 
 * @param   { EventHandler * }  this          The event handler to append to.
 * @param   { EventHandler * }  pNextHandler  The next event handler we wish to put in the chain.
*/
void EventHandler_chain(EventHandler *this, EventHandler *pNextHandler) {
  this->pNextHandler = pNextHandler;
}

/**
 * //
 * ////
 * //////    Event class
 * ////////
 * ////////// 
*/

/**
 * A class that represents a template for events.
 * We have to forward the typedef because it uses a pointer of its type inside itself.
 * 
 * @class
*/
struct Event {

  Event *pNextEvent;                // The next event in the event chain
  EventType eType;                  // What type of event it is
  EventHandler *pHeadHandler;       // The first event handler assigned to this type of event
                                    // Note that we don't need the last one since we won't be appending handlers
                                    //    once the event has been created

  char cState;                      // The information stored by the event
                                    // A character suffices to store any key press / a binary mouse press / smth else
                                    // We will not be dealing with more complicated events anyway
  
};

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
 * @param   { Event * }           this            The event instance to be initialized.
 * @param   { Event * }           pNextEvent      The event after the current instance.
 * @param   { EventType }         eType           The type of the event.
 * @param   { EventHandler * }    pHeadHandler    The first callback function that resolves the event.
 * @param   { char }              cState          What value is stored by the event.
 * @return  { Event * }                           A pointer to the initialized instance.
*/
Event *Event_init(Event *this, Event *pNextEvent, EventType eType, EventHandler *pHeadHandler, char cState) {
  
  // Store the reference to the next event
  this->pNextEvent = pNextEvent;
  this->eType = eType;

  // Store the event handler
  this->pHeadHandler = pHeadHandler;

  // Set the data value stored by the event
  this->cState = cState;
  
  return this;
}

/**
 * Creates a new initialized event.
 * 
 * @param   { Event * }           pNextEvent      The event after the current instance.
 * @param   { EventType }         eType           The type of the event.
 * @param   { EventHandler * }    pHeadHandler    The first callback function that resolves the event.
 * @param   { char }              cState          What value is stored by the event.
 * @return  { Event * }                           A pointer to the initialized instance.
*/
Event *Event_create(Event *pNextEvent, EventType eType, EventHandler *pHeadHandler, char cState) {
  return Event_init(Event_new(), pNextEvent, eType, pHeadHandler, cState);
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

  // While we have handlers on the chain
  while(this->pHeadHandler != NULL) {

    // Execute the event handler then move the pointer
    // Note that we DON'T free the handlers from memory BECAUSE
    //    these are the same handlers other events of the same type
    //    will refer to
    this->pHeadHandler->fEventHandler(this);
    this->pHeadHandler = this->pHeadHandler->pNextHandler;
  }
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
 *    that class; this is also true for the EventHandler class--all its methods should not
 *    be accessed directly. The design pattern we follow here is similar to how our thread 
 *    implementation works too.
*/
typedef struct EventManager {
  
  Event *pHead;                                             // A reference to the head so we can resolve the oldest event
  Event *pTail;                                             // A reference so we know where to append new events

  EventHandler *pHandlerHeads[EVENT_MAX_HANDLER_CHAINS];    // The head pointers to the event chains associated with each event type
  EventHandler *pHandlerTails[EVENT_MAX_HANDLER_CHAINS];    // We need these references to be able to append to the queue

  int dEventCount;                                          // How many events there are at the moment

} EventManager;

/**
 * Initializes the event manager object.
 * 
 * @param   { EventManager * }  this  The EventManager to initialize.
*/
void EventManager_init(EventManager *this) {
  int i;

  // No events yet
  this->pHead = NULL;
  this->pTail = NULL;
  this->dEventCount = 0;

  // Just so we don't have garbage values atm
  // We need to do this here so we can check for NULL when assigning events their handlers
  for(i = 0; i < EVENT_MAX_HANDLER_CHAINS; i++) {
    this->pHandlerHeads[i] = NULL;
    this->pHandlerTails[i] = NULL;
  }
}

/**
 * Cleans up after the event manager object.
 * 
 * @param   { EventManager * }  this  The EventManager to close.
*/
void EventManager_exit() {
  // ! for each event, force resolve them then remove them from memory
  // ! clear the event handler chain
}

/**
 * Creates an event and appends it to the event chain.
 * 
 * @param   { EventManager * }  this    The event manager we used to create the event.
 * @param   { EventType }       eType   The type of event to create.
 * @param   { char }            cState  What data the event will store.
*/
void EventManager_createEvent(EventManager *this, EventType eType, char cState) {

  // The new event
  Event *pEvent = Event_create(NULL, eType, NULL, cState);
  
  // If we have event handlers for this type of event
  if(this->pHandlerHeads[eType] != NULL)
    pEvent->pHeadHandler = this->pHandlerHeads[eType];

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
 * 
*/
void EventManager_createEventHandler() {
  // ! only create event handlers when the chain currently isnt being used by the events
}

/**
 * 
*/
void EventManager_killEventHandler() {
  // ! only remove event handlers when the chain isnt being used by the events
}

/**
 * //
 * ////
 * //////    Event listener imports
 * ////////
 * ////////// 
*/

// You are in Windows
#ifdef _WIN32
#include "./win/utils.event.win.h"

// Not in Windows
#else
#include "./unix/utils.event.unix.h"
#endif

#endif