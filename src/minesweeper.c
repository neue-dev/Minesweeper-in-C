/**
 * @ Author: MMMM
 * @ Create Time: 2024-01-29 12:01:16
 * @ Modified time: 2024-02-29 22:25:17
 * @ Description:
 */

#include "./engine.c"
#include "game/field.obj.h"

#include "utils/utils.io.h"

#include <stdio.h>

int main() {

  // Some IO stuff
  IO io;
  IO_init(&io);

  // Set the console size
  IO_setSize(128, 32);
  IO_clear();
  
  // Set up the engine
  Engine engine;
  Engine_init(&engine);

  // Keep the main thread open while the engine is running
  while(Engine_getState(&engine)) {};

  // Clean up the engine
  Engine_exit(&engine);

  // Clean up the IO related stuff
  IO_clear();
  IO_exit(&io);

  return 0;
}