/**
 * @ Author: MMMM
 * @ Create Time: 2024-02-05 11:19:47
 * @ Modified time: 2024-02-20 11:42:56
 * @ Description:
 *    
 * A utility library for implementing threads in Unix-based systems.
 * Note that:
 *    (1) hThread is a handle to the actual thread while
 *    (2) pThread is a pointer to an instance of the Thread class
 */

#ifndef UTILS_THREAD_UNIX
#define UTILS_THREAD_UNIX

#include "../utils.types.h"
#include "../utils.vector.h"

#include <pthread.h>
#include <time.h>

/**
 * //
 * ////
 * //////    Thread class
 * ////////
 * ////////// 
*/

/**
 * The thread class stores information that describes a thread.
 * It helps abstract some of the finer details of implementing threads.
 * @class
*/
typedef struct Thread {

  char *sName;              // The name of the thread
                            // TBH, this is only here for convenience and debugging

  HandleThread hThread;     // A handle to the actual thread instance
  HandleMutex hStateMutex;  // A handle to the mutex that tells the thread to keep running
  HandleMutex hDataMutex;   // A handle to the mutex that tells the thread if it can 
                            //    modify the shared resource

  ParamFunc pCallee;        // A pointer to the routine to be run by the thread
  ParamObj pArgs;           // The arguments to the callee

} Thread;

/**
 * Executes the callback function assigned to the thread.
 * 
 * @param   { void * }  pThread   A reference to the Thread object whose info we need.
*/
void *Thread_caller(void *pThread) {

  // Note we have to do this because pthread_create expects a function of type void (*)(void *)
  Thread *this = (Thread *) pThread;

  do {

    // Try to lock the mutex first
    do {} while(pthread_mutex_trylock(this->hDataMutex));

    // Call the callback function
    this->pCallee(this->pArgs);

    // Unlock the mutex after use
    pthread_mutex_unlock(this->hDataMutex);

  // While the state mutex hasn't been released, keep running the thread
  // I find it kinda funny how usleep() means "microsleep" (counts in microseconds)
  //    Also just found out that usleep() is deprecated and now Unix prefers nanosleep()
  //    like WTF?? How reliable would specifying nanoseconds be within a program runtime 
  //    environment? Anyway, that kind of precision is overkill so I used usleep() (they 
  //    haven't borked it so I can)
  } while(!usleep(THREAD_TIMEOUT * 1000) && pthread_mutex_trylock(this->hStateMutex));

  return NULL;
}

/**
 * Allocates memory for a new thread object instance.
 * Note that this does not allocate memory for a thread but rather a *thread object*.
 *    The thread object refers to the struct above, which stores information about
 *    the thread, not the actual memory needed by the thread.
 * The function returns NULL when the allocation fails.
 * 
 * @return  { Thread * }  A pointer to the newly created thread object.
*/
Thread *Thread_new() {
  Thread *pThread = calloc(1, sizeof(*pThread));
  return pThread;
}

/**
 * Initializes an instance of the thread class.
 * Returns the initialized instance.
 * 
 * @param   { Thread * }      this          A pointer to the thread object to be initialized.
 * @param   { char * }        sName         The name of the thread instance.
 * @param   { HandleMutex }   hStateMutex   A handle to the state mutex.
 * @param   { HandleMutex }   hDataMutex    A handle to the data mutex.
 * @param   { ParamFunc }     pCallee       A pointer to the callback to be executed by the thread.
 * @param   { ParamObj }      pArgs         A pointer to the arguments to be passed to the callback.
 * @return  { Thread * }                    A pointer to the initialized thread object.
*/
Thread *Thread_init(Thread *this, char *sName, HandleMutex hStateMutex, HandleMutex hDataMutex, ParamFunc pCallee, ParamObj pArgs) {
  
  // Update its name
  this->sName = sName;

  // Store the references to the mutex
  this->hStateMutex = hStateMutex;
  this->hDataMutex = hDataMutex;

  // Store the callback and its argument object
  this->pCallee = pCallee;
  this->pArgs = pArgs;

  // Spawn a new thread
  this->hThread = calloc(1, sizeof(*this->hThread));
  pthread_create(this->hThread, NULL, Thread_caller, this);

  return this;
}

/**
 * Creates a new thread object with initialized parameters.
 * Returns the initialized instance.
 * 
 * @param   { char * }        sName         The name of the thread instance.
 * @param   { HandleMutex }   hStateMutex   A handle to the state mutex.
 * @param   { HandleMutex }   hDataMutex    A handle to the data mutex.
 * @param   { ParamFunc }     pCallee       A pointer to the callback to be executed by the thread.
 * @param   { ParamObj }      pArgs         A pointer to the arguments to be passed to the callback.
 * @return  { Thread * }                    A pointer to the initialized thread object.
*/
Thread *Thread_create(char *sName, HandleMutex hStateMutex, HandleMutex hDataMutex, ParamFunc pCallee, ParamObj pArgs) {
  return Thread_init(Thread_new(), sName, hStateMutex, hDataMutex, pCallee, pArgs);
}

/**
 * Terminates a thread then destroys a Thread object instance and frees the memory of the object.
*/
void Thread_kill(Thread *this) {

  // Release its mutexes first
  pthread_mutex_unlock(this->hStateMutex);
  pthread_mutex_unlock(this->hDataMutex);

  // No need to wait for the thread to terminate
  // Deallocate the object instance
  free(this);
}

/**
 * //
 * ////
 * //////    ThreadPool struct
 * ////////
 * ////////// 
*/

/**
 * The ThreadPool struct stores information related to all currently existing threads.
 * If ever we want to do anything involving threads, we must interact with this struct 
 *    instead of calling any of the Thread methods defined above. 
 * It allows us to do this without having to pollute the global namespace with variables.
 * @struct
*/
typedef struct ThreadPool {

  Thread *pThreadsArray[THREAD_MAX_COUNT];            // Stores references to all the threads

  // A mutual exclusion (mutex) prevents two different threads from modifying 
  //    a shared resource at the same time. Such "races" can be quite a problem
  //    if not handled correctly.
  HandleMutex hStateMutexesArray[THREAD_MAX_COUNT];   // Stores the mutexes that tell each thread to keep running
  HandleMutex hDataMutexesArray[THREAD_MAX_COUNT];    // Stores the mutexes that tell each thread if it can modify its resource
  int dThreadsCount;                                  // Stores the length of the threads array

} ThreadPool;

/**
 * Initializes the thread pool variables.
 * 
 * @param   { ThreadPool * }  this  The thread pool to be initialized.
 * @return  { ThreadPool * }        The initialized thread pool struct.
*/
ThreadPool *ThreadPool_init(ThreadPool *this) {

  // Set the array size to 0
  this->dThreadsCount = 0;

  return this;
}

/**
 * Cleans up the state of the thread pool.
 * 
 * @param   { ThreadPool * }  The thread pool to exit.
*/
void ThreadPool_exit(ThreadPool *this) {

  // Kill all the threads first
  while(--this->dThreadsCount) {

    // Kill the current thread
    Thread_kill(this->pThreadsArray[this->dThreadsCount]);
  }
  
  // Reset the thread counter
  this->dThreadsCount = 0;
}

/**
 * Creates a new thread object instance and adds it to the array.
 * Returns the index of the thread within that array. 
 * Returns -1 if there are too many threads already.
 * 
 * @param   { ThreadPool * }  this              A reference to the thread manager object.
 * @param   { char * }        sName             The name of the thread instance.
 * @param   { HandleMutex }   hSharedDataMutex  A handle to the mutex that tells the thread whether it can modify its resource.
 * @param   { ParamFunc }     pCallee           A pointer to the callback to be executed by the thread.
 * @param   { ParamObj }      pArgs             A pointer to the arguments to be passed to the callback
 * @return  { int }                             The index of the created thread within the array of the manager.
*/
int ThreadPool_createThread(ThreadPool *this, char *sName, HandleMutex hSharedDataMutex, ParamFunc pCallee, ParamObj pArgs) {
  
  // The stuff to create
  // Because the Unix API and Windows API behave differently, we have to initialize our 
  //    pointers here before being able to pass them into the functions provided by pthread.h
  Thread *pThread = calloc(1, sizeof(*pThread));
  HandleMutex hStateMutex = calloc(1, sizeof(*hStateMutex));
  HandleMutex hDataMutex = calloc(1, sizeof(*hDataMutex));

  // If we don't have too many threads yet
  if(this->dThreadsCount >= THREAD_MAX_COUNT)
    return -1;

  // Create the pertinent mutexes of the thread
  pthread_mutex_init(hStateMutex, NULL);
  if(hSharedDataMutex == NULL) {
    pthread_mutex_init(hDataMutex, NULL);
  } else {
    free(hDataMutex); 
    hDataMutex = hSharedDataMutex;
  }

  // We have to lock it to prevent the thread from terminating
  // The thread only terminates when it (the thread) locks the mutex
  pthread_mutex_trylock(hStateMutex);
  
  // Create the thread and store its mutexes in their arrays
  pThread = Thread_create(sName, hStateMutex, hDataMutex, pCallee, pArgs);
  this->hStateMutexesArray[this->dThreadsCount] = hStateMutex;
  this->hDataMutexesArray[this->dThreadsCount] = hDataMutex;
  this->pThreadsArray[this->dThreadsCount] = pThread;
  
  // Return the index
  return this->dThreadsCount++;
}

/**
 * Terminates the thread with the given index in the array.
 * Also deallocates the memory associated with its object.
 * 
 * @param   { ThreadPool * }  this        A reference to the thread manager object.
 * @param   { int }           dThreadId   The index of thread to be terminated.
*/
void ThreadPool_killThread(ThreadPool *this, int dThreadId) {
  int i;  // Lmao
  
  if(dThreadId >= this->dThreadsCount)
    return;

  // Kill the thread instance
  Thread_kill(this->pThreadsArray[dThreadId]);

  // Shorten the length of the list
  // We can execute this before the loop because of the i + 1
  this->dThreadsCount--;

  // Update the array
  for(i = dThreadId; i < this->dThreadsCount; i++)
    this->pThreadsArray[i] = this->pThreadsArray[i + 1];
}

/**
 * Locks the data mutex of the thread with a given index.
 * Note that this function does not terminate until it gets a handle to the mutex.
 * 
 * @param   { ThreadPool * }  this        A reference to an instance of ThreadPool to modify.
 * @param   { int }           dThreadId   The id of the thread whose mutex we will lock.
*/
void ThreadPool_LockMutex(ThreadPool *this, int dThreadId) {
  do {} while(pthread_mutex_trylock(this->hDataMutexesArray[dThreadId]));
}

/**
 * Unlocks the data mutex of the thread with a given index.
 * 
 * @param   { ThreadPool * }  this        A reference to an instance of ThreadPool to modify.
 * @param   { int }           dThreadId   The id of the thread whose mutex we will unlock.
*/
void ThreadPool_UnlockMutex(ThreadPool *this, int dThreadId) {
  pthread_mutex_unlock(this->hDataMutexesArray[dThreadId]);
}

#endif