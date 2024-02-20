/**
 * @ Author: Mo David
 * @ Create Time: 2024-01-29 12:01:02
 * @ Modified time: 2024-02-20 11:38:27
 * @ Description:
 *    A utility library for implementing threads.
 */

#ifndef UTILS_THREAD
#define UTILS_THREAD

#define THREAD_MAX_COUNT 16                             // Maximum number of threads we can have for our program
#define THREAD_FRAME_RATE 30                            // Number of frames per second
#define THREAD_TIMEOUT (long) 1000 / THREAD_FRAME_RATE  // Length of the execution cycle (in ms) per thread
                                                        // Note that it has to be a long because the Windows API
                                                        //    that deals with the timeout only accepts numbers of 
                                                        //    type long for some reason

// You are in Windows
#ifdef _WIN32
#include "./win/utils.thread.win.h"

// Not in Windows
#else
#include "./unix/utils.thread.unix.h"
#endif

#endif