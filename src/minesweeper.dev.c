#include "utils/utils.graphics.h"
#include "utils/utils.thread.h"

#include <stdio.h>

void dummy(void *test) {
  printf("This is printing from a thread\n");
}

int main() {
  long test;
  Graphics_setForeground(0x111111);
  Graphics_setBackground(0xEEEEEE);
  printf("Lets test this out\n");
  printf("Size of long: %d\n", sizeof(test));

  ThreadPool threadPool;
  ThreadPool_init(&threadPool);
  ThreadPool_createThread(&threadPool, dummy, NULL);

  for(int i = 0; i < 100; i++)
    printf("haha\n");

  do {} while(_getch() != 'q');

  return 0;
}