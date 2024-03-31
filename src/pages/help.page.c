/**
 * @ Author: MMMM
 * @ Create Time: 2024-02-25 15:06:24
 * @ Modified time: 2024-03-31 20:30:57
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
  int dWidth, dHeight, dMargin;

  // Components
  char *sHelpComponent = "help.fixed";
  char *sHelpContainerComponent = "help-container.col";
  char *sTitleComponent = "title.aleft-x.abottom-y";
  char *sDividerComponent = "divider.aleft-x.abottom-y";
  char *sBodyTextComponent = "body-text.aleft-x.atop-y";
  char *sPromptTextComponent = "prompt-text.aright-x.atop-y";

  // Page title and asset
  char *sPageTitleFont = "body-font";
  char *sPageTitle = "how to play";
  char sPageTitleKey[STRING_KEY_MAX_LENGTH];

  // Divider text
  char *sDividerText;

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
      Page_addComponentContext(this, sHelpComponent, "root", 0, 0, dWidth, dHeight, "secondary", "primary");
      Page_addComponentContainer(this, sHelpContainerComponent, sHelpComponent, dMargin, dMargin / 2);
      Page_addComponentAsset(this, sTitleComponent, sHelpContainerComponent, -1, 1, "", "", sPageTitleKey);
      Page_addComponentText(this, sDividerComponent, sHelpContainerComponent, 0, 0, "accent", "", sDividerText);
      Page_addComponentText(this, sBodyTextComponent, sHelpContainerComponent, 0, 2, "", "",
        String_join("\n", "-", dWidth / 2 - dMargin,
          "MINEZ is a minesweeper spin-off implemented in C with some neat features.", "","",
          "The goal of the game is to identify all the cells with mines on them.", "","",
          "Numerical hints are provided on the grid to help you deduce where mines are. Each hint tells you how many mines are adjacent to a given cell. You must use flags to indicate the presence of a mine on a cell.", "","",
          "More information about the controls can be found in the settings, although these will also be given as you play along.",
          "-"
        ));
      Page_addComponentText(this, sPromptTextComponent, sHelpComponent, dWidth - dMargin, dHeight - dMargin / 2, 
        "secondary-lighten-0.5", "", "[backspace] or [esc] to go back");

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

    break;

    default:
      //  At this point, the page manager should detect that the page has become idle
      //  After that, the page manager should switch control to the page indicated by this->sNextPage
    break;
  }
}

#endif