/**
 * @ Author: MMMM
 * @ Create Time: 2024-01-29 17:00:34
 * @ Modified time: 2024-02-28 17:59:45
 * @ Description:
 * 
 * The main game file.
 */

#include "./engine.c"
#include "game/field.obj.h"

#include "utils/utils.animation.h"
#include "utils/utils.hashmap.h"
#include "utils/utils.graphics.h"
#include "utils/utils.thread.h"
#include "utils/utils.buffer.h"
#include "utils/utils.grid.h"
#include "utils/utils.io.h"

#include <stdio.h>

int main() {

  ThreadManager tm;
  ThreadManager_init(&tm);

  ThreadManager_createMutex(&tm, "mutex");

  while(1);

  return 0;
}