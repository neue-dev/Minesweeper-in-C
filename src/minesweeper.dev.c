#include "utils/utils.graphics.h"
#include "utils/utils.thread.h"

#include <stdio.h>

int main() {
  long test;
  Graphics_setForeground(0x111111);
  Graphics_setBackground(0xEEEEEE);
  printf("Lets test this out\n");
  printf("Size of long: %d", sizeof(test));

  return 0;
}