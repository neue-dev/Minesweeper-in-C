/**
 * @ Author: MMMM
 * @ Create Time: 2024-01-29 17:00:34
 * @ Modified time: 2024-04-01 06:51:58
 * @ Description:
 * 
 * The main game file.
 */

#include "./engine.c"
#include "game/field.obj.h"
#include "game/game.c"
#include "game/stats.game.c"
#include "game/editor.game.c"

#include "utils/utils.asset.h"
#include "utils/utils.file.h"
#include "utils/utils.hashmap.h"
#include "utils/utils.graphics.h"
#include "utils/utils.thread.h"
#include "utils/utils.theme.h"
#include "utils/utils.grid.h"
#include "utils/utils.io.h"

#include <stdio.h>
#include <wchar.h>
#include <time.h>

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

  Game test;
  Profile profile;
  test.pProfile = &profile;

  Profile_init(&profile);
  Profile_login(&profile, "MODEV", "MOGEN");
  Stats_readProfile(&profile);
  Profile_delete(&profile, "TEST");

  // printf("\n%d %d %d", 
  // Stats_getWins(&profile, GAME_TYPE_CLASSIC, GAME_DIFFICULTY_DIFFICULT),
  // Stats_getLosses(&profile, GAME_TYPE_CLASSIC, GAME_DIFFICULTY_DIFFICULT),
  // Stats_getQuits(&profile, GAME_TYPE_CLASSIC, GAME_DIFFICULTY_DIFFICULT));
  
  // printf("\n%d %d %d", 
  // Stats_getWins(&profile, GAME_TYPE_CLASSIC, GAME_DIFFICULTY_EASY),
  // Stats_getTotalGames(&profile, GAME_TYPE_CLASSIC, GAME_DIFFICULTY_DIFFICULT),
  // Stats_getTotalGames(&profile, GAME_TYPE_CUSTOM, GAME_DIFFICULTY_DIFFICULT));

  // // Profile_register(&profile, "MOD", "KLANG");
  // Profile_init(&profile);
  // strcpy(profile.sCurrentProfile, "MODEV");
  // Stats_readProfile(&profile);
  
  // Editor_setup(&test);
  // Game_setup(&test,GAME_TYPE_CUSTOM, GAME_DIFFICULTY_EASY);
  // Editor_setSaveName(&test, "DEV");
  // Editor_loadLevel(&test);

  // // SleepEx(2000, FALSE);
  // // char *bruh = Game_getTime(&test);

  // int n = 0;
  // char *sOutputBuffer[GAME_MAX_ROWS + 10];
  // Stats_getBoard(&profile, 4, &n, sOutputBuffer);


  // for(int i = 0; i < n; i++)
  //   printf("[%s]", sOutputBuffer[i]);

  while(1);

  return 0;
}