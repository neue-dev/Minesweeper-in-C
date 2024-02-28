/**
 * @ Author: MMMM
 * @ Create Time: 2024-01-29 17:00:34
 * @ Modified time: 2024-02-28 17:03:03
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

  HashMap *pMyHashMap = HashMap_create();

  for(int i = 0; i < 8; i++) {
    char *sKey = calloc(16, 1);
    sprintf(sKey, "%dasd%d", i, i);
    
    Animation *pTest = Animation_create(sKey, NULL, 0);
    HashMap_createEntry(pMyHashMap, sKey, pTest);
  }

  for(int i = 0; i < pMyHashMap->dEntryCount; i++) {
    char *sKey = calloc(16, 1);
    sprintf(sKey, "%dasd%d", i, i);

    if(i % 2) {
      Animation *pTest = Animation_create("test", NULL, 0);
      HashMap_setEntry(pMyHashMap, sKey, pTest);  
    }

    Animation *pTest = (Animation *) HashMap_getEntry(pMyHashMap, sKey);
    printf("key: %s\n", sKey);
    printf("nam: %s\n", pTest->sName);
  }

  printf("%d\n", pMyHashMap->dEntryMaxSlots);

  printf("hello");

  while(1);

  return 0;
}