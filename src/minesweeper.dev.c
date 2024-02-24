/**
 * @ Author: Mo David
 * @ Create Time: 2024-01-29 17:00:34
 * @ Modified time: 2024-02-24 14:09:42
 * @ Description:
 * 
 * The main game file.
 */

#include "game/field.obj.h"

#include "utils/utils.graphics.h"
#include "utils/utils.thread.h"
#include "utils/utils.buffer.h"
#include "utils/utils.grid.h"
#include "utils/utils.io.h"

#include <stdio.h>

void dummy(p_obj pArgs) {
  printf("This is printing from a thread\n");
}

void dummy2(p_obj pArgs) {
  printf("This is printing from another thread\n");
}

int main() {

  // Some IO stuff
  IO io;
  IO_init(&io);
  IO_setSize(60, 30);
  
  // Thread pool
  ThreadManager threadManager;
  ThreadManager_init(&threadManager);

  printf("hmmm\n");
  int mutexId = ThreadManager_createMutex(&threadManager, "hello-mutex");
  int mutexId2 = ThreadManager_createMutex(&threadManager, "hello2-mutex");
  int threadId = ThreadManager_createThread(&threadManager, "hello", "hello-mutex", dummy, NULL);
  int threadId2 = ThreadManager_createThread(&threadManager, "hello2", "hello2-mutex", dummy2, NULL);

  // // Buffers
  // Buffer *test = Buffer_create(IO_getWidth(), IO_getHeight());

  // // Buffer_append(test, "hello%s%s world!", Graphics_getCodeFG(0x212121), Graphics_getCodeBG(0xf0f0f0));
  // // Buffer_append(test, "another one %s%shahahaha", Graphics_getCodeFG(0x123456), Graphics_getCodeBG(0x654321));
  // // Buffer_append(test, "what thefuck%s%sim so cool", Graphics_getCodeFG(0xaa11ff), Graphics_getCodeBG(0x121212));

  // Field field;
  // // Grid *pGrid = Grid_create(15, 10);
  // // Grid_clear(pGrid, 1);
  // // Grid_clear(pGrid, 0);

  // Field_init(&field, 15, 10);
  // Field_populate(&field, 30);

  // for(int i = 0; i < field.dHeight; i++) {
  //   char string[65] = "";
  //   char bit[2] = "";

  //   for(int j = 0; j < field.dWidth; j++) {
  //     sprintf(bit, "%d", Grid_getBit(field.pMineGrid, j, i));
  //     strcat(string, bit);
  //   }

  //   Buffer_append(test, string);
  // }

  // // Buffer_print(test);
  
  #ifndef _WIN32
    int i = 99999999;
    do{}while(--i);
    ThreadManager_killThread(&threadManager, "hello");  
    i = 99999999;
    do{}while(1);
  #else
    do {} while(_getch() != 'q');
    ThreadManager_killThread(&threadManager, "hello");
    do {} while(_getch() != 'q');
  #endif

  IO_exit(&io);

  return 0;
}