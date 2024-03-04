/**
 * @ Author: Mo David
 * @ Create Time: 2024-03-04 14:31:48
 * @ Modified time: 2024-03-04 14:54:20
 * @ Description:
 * 
 * This file defines a queue data structure.
 * We will use this for events and component rendering.
 */

#ifndef UTILS_QUEUE_
#define UTILS_QUEUE_

#include "./utils.types.h"
#include <stdlib.h>

typedef struct QueueEntry QueueEntry;
typedef struct Queue Queue;

/**
 * //
 * ////
 * //////    QueueEntry class
 * ////////
 * ////////// 
*/

/**
 * An instantiable class that represents an entry within a queue.
 * 
 * @class
*/
struct QueueEntry {
  p_obj pObject;      // The object stored by the entry
  QueueEntry *pNext;  // The next entry
};

/**
 * Allocates memory for an instance of the QueueEntry class.
 * 
 * @return	{ QueueEntry * }		A pointer to the allocated memory.
*/
QueueEntry *QueueEntry_new() {
  QueueEntry *pQueueEntry = calloc(1, sizeof(*pQueueEntry));
  return pQueueEntry;
}

/**
 * Initializes an instance of the QueueEntry class.
 * 
 * @param		{ QueueEntry * }		this	    A pointer to the instance to initialize.
 * @param   { p_obj }           pObject   The object stored by the queue entry.
 * @return	{ QueueEntry * }					    A pointer to the initialized instance.
*/
QueueEntry *QueueEntry_init(QueueEntry *this, p_obj pObject) {
  this->pObject = pObject;
  this->pNext = NULL;
  return this;
}

/**
 * Creates an initialized instance of the QueueEntry class.
 * 
 * @param   { p_obj }           pObject   The object stored by the queue entry.
 * @return	{ QueueEntry * }		          A pointer to the newly created initialized instance.
*/
QueueEntry *QueueEntry_create(p_obj pObject) {
  return QueueEntry_init(QueueEntry_new(), pObject);
}

/**
 * Deallocates the memory of an instance of the QueueEntry class.
 * 
 * @param		{ QueueEntry * }		this	A pointer to the instance to deallocate.
*/
void QueueEntry_kill(QueueEntry *this) {
  free(this);
}

/**
 * Chains another entry unto the specified entry.
 * 
 * @param   { QueueEntry * }  this    The entry to append to.
 * @param   { QueueEntry * }  pNext   The entry to append.
*/
void QueueEntry_chain(QueueEntry *this, QueueEntry *pNext) {
  this->pNext = pNext;
}

/**
 * //
 * ////
 * //////    Queue class
 * ////////
 * ////////// 
*/

/**
 * A class that holds queue entries and provides methods for managing these.
 * This is essentially just a singly-linked list.
 * 
 * @class
*/
struct Queue {
  QueueEntry *pHead;  // The first entry in the queue
  QueueEntry *pTail;  // The last entry in the queue
};

/**
 * Allocates memory for an instance of the Queue class.
 * 
 * @return	{ Queue * }		A pointer to the allocated memory.
*/
Queue *Queue_new() {
  Queue *pQueue = calloc(1, sizeof(*pQueue));
  return pQueue;
}

/**
 * Initializes an instance of the Queue class.
 * 
 * @param		{ Queue * }		this	A pointer to the instance to initialize.
 * @return	{ Queue * }					A pointer to the initialized instance.
*/
Queue *Queue_init(Queue *this) {
  this->pHead = NULL;
  this->pTail = NULL;

  return this;
}

/**
 * Creates an initialized instance of the Queue class.
 * 
 * @return	{ Queue * }		A pointer to the newly created initialized instance.
*/
Queue *Queue_create() {
  return Queue_init(Queue_new());
}

/**
 * Deallocates the memory of an instance of the Queue class.
 * 
 * @param		{ Queue * }		this	A pointer to the instance to deallocate.
*/
void Queue_kill(Queue *this) {
  free(this);
}

/**
 * Returns the object stored at the head of the queue.
 * 
 * @param   { Queue * }   this  The queue to read.
 * @return  { p_obj }           The object currently indexed at the head.
*/
p_obj Queue_getHead(Queue *this) {
  return this->pHead->pObject;
}

/**
 * Pushes an entry to the end of the queue.
 * 
 * @param   { Queue * }   this      The queue object we wish to modify.
 * @param   { p_obj }     pObject   The object we wish to enqueue.
*/
void Queue_push(Queue *this, p_obj pObject) {
  QueueEntry *pQueueEntry = QueueEntry_create(pObject);

  // Do stuf acc. to if its the first element or not
  if(this->pHead == NULL)
    this->pHead = pQueueEntry;
  else 
    QueueEntry_chain(this->pTail, pQueueEntry);

  // Make the entry the new tail
  this->pTail = pQueueEntry;
}

/**
 * Removes an entry from the head of the queue.
 * Note that this also frees the instance from memory, BUT 
 *    it DOESN'T free the pObject member of the entry.
 * 
 * @param   { Queue * }   this  The queue object we wish to modify.
*/
void Queue_pop(Queue *this) {
  QueueEntry *pHead;

  // Exit the function if the queue is empty
  if(this->pHead == NULL)
    return;

  // Get a reference to the old head
  pHead = this->pHead;

  // Set the new head to the next element
  this->pHead = this->pHead->pNext;

  // Free the element
  free(pHead);
}

#endif