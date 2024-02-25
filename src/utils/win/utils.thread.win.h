/**
 * @ Author: MMMM
 * @ Create Time: 2024-02-05 11:18:06
 * @ Modified time: 2024-02-25 13:19:23
 * @ Description:
 * 
 * A utility library for implementing threads in Windows.
 * Note that:
 *    (1) hThread is a handle to the actual thread while
 *    (2) pThread is a pointer to an instance of the Thread class
 */

#ifndef UTILS_THREAD_WIN_
#define UTILS_THREAD_WIN_

#include <windows.h>
#include <conio.h>
#include <process.h>

#define MUTEX_MAX_COUNT 16                              // Maximum number of mutexes we can have for our program
#define THREAD_MAX_COUNT 16                             // Maximum number of threads we can have for our program

#define THREAD_FRAME_RATE 30                            // Number of frames per second
#define THREAD_TIMEOUT (long) 1000 / THREAD_FRAME_RATE  // Length of the execution cycle (in ms) per thread
                                                        // Note that it has to be a long because the Windows API
                                                        //    that deals with the timeout only accepts numbers of 
                                                        //    type long for some reason

/**
 * //
 * ////
 * //////    Mutex class
 * ////////
 * ////////// 
*/

/**
 * A class that wraps around the Windows implementation of mutexes.
 * 
 * @class
*/
typedef struct Mutex {
  
  char *sName;              // The name of the mutex
  void *hMutex;             // A handle to the actual mutex

} Mutex;

/**
 * Allocates memory for a new instance of the mutex class.
 * 
 * @return  { Mutex * }   A pointer to the location of the allocated space.
*/
Mutex *Mutex_new() {
  Mutex *pMutex = calloc(1, sizeof(*pMutex));

  return pMutex;
}

/**
 * Initializes the instance of the mutex class we pass to the function.
 * 
 * @param   { Mutex * }   this    The instance to initialize.
 * @param   { char * }    sName   An identifier or the mutex.
 * @return  { Mutex * }           The initialized instance.
*/
Mutex *Mutex_init(Mutex *this, char *sName) {
  this->sName = sName;
  this->hMutex = CreateMutexA(NULL, FALSE, NULL);

  return this;
}

/**
 * Creates an initialized mutex instance.
 * 
 * @param   { char * }  sName   The identifier for the instance.
*/
Mutex *Mutex_create(char *sName) {
  return Mutex_init(Mutex_new(), sName);
}

/**
 * Frees the memory to an instance of the mutex class.
 * 
 * @param   { Mutex * }   this  The instance to destroy.
*/
void Mutex_kill(Mutex *this) {
  if(this->hMutex)
    CloseHandle(this->hMutex);

  free(this);
}

/**
 * Locks a mutex.
 * Note that this function will wait indefinitely until it is able to lock the mutex.
 * 
 * @param   { Mutex * }   this  A reference to the mutex we wish to lock.
*/
void Mutex_lock(Mutex *this) {
  WaitForSingleObject(this->hMutex, INFINITE);
}

/**
 * Locks a mutex unless a timeout is reached.
 * Note that this function will wait only until the specified amount of time.
 * 
 * @param   { Mutex * }   this  A reference to the mutex we wish to lock.
 * @param   { int }             Returns whether or not the timeout happened first or the mutex was locked.
*/
int Mutex_lockTimed(Mutex *this, int dMillis) {
  return WaitForSingleObject(this->hMutex, dMillis);
}

/**
 * Frees a mutex and makes it available for other threads.
 * Assumes that the caller is the one who currently holds the mutex.
 * 
 * @param   { Mutex * }   this  A reference to the mutex we wish to unlock.
*/
void Mutex_unlock(Mutex *this) {
  ReleaseMutex(this->hMutex);
}

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

  char *sName;                // The name of the thread
                              // TBH, this is only here for convenience and debugging

  void *hThread;              // A handle to the actual thread instance
  Mutex *pStateMutex;         // A pointer to the mutex that tells the thread to keep running
  Mutex *pDataMutex;          // A pointer to the mutex that tells the thread if it can 
                              //    modify the shared resource

  f_void_callback fCallee;    // A pointer to the routine to be run by the thread
  p_obj pArgs_ANY;            // The arguments to the callee
  int tArg_ANY;               // An optinal argument to the callee

} Thread;

/**
 * Constructors and destructors
*/
Thread *Thread_new();

Thread *Thread_init(Thread *this, char *sName, Mutex *pStateMutex, Mutex *pDataMutex, f_void_callback fCallee, p_obj pArgs_ANY, int tArg_ANY);

Thread *Thread_create(char *sName, Mutex *pStateMutex, Mutex *pDataMutex, f_void_callback fCallee, p_obj pArgs_ANY, int tArg_ANY);

void Thread_kill(Thread *this);

/**
 * Helper function for callbacks
 * 
 * NOTE THAT 
 *    on Windows, this function should return void
 *    on Unix, it should return void *
*/
void ThreadHandler(void *pThread);

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
 * @param   { Thread * }          this          A pointer to the thread object to be initialized.
 * @param   { char * }            sName         The name of the thread instance.
 * @param   { Mutex * }           pStateMutex   A pointer to the state mutex.
 * @param   { Mutex * }           pDataMutex    A pointer to the data mutex.
 * @param   { f_void_callback }   fCallee       A pointer to the callback to be executed by the thread.
 * @param   { p_obj }             pArgs_ANY     A pointer to the arguments to be passed to the callback.
 * @param   { int }               tArg_ANY      An argument that might be needed by the callback function.
 * @return  { Thread * }                        A pointer to the initialized thread object.
*/
Thread *Thread_init(Thread *this, char *sName, Mutex *pStateMutex, Mutex *pDataMutex, f_void_callback fCallee, p_obj pArgs_ANY, int tArg_ANY) {
  
  // Update its name
  this->sName = sName;

  // Store the references to the mutex
  this->pStateMutex = pStateMutex;
  this->pDataMutex = pDataMutex;

  // Store the callback and its argument object
  this->fCallee = fCallee;
  this->pArgs_ANY = pArgs_ANY;
  this->tArg_ANY = tArg_ANY;

  // Spawn a new thread
  this->hThread = (Thread *) _beginthread(ThreadHandler, 0, this);

  return this;
}

/**
 * Creates a new thread object with initialized parameters.
 * Returns the initialized instance.
 * 
 * @param   { char * }            sName         The name of the thread instance.
 * @param   { Mutex * }           pStateMutex   A pointer to the state mutex.
 * @param   { Mutex * }           pDataMutex    A pointer to the data mutex.
 * @param   { f_void_callback }   fCallee       A pointer to the callback to be executed by the thread.
 * @param   { p_obj }             pArgs_ANY     A pointer to the arguments to be passed to the callback.
 * @param   { int }               tArg_ANY      An argument that might be needed by the callback function.
 * @return  { Thread * }                        A pointer to the initialized thread object.
*/
Thread *Thread_create(char *sName, Mutex *pStateMutex, Mutex *pDataMutex, f_void_callback fCallee, p_obj pArgs_ANY, int tArg_ANY) {
  return Thread_init(Thread_new(), sName, pStateMutex, pDataMutex, fCallee, pArgs_ANY, tArg_ANY);
}

/**
 * Terminates a thread then destroys a Thread object instance and frees the memory of the object.
*/
void Thread_kill(Thread *this) {

  // Release its mutex first
  Mutex_unlock(this->pDataMutex);

  // Free its state mutex too
  Mutex_kill(this->pStateMutex);

  // No need to wait for thread to terminate
  // Deallocate the object instance
  free(this);
}

/**
 * //
 * ////
 * //////    Callback template
 * ////////
 * ////////// 
*/

/**
 * Executes the callback function assigned to the thread.
 * 
 * @param   { void * }  pThread   A reference to the Thread object whose info we need.
*/
void ThreadHandler(void *pThread) {

  // Note we have to do this because _beginthread expects a function of type void (*)(void *)
  Thread *this = (Thread *) pThread;

  do {
    
    // Wait for it to be able to modify data
    Mutex_lock(this->pDataMutex);

    // Call the callback function
    this->fCallee(this->pArgs_ANY, this->tArg_ANY);

    // Release the mutex
    Mutex_unlock(this->pDataMutex);

  // While the state mutex hasn't been released, keep running the thread
  } while(Mutex_lockTimed(this->pStateMutex, THREAD_TIMEOUT) == WAIT_TIMEOUT);

  // Perform cleanup
  Thread_kill(this);

  return;
}

#endif