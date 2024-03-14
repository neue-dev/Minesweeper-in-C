/**
 * @ Author: MMMM
 * @ Create Time: 2024-02-25 15:06:24
 * @ Modified time: 2024-03-14 10:53:16
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
  int dWidth, dHeight, dMargin, i;

  // Components
  char *sHelpComponent = "help-fixed";
  char *sHelpContainerComponent = "help.container-col";
  char *sTitleComponent = "title-aleft.x-abottom.y";
  char *sDividerComponent = "divider-aleft.x-abottom.y";
  char *sBodyTextComponent = "body.text-aleft.x-atop.y";
  char *sPromptTextComponent = "prompt.text-aright.x-atop.y";

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
      dMargin = 10;

      // Create text assets
      AssetManager_createTextAsset(this->pSharedAssetManager, sPageTitle, sPageTitleFont);

      // Create keys
      String_keyAndStr(sPageTitleKey, sPageTitleFont, sPageTitle);

      // Create component tree
      Page_addComponentContext(this, sHelpComponent, "root", 0, 0, dWidth, dHeight, "secondary", "primary");
      Page_addComponentContainer(this, sHelpContainerComponent, sHelpComponent, dMargin, 0);
      Page_addComponentAsset(this, sTitleComponent, sHelpContainerComponent, -100, 6, "", "", sPageTitleKey);
      Page_addComponentText(this, sDividerComponent, sHelpContainerComponent, 0, 0, "accent", "", String_repeat("▄", dWidth - dMargin * 2));
      Page_addComponentText(this, sBodyTextComponent, sHelpContainerComponent, 0, 2, "", "",
        String_join("\n", "-", dWidth - dMargin * 2,
          "MINEZ is a minesweeper spin-off implemented in C with some neat bonus features.",
          "","",
          "The goal of the game is to identify all the cells with mines on them by using the numerical hints provided throughout the grid.",
          "","",
          "Each hint tells us how many mines are adjacent to any given cell. To identify a cell as having a mine, simply plant a flag on it.",
          "","",
          "More information about the controls can be found in the settings, although these will also be given as you play along.",
          "-"
        ));
      Page_addComponentText(this, sPromptTextComponent, sHelpComponent, 999, dHeight - dMargin / 2, 
        "secondary-lighten-0.5", "", "[backspace] or [esc] to go back");

      // Start animations
      Page_resetComponentInitialPosition(this, sDividerComponent, -100, PAGE_NULL_INT);
      Page_resetComponentInitialPosition(this, sBodyTextComponent, -100, PAGE_NULL_INT);

      Page_setComponentTargetPosition(this, sTitleComponent, -1, PAGE_NULL_INT, 0.7);
      Page_setComponentTargetPosition(this, sPromptTextComponent, dWidth - dMargin, PAGE_NULL_INT, 0.0);
      Page_setComponentTargetPosition(this, sDividerComponent, 0, PAGE_NULL_INT, 0.0);
      Page_setComponentTargetPosition(this, sBodyTextComponent, 0, PAGE_NULL_INT, 0.0);
      
    break;

    case PAGE_ACTIVE_RUNNING:

      // Switch based on what key was last pressed
      switch(EventStore_get(this->pSharedEventStore, "key-pressed")) {

        // Exit the page
        case 8: case 27:
          this->dStage = 999;
        break;

        default:

        break;
      }
      
      // Animations
      switch(this->dStage) {
        
        case 0:     // The header
          if(Page_getComponentDist(this, sTitleComponent, 0) < MATH_E_POS1)
            Page_nextStage(this);
        break;

        case 1:     // The divider
          Page_setComponentTransitionSpeed(this, sDividerComponent, 0.8);
          
          if(Page_getComponentDist(this, sDividerComponent, 0) < MATH_E_POS1)
            Page_nextStage(this);
        break;

        case 2:     // The help text 
          Page_setComponentTransitionSpeed(this, sBodyTextComponent, 0.8);

          if(Page_getComponentDist(this, sBodyTextComponent, 0) < MATH_E_POS1)
            Page_nextStage(this);
        break;

        case 3:     // The exit prompt
          Page_setComponentTransitionSpeed(this, sPromptTextComponent, 0.8);
        break;

        default:    // Exit the page
          Page_idle(this);
          Page_setNext(this, "menu");
        break;
      }

    break;

    default:
      // ! exit the page
    break;
  }
}

#endif