/**
 * @ Author: MMMM
 * @ Create Time: 2024-01-29 17:00:34
 * @ Modified time: 2024-02-26 17:40:17
 * @ Description:
 * 
 * The main game file.
 */

#include "./engine.c"
#include "game/field.obj.h"

#include "utils/utils.graphics.h"
#include "utils/utils.thread.h"
#include "utils/utils.buffer.h"
#include "utils/utils.grid.h"
#include "utils/utils.io.h"

#include <stdio.h>

int main() {

  // Some IO stuff
  IO io;
  IO_init(&io);
  IO_setSize(60, 30);
  
  // Set up the engine
  Engine engine;
  Engine_init(&engine);

  // Keep the main thread open while the engine is running
  while(Engine_getState(&engine)) {};

  // Clean up some stuff
  Engine_exit(&engine);
  IO_exit(&io);

  return 0;
}