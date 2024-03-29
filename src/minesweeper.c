/**
 * @ Author: MMMM
 * @ Create Time: 2024-01-29 12:01:16
 * @ Modified time: 2024-03-30 01:04:49
 * @ Description:
 */

#include "./engine.c"
#include "game/field.obj.h"

#include "utils/utils.io.h"

int main() {

  // Some IO stuff
  IO io;
  IO_init(&io);

  // Set the console size
  IO_setSize(132, 36);
  IO_clear();
  
  // Set up the engine
  Engine engine;
  Engine_init(&engine);

  // Keep the main thread open while the engine is running
  while(Engine_getState(&engine));

  // Clean up the IO related stuff
  IO_clear();
  IO_exit(&io);

  // Clean up the engine
  Engine_exit(&engine);

  return 0;
}