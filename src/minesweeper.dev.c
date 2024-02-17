#include "utils/utils.graphics.h"
#include "utils/utils.thread.h"
#include "utils/utils.io.h"

#include <stdio.h>

void dummy(void *pArgs) {
  Graphics_setForeground(0xEEEEEE);
  Graphics_setBackground(0x111111);
  printf("This is printing from a thread\n");
}

void dummy2(void *pArgs) {
  Graphics_setForeground(0x111111);
  Graphics_setBackground(0xEEEEEE);
  printf("This is printing from another thread\n");
}

int main() {
  ThreadPool threadPool;
  ThreadPool_init(&threadPool);

  printf("hmmm\n");
  int threadId = ThreadPool_createThread(&threadPool, "test", dummy, NULL);
  int threadId2 = ThreadPool_createThread(&threadPool, "test2", dummy2, NULL);

  #ifndef _WIN32
    do{}while(1);
    #include<time.h>
    usleep(1000000);
  #else
    do {} while(_getch() != 'q');
  #endif

  ThreadPool_killThread(&threadPool, threadId);

  return 0;
}