/**
 * @ Author: MMMM
 * @ Create Time: 2024-01-29 12:01:16
 * @ Modified time: 2024-03-08 10:17:35
 * @ Description:
 */

#include "utils/utils.debug.h"

#include "./engine.c"
#include "game/field.obj.h"

#include "utils/utils.io.h"

#include <stdio.h>

int main() {

  // Debug_clearLogs();

  // Some IO stuff
  IO io;
  IO_init(&io);

  // Set the console size
  IO_setSize(128, 40);
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