/**
 * A utility library for implementing threads in Windows.
*/

#ifndef UTILS_THREAD_WIN
#define UTILS_THREAD_WIN

#include "../utils.types.h"
#include "../utils.vector.h"

#include <windows.h>
#include <conio.h>
#include <process.h>

#define THREAD_MAX_COUNT 16                             // Maximum number of threads we can have for our program
#define THREAD_FRAME_RATE 30                            // Number of frames per second
#define THREAD_TIMEOUT (long) 1000 / THREAD_FRAME_RATE  // Length of the execution cycle (in ms) per thread
                                                        // Note that it has to be a long because the Windows API
                                                        //    that deals with the timeout only accepts numbers of 
                                                        //    type long for some reason

/**
 * //
 * ////
 * //////    ThreadCallback struct
 * ////////
 * ////////// 
*/

/**
 * A struct for storing information about the callback to a thread.
 * We need to do this so we can call the callback within a timeout loop
 *    that executes every THREAD_TIMEOUT milliseconds.
 * @struct
*/
typedef struct ThreadCallback {

  ParamFunc pCallback;  // The callback of the thread
  ParamObj pArgs;       // The arguments to the callback

} ThreadCallback;

/**
 * The function represents the timeout function we input into a thread, with a ThreadCallback struct as 
 *    its argument. Note that I did not use ParamObj as a type for pThreadCallback because it would 
 *    be best to use it only for the properties of the ThreadCallback struct itself.
 * 
 * @param   { HandleMutex }   hStateMutex       A mutex that prevents threads from overrunning each other.
 * @param   { void * }        pThreadCallback   An object containing details about what the thread should execute.
*/
void ThreadTimeoutCallback(void *pThreadCallback) {
  do {

    // ! Fix the goddamn errors here
    ((ThreadCallback *) pThreadCallback)->pCallback(
      ((ThreadCallback *) pThreadCallback)->pArgs);


    // ! Change this while condition to a timeout every few milliseconds
  } while(1);
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

  char *sName;              // The name of the thread
                            // TBH, this is only here for convenience and debugging

  HandleThread hThread;     // A handle to the actual thread instance
  HandleMutex hStateMutex;  // A handle to the state mutex stored in the thread pool struct
  HandleMutex hDataMutex;   // A handle to the shared data mutex

} Thread;

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
 * @param   { ParamFunc }     pCallback     A pointer to the callback to be executed by the thread.
 * @param   { ParamObj }      pArgs         A pointer to the arguments to be passed to the callback.
 * @return  { Thread * }                    A pointer to the initialized thread object.
*/
Thread *Thread_init(Thread *this, char *sName, HandleMutex hStateMutex, HandleMutex hDataMutex, ParamFunc pCallback, ParamObj pArgs) {
  
  // Update its name
  this->sName = sName;

  // Store the references to the pertinent mutexes first
  this->hStateMutex = hStateMutex;
  this->hDataMutex = hDataMutex;

  // Spawn a new thread
  this->hThread = (HandleThread) _beginthread(pCallback, 0, pArgs);

  return this;
}

/**
 * Creates a new thread object with initialized parameters.
 * Returns the initialized instance.
 * 
 * @param   { char * }        sName         The name of the thread instance.
 * @param   { HandleMutex }   hStateMutex   A handle to the state mutex.
 * @param   { HandleMutex }   hDataMutex    A handle to the data mutex.
 * @param   { ParamFunc }     pCallback     A pointer to the callback to be executed by the thread.
 * @param   { ParamObj }      pArgs         A pointer to the arguments to be passed to the callback.
 * @return  { Thread * }                    A pointer to the initialized thread object.
 * @return  { Thread * }        A pointer to the initialized thread object.
*/
Thread *Thread_create(char *sName, HandleMutex hStateMutex, HandleMutex hDataMutex, ParamFunc pCallback, ParamObj pArgs) {
  return Thread_init(Thread_new(), sName, hStateMutex, hDataMutex, pCallback, pArgs);
}

/**
 * Destroys a thread instance and frees the memory of the object.
 * 
 * ! actually terminate the thread and what not
 * ! search for counterpart to _beginthread ?
*/
void Thread_kill(Thread *this) {
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
  
  // These are the pertinent mutexes for the thread pool
  // A mutual exclusion (mutex) prevents two different threads from modifying 
  //    a shared resource at the same time. Such "races" can be quite a problem
  //    if not handled correctly.
  // ! remove the state mutex?? I dont think they need to execute one after the other,, they can overlap
  HandleMutex hStateMutex;                  // Describes the state of the thread pool (tells threads to keep running)
  HandleMutex hDataMutex;                   // Refers to whether or not the data resource shared by the threads is free

  Thread *pThreadsArray[THREAD_MAX_COUNT];  // Stores references to all the threads
  int dThreadsCount;                        // Stores the length of the threads array

} ThreadPool;

/**
 * Initializes the thread pool variables.
 * 
 * @param   { ThreadPool * }  this  The thread pool to be initialized.
 * @return  { ThreadPool * }        The initialized thread pool struct.
*/
ThreadPool *ThreadPool_init(ThreadPool *this) {

  // The mutexes
  this->hStateMutex = CreateMutexA(NULL, FALSE, NULL);
  this->hDataMutex = CreateMutexA(NULL, FALSE, NULL);

  // The array
  this->dThreadsCount = 0;

  printf("hello world");

  return this;
}

/**
 * Cleans up the state of the thread pool.
 * 
 * @param   { ThreadPool * }  The thread pool to exit.
*/
void ThreadPool_exit(ThreadPool *this) {

  // Make sure no thread is holding onto the mutex anymore
  ReleaseMutex(this->hStateMutex);

  // Kill all the threads first
  while(--this->dThreadsCount) {

    // Wait for the thread to die, then deallocate the thread object 
    WaitForSingleObject(this->pThreadsArray[this->dThreadsCount]->hThread, INFINITE);
    Thread_kill(this->pThreadsArray[this->dThreadsCount]);
  }
  
  // Reset the thread counter
  this->dThreadsCount = 0;

  // Close the mutexes
  if(this->hStateMutex) CloseHandle(this->hStateMutex);
}

/**
 * Creates a new thread object instance and adds it to the array.
 * Returns the index of the thread within that array. 
 * Returns -1 if there are too many threads already.
 * 
 * @param   { ThreadPool * }  this          A reference to the thread manager object.
 * @param   { char * }        sName         The name of the thread instance.
 * @param   { ParamFunc }     pCallback     A pointer to the callback to be executed by the thread.
 * @param   { ParamObj }      pArgs         A pointer to the arguments to be passed to the callback
 * @return  { int }                         The index of the created thread within the array of the manager.
*/
int ThreadPool_createThread(ThreadPool *this, char *sName, ParamFunc pCallback, ParamObj pArgs) {
  Thread *pThread;

  if(this->dThreadsCount >= THREAD_MAX_COUNT)
    return -1;

  // Create a new thread
  // !CHANGE THE NULLS HERE
  pThread = Thread_create(sName, this->hStateMutex, this->hDataMutex, pCallback, pArgs);

  // Store the thread in its array
  this->pThreadsArray[this->dThreadsCount] = pThread;
  
  // Return the index
  return ++this->dThreadsCount;
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

  // ! fix this function first

  // Kill the thread instance first
  Thread_kill(this->pThreadsArray[dThreadId]);

  // Shorten the length of the list
  // We can execute this before the loop because of the i + 1
  this->dThreadsCount--;

  // Update the array
  for(i = dThreadId; i < this->dThreadsCount; i++) {
    this->pThreadsArray[i] = this->pThreadsArray[i + 1];
  }
}

#endif