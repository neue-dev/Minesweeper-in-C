/**
 * @ Author: MMMM
 * @ Create Time: 2024-01-29 12:01:02
 * @ Modified time: 2024-02-24 13:44:40
 * @ Description:
 *    
 * A utility library for implementing threads.
 */

#ifndef UTILS_THREAD_
#define UTILS_THREAD_

// You are in Windows
#ifdef _WIN32
#include "./win/utils.thread.win.h"

// Not in Windows
#else
#include "./unix/utils.thread.unix.h"
#endif

#include <string.h>

/**
 * //
 * ////
 * //////    ThreadManager struct
 * ////////
 * ////////// 
*/

/**
 * The ThreadManager struct stores information related to all currently existing threads.
 * If ever we want to do anything involving threads, we must interact with this struct 
 *    instead of calling any of the Thread methods defined above. 
 * It allows us to do this without having to pollute the global namespace with variables.
 * Note that this also uses the implementations we defined for both Windows and Unix, without
 *    it having to know the stuff they do under the hood. This keeps our code clean. You can think
 *    of this as some sort of "manager" which handles all our thread and mutex instances.
 * 
 * @struct
*/
typedef struct ThreadManager {

  Thread *pThreadArray[THREAD_MAX_COUNT];             // Stores references to all the threads
  Mutex *pStateArray[THREAD_MAX_COUNT];               // For each thread, we have a state mutex to tell it to keep running        
  Mutex *pMutexArray[MUTEX_MAX_COUNT];                // Stores references to all the mutexes

  int dThreadCount;                                   // Stores the length of the threads array
  int dMutexCount;                                    // Stores the length of the mutexes array

} ThreadManager;

/**
 * Initializes the thread manager variables.
 * 
 * @param   { ThreadManager * }  this  The thread manager to be initialized.
 * @return  { ThreadManager * }        The initialized thread manager struct.
*/
ThreadManager *ThreadManager_init(ThreadManager *this) {

  // Set the array sizes to 0
  this->dThreadCount = 0;
  this->dMutexCount = 0;

  return this;
}

/**
 * Cleans up the state of the thread manager.
 * 
 * @param   { ThreadManager * }  The thread manager to exit.
*/
void ThreadManager_exit(ThreadManager *this) {

  // Kill all the threads first
  while(this->dThreadCount) {

    // This tells the thread to terminate
    Mutex_unlock(this->pStateArray[this->dThreadCount]);

    // Get rid of the references
    this->pThreadArray[this->dThreadCount] = NULL;

    // Decrement the size of the array
    this->dThreadCount--;
  }

  // Kill all the mutexes after killing the threads
  while(this->dMutexCount) {

    // Kill the mutex
    Mutex_kill(this->pMutexArray[this->dMutexCount]);

    // Get rid of the references
    this->pMutexArray[this->dMutexCount] = NULL;

    // Decrement the size of the array
    this->dMutexCount--;
  }
}

/**
 * Creates a new thread object instance and adds it to the array.
 * Returns the index of the thread within that array. 
 * Returns -1 if there are too many threads already or something else happened.
 * 
 * @param   { ThreadManager * }   this              A reference to the thread manager object.
 * @param   { char * }            sName             The identifier for the thread and its data mutex.
 * @param   { char * }            sMutexName        The name of the mutex to be associated with.
 * @param   { Mutex * }           pDataMutex        A handle to the mutex that tells the thread whether it can modify its resource.
 * @param   { param_func }        pCallee           A pointer to the callback to be executed by the thread.
 * @param   { param_obj }         pArgs             A pointer to the arguments to be passed to the callback
 * @return  { int }                                 The index of the created thread within the array of the manager.
*/
int ThreadManager_createThread(ThreadManager *this, char *sName, char *sMutexName, param_func pCallee, param_obj pArgs) {
  int i = 0, dIndex = 0, dMutexIndex = 0;
  
  // Find an empty spot in our array first
  for(i = 0; i < this->dThreadCount; i++) {
    if(this->pThreadArray[i] == NULL) {
      dIndex = i;
      i = this->dThreadCount + 1;
    }
  }

  if(i <= this->dThreadCount)
    dIndex = this->dThreadCount;

  // Gets the last mutex with the specified name
  for(i = 0; i < this->dMutexCount; i++) {
    if(!strcmp(this->pMutexArray[i]->sName, sMutexName))
      dMutexIndex = i;
  }

  // If we don't have too many threads yet
  if(this->dThreadCount >= THREAD_MAX_COUNT)
    return -1;

  // If the mutex is invalid
  if(dMutexIndex >= this->dMutexCount)
    return -1;

  // The data mutex is the mutex at the specified index
  // Also, we have a new state mutex specifically for the new thread (these are not saved in the array of mutexes)
  // We have to lock this immediately to prevent the thread from terminating upon creation
  Mutex *pStateMutex = Mutex_create(sName);
  Mutex *pDataMutex = this->pMutexArray[dMutexIndex];
  Mutex_lock(pStateMutex);

  // Create then save the thread and its state mutex
  Thread *pThread = Thread_create(sName, pStateMutex, pDataMutex, pCallee, pArgs);
  this->pThreadArray[dIndex] = pThread;
  this->pStateArray[dIndex] = pStateMutex;

  // Return the index of the created thread
  return this->dThreadCount++;
}

/**
 * Creates a new mutex object instance and adds it to the array.
 * Note that once a mutex has been created, it cannot be destroyed unless we wish to destroy
 *    all the mutexes. This is a safety feature to prevent any undefined behaviour. (We don't
 *    have a function for the ThreadManager struct that deletes a single Mutex).
 * 
 * Returns the index of the mutex within that array. 
 * Returns -1 if there are too many mutexes already.
 * 
 * @param   { ThreadManager * }   this    A reference to the thread manager object.
 * @param   { char * }            sName   The identifier for the mutex.
*/
int ThreadManager_createMutex(ThreadManager *this, char *sName) {

  // If we don't have too many mutexes yet
  if(this->dMutexCount >= MUTEX_MAX_COUNT)
    return -1;

  // Create and save the new mutex
  Mutex *pMutex = Mutex_create(sName);
  this->pMutexArray[this->dMutexCount] = pMutex;
  
  // Return the index of the created mutex
  return this->dMutexCount++;
}

/**
 * Terminates the thread with the given index in the array.
 * Also deallocates the memory associated with its object.
 * 
 * Note that what happens here is the following:
 *    (1) The function releases the state mutex associated with the thread.
 *    (2) The pointer to the thread is then deleted within the array (although the instance is still in memory).
 *    (3) The thread, upon realizing that its state mutex is free, cleans up after itself and deletes its instance.
 *          It does this by using the Thread_kill() function, which it calls thereafter.
 * 
 * @param   { ThreadManager * }   this          A reference to the thread manager object.
 * @param   { char * }            sThreadName   The name of thread to be terminated.
*/
void ThreadManager_killThread(ThreadManager *this, char *sThreadName) {
  int i, dThreadId = 0, dThreadLast = this->dThreadCount;

  // Look for the index of the thread
  for(i = 0; i < dThreadLast && dThreadLast <= THREAD_MAX_COUNT; i++) {

    // If its null, we skip it and check one more thread
    if(this->pThreadArray[i] == NULL) {
      dThreadLast++;

    // We found the thread
    } else if(!strcmp(this->pThreadArray[i]->sName, sThreadName)) {
      dThreadId = i;
      i = dThreadLast;
    }
  }

  // Release the mutex associated with the thread
  // This automatically kills the thread afterwards, since Thread_kill is called when the 
  //    thread routine terminates
  Mutex_unlock(this->pStateArray[dThreadId]);

  // Update the array
  this->pThreadArray[dThreadId] = NULL;
  this->pStateArray[dThreadId] = NULL;

  // Shorten the length of the list
  // We can execute this before the loop because of the i + 1
  this->dThreadCount--;
}

/**
 * Locks the mutex with a given id.
 * Note that this function does not terminate until it gets a handle to the mutex.
 * 
 * @param   { ThreadManager * }   this        A reference to an instance of ThreadManager to modify.
 * @param   { int }               dMutexId    The id of the thread whose mutex we will lock.
*/
void ThreadManager_LockMutex(ThreadManager *this, int dMutexId) {
  Mutex_lock(this->pMutexArray[dMutexId]);
}

/**
 * Unlocks the data mutex of the thread with a given index.
 * 
 * @param   { ThreadManager * }   this        A reference to an instance of ThreadManager to modify.
 * @param   { int }               dMutexId    The id of the thread whose mutex we will unlock.
*/
void ThreadManager_UnlockMutex(ThreadManager *this, int dMutexId) {
  Mutex_unlock(this->pMutexArray[dMutexId]);
}

#endif