/**
 * @ Author: MMMM
 * @ Create Time: 2024-02-24 13:43:39
 * @ Modified time: 2024-02-25 11:16:21
 * @ Description:
 * 
 * An event object class. This object is instantiable and is created everytime
 *    a new event is fired. The events are stored as a chain (a queue / linked list)
 *    where only the reference to the head is stored and each event is resolved
 *    one after the other (FIFO).
 * Note that this library relies on the utils.thread.h implementation
 */

#ifndef UTILS_EVENT_
#define UTILS_EVENT_

#include "./utils.types.h"
#include "./utils.string.h"

#include <stdio.h>
#include <string.h>

// We'll have one chain per event type, and at most one listener per event type
// At most 8 in case we want to extend this in the future
#define EVENT_MAX_HANDLER_CHAINS 8
#define EVENT_MAX_LISTENERS 8

typedef enum EventType {
  EVENT_KEY,                // Key events
  EVENT_MOUSE,              // Not even sure if i'll implement mouse events
                            //    Consider it just a placeholder so the enum
                            //    doesn't have a single lonely type ahahaha
} EventType;

typedef struct Event Event;
typedef struct EventHandler EventHandler;
typedef struct EventListener EventListener;

/**
 * //
 * ////
 * //////    EventListener class
 * ////////
 * ////////// 
*/

/**
 * A class that represents how events are triggered.
 * Note that only one event listener can be implemented per event type.
 * Event handlers, on the other hand, can be chained so that each event type
 *    can have more than one way of resolving.
 * 
 * In other words, only ONE THING can trigger events but MULTIPLE THINGS can
 *    react to them. EventListeners trigger events while EventHandlers react 
 *    to them (aka resolving them).
 * 
 * @class
*/
struct EventListener {
  f_event_listener fEventListener;  // The actual event listener function
};

/**
 * Allocates memory for a new event handler instance.
 * 
 * @return  { EventHandler * }    A new instance of the event handler.
*/
EventListener *EventListener_new() {
  EventListener *pEventListener = calloc(1, sizeof(*pEventListener));
  return pEventListener;
}

/**
 * Initializes an event listener instance.
 * 
 * @param   { EventListener * }   this            The instance to initialize.
 * @param   { f_event_listener }  fEventListener  The actual callback to execute to trigger events.
 * @return  { EventListener * }                   A new instance of the event listener.
*/
EventListener *EventListener_init(EventListener *this, f_event_listener fEventListener) {
  this->fEventListener = fEventListener;

  return this;
}

/**
 * Creates an initialized event handler instance.
 * 
 * @param   { f_event_listener }  fEventHandler   The actual callback to execute to trigger events.
 * @return  { EventListener * }                   A new instance of the event listener.
*/
EventListener *EventListener_create(f_event_listener fEventListener) {
  return EventListener_init(EventListener_new(), fEventListener);
}

/**
 * Deallocates the memory associated with an event listener instance. 
 * 
 * @param   { EventListener * }   this  The event listener to free.
*/
void EventListener_kill(EventListener *this) {
  free(this);
}

/**
 * Waits for the event trigger and returns the outcome of the event.
 * This should return 0 when nothing happens.
 * 
 * @param   { EventListener * }   this  The event listener to wait for.
 * @return  { char }                    The outcome of the event (key press value, mouse click true, etc.).
*/
char EventListener_trigger(EventListener *this) {
  return this->fEventListener();
}

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
void Event_resolve(Event *this, p_obj Args2_ANY) {

  // While we have handlers on the chain
  while(this->pHeadHandler != NULL) {

    // Execute the event handler then move the pointer
    // Note that we DON'T free the handlers from memory BECAUSE
    //    these are the same handlers other events of the same type
    //    will refer to
    this->pHeadHandler->fEventHandler(this, Args2_ANY);
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
  
  EventListener *pListeners[EVENT_MAX_LISTENERS];           // We have at most one listener per event type
  EventHandler *pHandlerHeads[EVENT_MAX_HANDLER_CHAINS];    // The head pointers to the event chains associated with each event type
  EventHandler *pHandlerTails[EVENT_MAX_HANDLER_CHAINS];    // We need these references to be able to append to the queue

  int dEventCount;                                          // How many events there are at the moment
  p_obj Args2_ANY;                                          // State manager: what object handlers mutate to manage state

} EventManager;

/**
 * Initializes the event manager object.
 * 
 * @param   { EventManager * }  this        The EventManager to initialize.
 * @param   { p_obj }           Args2_ANY   An object that stores state.
*/
void EventManager_init(EventManager *this, p_obj Args2_ANY) {
  int i;

  // No events yet
  this->pHead = NULL;
  this->pTail = NULL;

  this->dEventCount = 0;
  this->Args2_ANY = Args2_ANY;

  // No listeners yet
  for(i = 0; i < EVENT_MAX_LISTENERS; i++)
    this->pListeners[i] = NULL;

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
void EventManager_exit(EventManager *this) {
  int i;

  // No new events should be coming in, they should be resolved by the handler thread
  // If there is no handler thread, they should be manually resolved by calling EventManager_resolveEvent();
  //    before calling EventManager_exit();
  while(this->dEventCount) { 

    // Wait for the events to run out 
    // No code here
  }

  // In this case, we just deal with the event handlers and clean them up
  for(i = 0; i < EVENT_MAX_HANDLER_CHAINS; i++) {

    // While the chain has event handlers
    while(this->pHandlerHeads[i] != NULL) {

      // Get the next handler to destroy
      EventHandler *pNextHandler = this->pHandlerHeads[i]->pNextHandler;
      
      // Destroy the current handler
      EventHandler_kill(this->pHandlerHeads[i]);

      // Move the pointer forward
      this->pHandlerHeads[i] = pNextHandler;
    }

    // Just set the tails to NULL after the heads have been cleared
    this->pHandlerTails[i] = NULL;
  }
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
 * Waits for event triggers and creates events when they occur.
 * Note that we specify event type here so that we can pass this function into different threads
 *    for each event type. That way, different event types dont block each other when triggering.
 * 
 * @param   { p_obj }   pArgs_EventManager  The event manager that will trigger events.
 * @param   { int }     tArg_eType          The event type to watch out for.
*/
void EventManager_triggerEvent(p_obj pArgs_EventManager, int tArg_eType) {
  EventManager *this = (EventManager *) pArgs_EventManager;
  EventType eType = (EventType) tArg_eType;

  // There is no event listener for the event type
  if(this->pListeners[eType] == NULL)
    return;
    
  // Save the outcome of the event
  char cState = EventListener_trigger(this->pListeners[eType]);

  // If the event occured
  if(cState)
    EventManager_createEvent(this, eType, cState);
}

/**
 * Resolves the head event of the chain.
 * 
 * @param   { p_obj }   pArgs_EventManager  The event manager that will resolve events.
 * @param   { int }     tArg_NULL           A dummy variable we don't need. Specifying event types is not
 *                                            needed here since we're resolving the entire event chain in 
 *                                            order without any bias for the event type.
*/
void EventManager_resolveEvent(p_obj pArgs_EventManager, int tArg_NULL) {
  EventManager *this = (EventManager *) pArgs_EventManager;
  
  // We don't have events at the moment
  if(!this->dEventCount)
    return;

  // Resolve the head
  Event_resolve(this->pHead, this->Args2_ANY);

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
 * Once an event handler has been added to the chain, it cannot be removed.
 * 
 * @param   { EventManager * }      this            The event manager to append the handler to.
 * @param   { EventType }           eType           What type of events the handler applies to.
 * @param   { f_event_handler * }   fEventHandler   The event handler function to append.
*/
void EventManager_createEventHandler(EventManager *this, EventType eType, f_event_handler fEventHandler) {
  
  // Create the new event handler object with NULL as its next in line
  EventHandler *pEventHandler = EventHandler_create(NULL, fEventHandler);

  // If there are currently no handlers associated with that event type
  if(this->pHandlerHeads[eType] == NULL) {
    this->pHandlerHeads[eType] = pEventHandler;
    this->pHandlerTails[eType] = pEventHandler;
  
  // Otherwise, append it to the end of the chain
  } else {
    EventHandler_chain(this->pHandlerTails[eType], pEventHandler);
    this->pHandlerTails[eType] = pEventHandler;
  }
}

/**
 * Creates an event listener for the specified event type.
 * Note that if an event listener already exists for that event type, it overwrites the original listener.
 * 
 * @param   { EventManager * }      this            The event manager to add the listener to.
 * @param   { EventType }           eType           What type of events the listener applies to.
 * @param   { f_event_listener * }  fEventListener  The event listener function to append.
*/
void EventManager_createEventListener(EventManager *this, EventType eType, f_event_listener fEventListener) {
  
  // Create a new event listener
  EventListener *pEventListener = EventListener_create(fEventListener);

  // If there are currently no listeners for the event type
  if(this->pListeners[eType] == NULL) {
    this->pListeners[eType] = pEventListener;
  
  // Otherwise, overwrite the existing listener
  } else {
    EventListener_kill(this->pListeners[eType]);
    this->pListeners[eType] = pEventListener;
  }
}

#endif