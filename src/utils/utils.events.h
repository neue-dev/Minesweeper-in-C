/**
 * A utility library for implementing events.
*/

#ifndef UTILS_EVENTS
#define UTILS_EVENTS

// Define some constants
#define EVENTS_MAX_COUNT 32

// Windows for now
#ifdef _WIN32

// The needed header files
#include <stdlib.h>
#include <windows.h>
#include <conio.h>
#include <winnt.h>

/**
 * A struct for creating events.
*/
struct Event {
  HANDLE handle;  // Stores a handle to the event

};

// Stores a reference to all the event handles
// We have a maximum of 32 different events
HANDLE eventHandles[EVENTS_MAX_COUNT];

/**
 * Allocates memory for a new event instance.
 * 
 * @return  {struct Event *}  A pointer to the memory location of the created event instance.
*/
struct Event *Event_new() {
  struct Event *pEvent;

  pEvent = calloc(1, sizeof(*pEvent));

  if(pEvent == NULL)
    return NULL;
  return pEvent;
}

/**
 * Initializes an event instance.
 * 
 * @param   {struct Event *}  pEvent      A pointer to the memory location of the event instance.
 * @param   {char *}          eventName   The name of the event.
 * @param   {int}             eventState  The initial state of the event.
*/
void Event_init(struct Event *pEvent, char *eventName, int eventState) {
  pEvent->handle = CreateEvent(
    NULL, 
    TRUE, 
    eventState ? TRUE : FALSE, 
    TEXT(eventName));

  // ! Make sure the event handle is also stored in the event handles array

  if(pEvent->handle == NULL) 
    return;
}

/**
 * Creates a new initialized event instance.
 * 
 * @param   {char *}          eventName   The name of the event.
 * @param   {int}             eventState  The initial state of the event.
 * @return  {struct Event *}  A pointer to the memory location of the created event instance.
*/
struct Event *Event_create(char *eventName, int eventState) {
  struct Event *pEvent = Event_new();

  Event_init(pEvent, eventName, eventState);

  return pEvent;
}

/**
 * Frees the memory from the event instance.
*/
void Event_kill() {
  // ! Implement this part too
}

/**
 * A utility function for cleaning up all the threads after the main program executes.
*/
void CloseEvents() {
  for(int i = 0; i < EVENTS_MAX_COUNT; i++)
    CloseHandle(eventHandles[i]);

  // ! Make sure all the event instances are also freed from memory
}


// Not in Windows
#else
// !add some unix stuff
#endif

#endif