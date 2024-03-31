/**
 * @ Author: MMMM
 * @ Create Time: 2024-02-25 15:06:24
 * @ Modified time: 2024-03-31 22:48:16
 * @ Description:
 * 
 * This file defines the page handler for the help page.
 */

#ifndef PAGE_ACCOUNT_
#define PAGE_ACCOUNT_

#include "../game/game.c"
#include "../game/profile.game.c"
#include "../game/stats.game.c"

#include "../utils/utils.asset.h"
#include "../utils/utils.page.h"
#include "../utils/utils.component.h"

/**
 * Configures the main menu.
 * 
 * @param   { p_obj }   pArgs_Page  The page instance we need to configure.
*/
void PageHandler_account(p_obj pArgs_Page) {
  
  int i;
  Page *this = (Page *) pArgs_Page;
  Profile *pProfile = (Profile *) this->pSharedObject;
  int dWidth, dHeight, dMargin;

  // Components
  char *sAccountComponent = "account.fixed";
  char *sAccountContainerComponent = "account-container.col";
  char *sTitleComponent = "title.aleft-x.abottom-y";
  char *sDividerComponent = "divider.aleft-x.abottom-y";
  char *sStatsComponent = "stats.aleft-x.atop-y";
  char *sGridComponent = "grid.aleft-x.atop-y";
  char *sPromptTextComponent = "prompt-text.aright-x.atop-y";

  // Page title and asset
  char *sPageTitleFont = "body-font";
  char *sPageTitle = "account info";
  char sPageTitleKey[STRING_KEY_MAX_LENGTH];

  // Some text stuff
  char *sDividerText;
  char *sStatsText;
  char *sGridText;
  char *sGridBuffer[GAME_MAX_ROWS];
  int nGridBufferHeight = 0;

  // Do stuff based on page status
  switch(this->ePageStatus) {

    case PAGE_ACTIVE_INIT:

      // Get the dimensions 
      dWidth = IO_getWidth();
      dHeight = IO_getHeight();
      dMargin = 10;

      // Create text assets
      AssetManager_createTextAsset(this->pSharedAssetManager, sPageTitle, sPageTitleFont);

      // Create keys
      String_keyAndStr(sPageTitleKey, sPageTitleFont, sPageTitle);

      // Create texts
      sDividerText = String_repeat("▄", dWidth - dMargin * 2);

      // Create component tree
      Page_addComponentContext(this, sAccountComponent, "root", 0, 0, dWidth, dHeight, "secondary", "primary");
      Page_addComponentContainer(this, sAccountContainerComponent, sAccountComponent, dMargin, dMargin / 2);
      Page_addComponentAsset(this, sTitleComponent, sAccountContainerComponent, -1, 1, "", "", sPageTitleKey);
      Page_addComponentText(this, sDividerComponent, sAccountContainerComponent, 0, 0, "accent", "", sDividerText);
      Page_addComponentText(this, sStatsComponent, sAccountContainerComponent, 0, 0, "secondary", "", "");
      Page_addComponentText(this, sGridComponent, sAccountContainerComponent, 32, 0, "secondary", "", "");
      Page_addComponentText(this, sPromptTextComponent, sAccountComponent, dWidth - dMargin, dHeight - dMargin / 2, 
        "secondary-lighten-0.5", "", "[backspace] or [esc] to go back");

      // Garbage collection
      String_kill(sDividerText);
    break;

    case PAGE_ACTIVE_RUNNING:

      // Switch based on what key was last pressed
      switch(EventStore_get(this->pSharedEventStore, "key-pressed")) {

        // Exit the page
        case 8: case 27:
          Page_idle(this);
          Page_setNext(this, "menu");
        break;

        default:

        break;
      }

      // Update the UI
      // Generate the text for the stats first
      sStatsText = String_alloc(1024);

      // Account name
      sprintf(sStatsText, "PROFILE: %s\n", pProfile->sCurrentProfile);
      
      // Classic easy stats
      sprintf(sStatsText, "%s\nCLASSIC, EASY         %d wins  \n                      %d losses\n                      %d games\n", sStatsText, 
        Stats_getWins(pProfile, GAME_TYPE_CLASSIC, GAME_DIFFICULTY_EASY), 
        Stats_getLosses(pProfile, GAME_TYPE_CLASSIC, GAME_DIFFICULTY_EASY),
        Stats_getTotalGames(pProfile, GAME_TYPE_CLASSIC, GAME_DIFFICULTY_EASY));

      // Classic difficult stats
      sprintf(sStatsText, "%s\nCLASSIC, DIFFICULT    %d wins  \n                      %d losses\n                      %d games\n", sStatsText,
        Stats_getWins(pProfile, GAME_TYPE_CLASSIC, GAME_DIFFICULTY_DIFFICULT), 
        Stats_getLosses(pProfile, GAME_TYPE_CLASSIC, GAME_DIFFICULTY_DIFFICULT),
        Stats_getTotalGames(pProfile, GAME_TYPE_CLASSIC, GAME_DIFFICULTY_DIFFICULT));

      // Custom stats
      sprintf(sStatsText, "%s\nCUSTOM                %d wins  \n                      %d losses\n                      %d games\n", sStatsText,
        Stats_getWins(pProfile, GAME_TYPE_CUSTOM, GAME_DIFFICULTY_NONE), 
        Stats_getLosses(pProfile, GAME_TYPE_CUSTOM, GAME_DIFFICULTY_NONE),
        Stats_getTotalGames(pProfile, GAME_TYPE_CUSTOM, GAME_DIFFICULTY_NONE));

      // Get the grid and turn it into string
      Stats_getBoard(pProfile, 0, &nGridBufferHeight, sGridBuffer);
      sGridText = String_alloc(1024);
      for(i = 0; i < nGridBufferHeight; i++)
        sprintf(sGridText, "%s%s", sGridText, sGridBuffer[i]);

      // Add the stats and the grid to the page
      Page_setComponentText(this, sStatsComponent, sStatsText);
      Page_setComponentText(this, sGridComponent, sGridBuffer);

      // !KILL THE GRID BUFFER

      // Garbage collection
      String_kill(sStatsText);
      String_kill(sGridText);

    break;

    default:
      //  At this point, the page manager should detect that the page has become idle
      //  After that, the page manager should switch control to the page indicated by this->sNextPage
    break;
  }
}

#endif