/**
 * @ Author: MMMM
 * @ Create Time: 2024-01-29 12:01:02
 * @ Modified time: 2024-03-12 22:19:02
 * @ Description:
 *    
 * A utility library for implementing threads.
 */

#ifndef UTILS_THREAD_
#define UTILS_THREAD_

#include "./utils.hashmap.h"
#include "./utils.string.h"
#include "./utils.types.h"

#include <string.h>

// You are in Windows
#ifdef _WIN32
#include "./win/utils.thread.win.h"

// Not in Windows
#else
#include "./unix/utils.thread.unix.h"
#endif

typedef struct ThreadManager ThreadManager;

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
 *    instead of calling any of the Thread methods defined above; this is similar to how the
 *    EventManager abstracts some methods of the Event class.
 * It allows us to do this without having to pollute the global namespace with variables.
 * Note that this also uses the implementations we defined for both Windows and Unix, without
 *    it having to know the stuff they do under the hood. This keeps our code clean. You can think
 *    of this as some sort of "manager" which handles all our thread and mutex instances.
 * 
 * @struct
*/
struct ThreadManager {

  HashMap *pThreadMap;                                // Stores references to all the threads
  HashMap *pStateMap;                                 // For each thread, we have a state mutex to tell it to keep running        
  HashMap *pMutexMap;                                 // Stores references to all the mutexes

  int dThreadCount;                                   // Stores the length of the threads array
  int dMutexCount;                                    // Stores the length of the mutexes array
};

/**
 * Initializes the thread manager variables.
 * 
 * @param   { ThreadManager * }  this  The thread manager to be initialized.
 * @return  { ThreadManager * }        The initialized thread manager struct.
*/
ThreadManager *ThreadManager_init(ThreadManager *this) {

  // Init the different hashmaps
  this->pThreadMap = HashMap_create();
  this->pStateMap = HashMap_create();
  this->pMutexMap = HashMap_create();

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
  char sThreadKey[STRING_KEY_MAX_LENGTH];
  char sMutexKey[STRING_KEY_MAX_LENGTH];
  Mutex *pStateMutex, *pDataMutex;

  // Since the only time we need these is when cleaning up the manager
  //    we don't need to store this as part of the manager's state
  char *sThreadKeyArray[THREAD_MAX_COUNT];
  char *sMutexKeyArray[MUTEX_MAX_COUNT];

  // Store the keys here
  HashMap_getKeys(this->pThreadMap, sThreadKeyArray);
  HashMap_getKeys(this->pMutexMap, sMutexKeyArray);

  // Kill all the threads first
  while(this->dThreadCount) {
    strcpy(sThreadKey, sThreadKeyArray[this->dThreadCount]);
    pStateMutex = HashMap_get(this->pStateMap, sThreadKey);

    // This tells the thread to terminate
    if(pStateMutex != NULL)
      Mutex_unlock(pStateMutex);

    // Get rid of the references
    // We don't "kill" them here because the threads do that on their own automatically
    HashMap_set(this->pThreadMap, sThreadKey, NULL);

    // Decrement the size of the array
    this->dThreadCount--;
  }

  // Kill all the mutexes after killing the threads
  while(this->dMutexCount) {
    strcpy(sMutexKey, sMutexKeyArray[this->dMutexCount]);
    pDataMutex = HashMap_get(this->pMutexMap, sMutexKey);

    // Kill the mutex
    if(pDataMutex != NULL)
      Mutex_kill(pDataMutex);

    // Get rid of the references
    HashMap_set(this->pMutexMap, sMutexKey, NULL);

    // Memory clean up
    String_kill(sMutexKey);

    // Decrement the size of the array
    this->dMutexCount--;
  }

  // Kill all the hashmaps too
  HashMap_kill(this->pThreadMap);
  HashMap_kill(this->pMutexMap);
  HashMap_kill(this->pStateMap);
}

/**
 * Creates a new thread object instance and adds it to the hashmap.
 * 
 * @param   { ThreadManager * }   this              A reference to the thread manager object.
 * @param   { char * }            sThreadKey        The identifier for the thread and its data mutex.
 * @param   { char * }            sMutexKey         The name of the mutex to be associated with.
 * @param   { Mutex * }           pDataMutex        A handle to the mutex that tells the thread whether it can modify its resource.
 * @param   { f_void_callback }   fCallee           A pointer to the callback to be executed by the thread.
 * @param   { p_obj }             pArgs_ANY         A pointer to the arguments to be passed to the callback
 * @param   { int }               tArg_ANY          A parameter that the callback function might need (ie, an enum).
*/
void ThreadManager_createThread(ThreadManager *this, char *sThreadKey, char *sMutexKey, f_void_callback fCallee, p_obj pArgs_ANY, int tArg_ANY) {
  Mutex *pStateMutex = HashMap_get(this->pStateMap, sThreadKey), *pDataMutex;
  Thread *pThread = HashMap_get(this->pThreadMap, sThreadKey);

  // Duplicate key
  if(pThread != NULL)
    return;

  // If we don't have too many threads yet
  if(this->dThreadCount >= THREAD_MAX_COUNT)
    return;

  // The data mutex is the mutex at the specified index
  // Also, we have a new state mutex specifically for the new thread (these are not saved in the array of mutexes)
  // We have to lock this immediately to prevent the thread from terminating upon creation
  pDataMutex = HashMap_get(this->pMutexMap, sMutexKey);

  // If the mutex does not exist
  if(pDataMutex == NULL)
    return;

  // The state mutex keeps the thread alive
  // The thread always checks whether or not this mutex is still locked
  // The moment the mutex unlocks, the thread terminates its routine
  pStateMutex = Mutex_create(sThreadKey);
  Mutex_lock(pStateMutex);

  // Create then save the thread and its state mutex
  pThread = Thread_create(sThreadKey, pStateMutex, pDataMutex, fCallee, pArgs_ANY, tArg_ANY);

  // Save them to the respective hashmaps
  HashMap_add(this->pThreadMap, sThreadKey, pThread);
  HashMap_add(this->pStateMap, sThreadKey, pStateMutex);

  this->dThreadCount++;
}

/**
 * Creates a new mutex object instance and adds it to the hashmap.
 * Note that once a mutex has been created, it cannot be destroyed unless we wish to destroy
 *    all the mutexes. This is a safety feature to prevent any undefined behaviour. (We don't
 *    have a function for the ThreadManager struct that deletes a single Mutex).
 * 
 * @param   { ThreadManager * }   this        A reference to the thread manager object.
 * @param   { char * }            sMutexKey   The identifier for the mutex.
*/
void ThreadManager_createMutex(ThreadManager *this, char *sMutexKey) {
  Mutex *pMutex = HashMap_get(this->pMutexMap, sMutexKey);

  // Duplicate key
  if(pMutex != NULL)
    return;

  // If we don't have too many mutexes yet
  if(this->dMutexCount >= MUTEX_MAX_COUNT)
    return;

  // Create and save the new mutex
  pMutex = Mutex_create(sMutexKey);
  HashMap_add(this->pMutexMap, sMutexKey, pMutex); 

  this->dMutexCount++;
}

/**
 * Terminates the thread with the given index in the hashmap.
 * Also deallocates the memory associated with its object.
 * 
 * Note that what happens here is the following:
 *    (1) The function releases the state mutex associated with the thread.
 *    (2) The pointer to the thread is then deleted within the array (although the instance is still in memory).
 *    (3) The thread, upon realizing that its state mutex is free, cleans up after itself and deletes its instance.
 *          It does this by using the Thread_kill() function, which it calls thereafter.
 * 
 * @param   { ThreadManager * }   this          A reference to the thread manager object.
 * @param   { char * }            sThreadKey    The name of thread to be terminated.
*/
void ThreadManager_killThread(ThreadManager *this, char *sThreadKey) {

  // Release the mutex associated with the thread
  // This automatically kills the thread afterwards, since Thread_kill is called when the 
  //    thread routine terminates
  Mutex_unlock((Mutex *) HashMap_get(this->pStateMap, sThreadKey));

  // Wait for thread to die
  while(HashMap_get(this->pStateMap, sThreadKey) != NULL);

  // Update the hashmap
  HashMap_del(this->pThreadMap, sThreadKey);
  HashMap_del(this->pStateMap, sThreadKey);

  // Shorten the length of the list
  // We can execute this before the loop because of the i + 1
  this->dThreadCount--;
}

/**
 * Locks the mutex with a given key.
 * Note that this function does not terminate until it gets a handle to the mutex.
 * 
 * @param   { ThreadManager * }   this        A reference to an instance of ThreadManager to modify.
 * @param   { char * }            sMutexKey   The name of the mutex we will lock.
*/
void ThreadManager_lockMutex(ThreadManager *this, char *sMutexKey) {
  Mutex *pMutex = HashMap_get(this->pMutexMap, sMutexKey);

  if(HashMap_get(this->pMutexMap, sMutexKey) != NULL)
    Mutex_lock(pMutex);
}

/**
 * Unlocks the data mutex of the thread with a given index.
 * This function returns 1 on success, and 0 on failure.
 * 
 * @param   { ThreadManager * }   this        A reference to an instance of ThreadManager to modify.
 * @param   { char * }            sMutexKey   The name of the mutex we will unlock.
 * @return  { int }                           Whether or not the operation was successful.
*/
void ThreadManager_unlockMutex(ThreadManager *this, char *sMutexKey) {
  Mutex *pMutex = HashMap_get(this->pMutexMap, sMutexKey);

  if(HashMap_get(this->pMutexMap, sMutexKey) != NULL)
    Mutex_unlock(pMutex);
}

#endif