/**
 * @ Author: MMMM
 * @ Create Time: 2024-01-29 17:00:34
 * @ Modified time: 2024-03-04 12:31:52
 * @ Description:
 * 
 * The main game file.
 */

#include "./engine.c"
#include "game/field.obj.h"

#include "utils/utils.asset.h"
#include "utils/utils.file.h"
#include "utils/utils.debug.h"
#include "utils/utils.runner.h"
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

  // IO io;
  // IO_init(&io);
  // IO_exit(&io);

  char *test[10] = {
    "Hello world",
    "ello worldH",
    "Hello world",
    "ello worldH",
    "Hello world",
    "ello worldH",
    "Hello world",
    "ello worldH",
    "Hello world",
    "ello worldH",
  };

  AssetManager am;
  AssetManager_init(&am);

  AssetManager_createAsset(&am, "test", 10, test);
  AssetManager_readAssetFile(&am, "//", "./src/assets/title-font.asset.txt");

  printf("heloo dord");

  char **testOutput = AssetManager_getAssetText(&am, "header-font-m");

  for(int i = 0; i < 6; i++)
    printf("\n%s", testOutput[i]);

  while(1);

  return 0;
}