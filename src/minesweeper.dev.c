/**
 * @ Author: MMMM
 * @ Create Time: 2024-01-29 17:00:34
 * @ Modified time: 2024-03-02 00:53:37
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
#include <wchar.h>

int main() {

  // HashMap *pMyHashMap = HashMap_create();

  // for(int i = 0; i < 16; i++) {
  //   char *sKey = calloc(16, 1);
  //   sprintf(sKey, "%dasd%d", i, i);
    
  //   Animation *pTest = Animation_create(sKey, NULL, 0);
  //   HashMap_add(pMyHashMap, sKey, pTest);
  // }
  
  // char *sKeys[16];
  // HashMap_getKeys(pMyHashMap, sKeys);

  // for(int i = 0; i < 16; i++)
  //   printf("%s\n", sKeys[i]);

  IO io;
  IO_init(&io);

  char *testString = "╗█";
  char *c = testString;

  while(*c)
    printf("%d ", (unsigned char) *c >> 6) && c++;

  // Debug_log("%s", "test");

  IO_exit(&io);

  while(1);

  return 0;
}