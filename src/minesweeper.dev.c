#include "utils/utils.graphics.h"
#include "utils/utils.thread.h"
#include "utils/utils.buffer.h"
#include "utils/utils.grid.h"
#include "utils/utils.io.h"

#include <stdio.h>

void dummy(void *pArgs) {
  printf("This is printing from a thread\n");
}

void dummy2(void *pArgs) {
  printf("This is printing from another thread\n");
}

int main() {

  // Some IO stuff
  IO io;
  IO_init(&io);
  IO_setSize(60, 30);
  
  // Thread pool
  ThreadPool threadPool;
  ThreadPool_init(&threadPool);

  // printf("hmmm\n");
  // int threadId = ThreadPool_createThread(&threadPool, "test", NULL, dummy, NULL);
  // int threadId2 = ThreadPool_createThread(&threadPool, "test2", NULL, dummy2, NULL);

  // Buffers
  Buffer *test = Buffer_create(IO_getWidth(), IO_getHeight());

  // Buffer_append(test, "hello%s%s world!", Graphics_getCodeFG(0x212121), Graphics_getCodeBG(0xf0f0f0));
  // Buffer_append(test, "another one %s%shahahaha", Graphics_getCodeFG(0x123456), Graphics_getCodeBG(0x654321));
  // Buffer_append(test, "what thefuck%s%sim so cool", Graphics_getCodeFG(0xaa11ff), Graphics_getCodeBG(0x121212));

  Grid *pGrid = Grid_create(15, 10);

  for(int i = 0; i < pGrid->dHeight; i++) {
    char string[65] = "";
    char bit[2] = "";

    for(int j = 0; j < pGrid->dWidth; j++) {
      if(i == j || i == j * 2 % pGrid->dWidth)
        Grid_setBit(pGrid, j, i, 1);

      sprintf(bit, "%d", Grid_getBit(pGrid, j, i));
      strcat(string, bit);
    }

    Buffer_append(test, string);
  }

  Buffer_print(test);
  
  #ifndef _WIN32
    do{}while(1);
    #include<time.h>
    usleep(1000000);
  #else
    do {} while(_getch() != 'q');
  #endif

  // ThreadPool_killThread(&threadPool, threadId);

  IO_exit(&io);

  return 0;
}