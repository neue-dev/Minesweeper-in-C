/**
 * @ Author: MMMM
 * @ Create Time: 2024-01-29 17:00:34
 * @ Modified time: 2024-03-12 23:24:52
 * @ Description:
 * 
 * The main game file.
 */

#include "./engine.c"
#include "game/field.obj.h"

#include "utils/utils.asset.h"
#include "utils/utils.file.h"
#include "utils/utils.debug.h"
#include "utils/utils.hashmap.h"
#include "utils/utils.graphics.h"
#include "utils/utils.thread.h"
#include "utils/utils.theme.h"
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

  // IO io;
  // IO_init(&io);
  // IO_exit(&io);

  ThemeManager tm;
  ThemeManager_init(&tm);

  color c = ThemeManager_getActive(&tm, "primary-darken-0.5");

  printf("%d, %d, %d", c >> 16, (c >> 8) % (1 << 8), c % (1 << 8));

  while(1);

  return 0;
}