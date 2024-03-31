/**
 * @ Author: MMMM
 * @ Create Time: 2024-02-25 15:06:24
 * @ Modified time: 2024-04-01 02:21:54
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
  char *sDataContainerComponent = "data-container.fixed";
  char *sTitleComponent = "title.aleft-x.abottom-y";
  char *sDividerComponent = "divider.aleft-x.abottom-y";
  char *sStatsComponent = "stats.aleft-x.atop-y";
  char *sGridComponent = "grid.aleft-x.atop-y";
  char *sBoardIdPromptComponent = "board-id-prompt.aleft-x";
  char *sBoardIdFieldComponent = "board-id-field.aleft-x";
  char *sErrorPromptComponent = "error-prompt.aleft-x";
  char *sLegendComponent = "legend-text.aleft-x.atop-y";
  char *sBoardInfoComponent = "board-info.aleft-x.atop-y";
  char *sPromptTextComponent = "prompt-text.aright-x.atop-y";

  // Page title and asset
  char *sPageTitleFont = "body-font";
  char *sPageTitle = "account info";
  char sPageTitleKey[STRING_KEY_MAX_LENGTH];

  // Input handling
  char *sBoardIdField;
  int dBoardFieldId;

  // Some text stuff
  char *sDividerText;
  char *sStatsText;
  char *sGridText;
  char *sGridDescText;
  char *sLegendText;
  char *sGridBuffer[GAME_MAX_ROWS + 1];
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
      Page_addComponentContext(this, sAccountComponent, "root", 0, 0, dWidth, dHeight, "primary", "secondary");
      Page_addComponentContainer(this, sAccountContainerComponent, sAccountComponent, dMargin, dMargin / 2);
      Page_addComponentAsset(this, sTitleComponent, sAccountContainerComponent, -1, 1, "", "", sPageTitleKey);
      Page_addComponentText(this, sDividerComponent, sAccountContainerComponent, 0, 0, "accent", "", sDividerText);
      Page_addComponentContainer(this, sDataContainerComponent, sAccountContainerComponent, 0, 1);
      Page_addComponentText(this, sBoardIdPromptComponent, sDataContainerComponent, 0, 0, "", "", "Enter number:");
      Page_addComponentText(this, sBoardIdFieldComponent, sDataContainerComponent, 0, 1, "accent", "", "");
      Page_addComponentText(this, sStatsComponent, sDataContainerComponent, 64, 5, "primary-darken-0.33", "", "");
      Page_addComponentText(this, sGridComponent, sDataContainerComponent, 0, 5, "primary-darken-0.33", "", "");
      Page_addComponentText(this, sBoardInfoComponent, sDataContainerComponent, 18, 9, "primary-darken-0.33", "", "");
      Page_addComponentText(this, sErrorPromptComponent, sDataContainerComponent, 0, 3, "secondary", "accent", "");     
      Page_addComponentText(this, sLegendComponent, sDataContainerComponent, 0, 18, "primary-darken-0.67", "", "");
      Page_addComponentText(this, sPromptTextComponent, sAccountComponent, dWidth - dMargin, dHeight - dMargin / 2, 
        "primary-darken-0.5", "", "[esc] to go back");

      // Garbage collection
      String_kill(sDividerText);
    break;

    case PAGE_ACTIVE_RUNNING:

      // Switch based on what key was last pressed
      switch(EventStore_get(this->pSharedEventStore, "key-pressed")) {

        // Exit the page
        case 27:
          Page_idle(this);
          Page_setNext(this, "menu");
        break;

        default:
          EventStore_setString(this->pSharedEventStore, "key-pressed", "boardid-input");

          // Clear error
          if(EventStore_get(this->pSharedEventStore, "key-pressed"))
            Page_setComponentText(this, sErrorPromptComponent, "");
        break;
      }

      // Grab user input
      sBoardIdField = String_toUpper(EventStore_getString(this->pSharedEventStore, "boardid-input"));

      // Update the UI
      // Generate the text for the stats first
      sStatsText = String_alloc(1024);
      sGridText = String_alloc(1024);
      sGridDescText = String_alloc(1024);
      sLegendText = String_alloc(1024);

      // Classic easy stats
      sprintf(sStatsText, "%sCLASSIC, EASY         %d wins     \n                      %d losses\n                      %d games\n", sStatsText, 
        Stats_getWins(pProfile, GAME_TYPE_CLASSIC, GAME_DIFFICULTY_EASY), 
        Stats_getLosses(pProfile, GAME_TYPE_CLASSIC, GAME_DIFFICULTY_EASY),
        Stats_getTotalGames(pProfile, GAME_TYPE_CLASSIC, GAME_DIFFICULTY_EASY));

      // Classic difficult stats
      sprintf(sStatsText, "%s\nCLASSIC, DIFFICULT    %d wins     \n                      %d losses\n                      %d games\n", sStatsText,
        Stats_getWins(pProfile, GAME_TYPE_CLASSIC, GAME_DIFFICULTY_DIFFICULT), 
        Stats_getLosses(pProfile, GAME_TYPE_CLASSIC, GAME_DIFFICULTY_DIFFICULT),
        Stats_getTotalGames(pProfile, GAME_TYPE_CLASSIC, GAME_DIFFICULTY_DIFFICULT));

      // Custom stats
      sprintf(sStatsText, "%s\nCUSTOM                %d wins     \n                      %d losses\n                      %d games\n", sStatsText,
        Stats_getWins(pProfile, GAME_TYPE_CUSTOM, GAME_DIFFICULTY_NONE), 
        Stats_getLosses(pProfile, GAME_TYPE_CUSTOM, GAME_DIFFICULTY_NONE),
        Stats_getTotalGames(pProfile, GAME_TYPE_CUSTOM, GAME_DIFFICULTY_NONE));

      // Grid header
      dBoardFieldId = atoi(sBoardIdField);
      sprintf(sGridText, "Here is the snapshot of the %dth most recent game:\n\n", dBoardFieldId > 0 ? dBoardFieldId : 1);

      // Grid description
      sprintf(sGridDescText, "%s", "CLASSIC, DIFFICULT, WIN, BRO");

      // Get the grid
      Stats_getBoard(pProfile, dBoardFieldId > 0 ? dBoardFieldId - 1 : 0, &nGridBufferHeight, sGridBuffer);

      // If the number if too large
      if(atoi(sBoardIdField) > 
        Stats_getTotalGames(pProfile, GAME_TYPE_CLASSIC, GAME_DIFFICULTY_EASY) + 
        Stats_getTotalGames(pProfile, GAME_TYPE_CLASSIC, GAME_DIFFICULTY_DIFFICULT) +
        Stats_getTotalGames(pProfile, GAME_TYPE_CUSTOM, GAME_DIFFICULTY_NONE))
          Page_setComponentText(this, sErrorPromptComponent, "Error: number is larger than number of games played.");
      
      // Turn the grid into string
      for(i = 0; i < nGridBufferHeight; i++)
        strcat(sGridText, String_replace(sGridBuffer[i], '\r', '\0'));

      // Create the legend
      sprintf(sLegendText, 
        "LEGEND                                                         \n\n%s\n%s\n", 
        "[ exploded mine='@' ][ flag='^' ][ unflagged mine='*' ]       ",
        "If there are blank tiles or the grid is empty, the player quit");

      // Add the stats and the grid and the legend to the page
      Page_setComponentText(this, sStatsComponent, sStatsText);
      Page_setComponentText(this, sGridComponent, sGridText);
      Page_setComponentText(this, sBoardInfoComponent, sGridDescText);
      Page_setComponentText(this, sLegendComponent, sLegendText);

      // Indicate the user input on screen
      Page_setComponentText(this, sBoardIdFieldComponent, strlen(sBoardIdField) ? sBoardIdField : "____________________");

      // Garbage collection
      for(i = 0; i < nGridBufferHeight; i++)
        String_kill(sGridBuffer[i]);

      String_kill(sStatsText);
      String_kill(sGridText);
      String_kill(sGridDescText);
      String_kill(sLegendText);
    break;

    default:
      //  At this point, the page manager should detect that the page has become idle
      //  After that, the page manager should switch control to the page indicated by this->sNextPage
    break;
  }
}

#endif