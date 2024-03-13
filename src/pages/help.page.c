/**
 * @ Author: MMMM
 * @ Create Time: 2024-02-25 15:06:24
 * @ Modified time: 2024-03-13 22:59:14
 * @ Description:
 * 
 * This file defines the page handler for the help page.
 */

#ifndef PAGE_HELP_
#define PAGE_HELP_

#include "../utils/utils.asset.h"
#include "../utils/utils.page.h"
#include "../utils/utils.component.h"

/**
 * Configures the main menu.
 * 
 * @param   { p_obj }   pArgs_Page  The page instance we need to configure.
*/
void PageHandler_help(p_obj pArgs_Page) {

  Page *this = (Page *) pArgs_Page;
  int dWidth, dHeight, i;

  // Components
  char *sHelpComponent = "help-col";
  char *sTitleComponent = "title-aleft.x-abottom.y";
  char *sDividerComponent = "divider-aleft.x-abottom.y";
  char *sBodyTextComponent = "body.text-aleft.x-atop.y";

  // Page title and asset
  char *sPageTitleFont = "body-font";
  char *sPageTitle = "how to play";
  char sPageTitleKey[STRING_KEY_MAX_LENGTH];

  // Component names

  // Do stuff based on page status
  switch(this->ePageStatus) {

    case PAGE_ACTIVE_INIT:

      // Get the dimensions 
      dWidth = IO_getWidth();
      dHeight = IO_getHeight();

      // Create text assets
      AssetManager_createTextAsset(this->pSharedAssetManager, sPageTitle, sPageTitleFont);

      // Create keys
      String_keyAndStr(sPageTitleKey, sPageTitleFont, sPageTitle);

      // Create component tree
      Page_addComponentContext(this, sHelpComponent, "root", 0, 0, dWidth, dHeight, "secondary", "primary");
      Page_addComponentAsset(this, sTitleComponent, sHelpComponent, -100, 6, "", "", sPageTitleKey);
      Page_addComponentText(this, sDividerComponent, sHelpComponent, -100, 1, "accent", "", String_repeat("=", dWidth - 16));
      Page_addComponentText(this, sBodyTextComponent, sHelpComponent, -100, 1, "", "", 
        String_join("\n", "-",
          "MINEZ is a minesweeper spin-off implemented in C with some neat",
          "bonus features.",
          "",
          "The goal of the game is to identify all the cells with mines on",
          "them by using the numerical hints provided throughout the grid.",
          "",
          "Each hint tells us how many mines are adjacent to any given cell.",
          "To identify a cell as having a mine, simply plant a flag on it.",
          "",
          "More information about the controls can be found in the settings,",
          "although these will also be given as you play along.",
          "-"
        ));

      // Start animations
      Page_setComponentTargetPosition(this, sTitleComponent, 7, PAGE_NULL_INT, 0.7);
      Page_setComponentTargetPosition(this, sDividerComponent, 8, PAGE_NULL_INT, 0.7);
      Page_setComponentTargetPosition(this, sBodyTextComponent, 8, PAGE_NULL_INT, 0.7);
    break;

    case PAGE_ACTIVE_RUNNING:
      
      // Animations
      switch(this->dStage) {
        
        case 0: 
          
        break;
      }

    break;

    default:
      // ! exit the page
    break;
  }
}

#endif