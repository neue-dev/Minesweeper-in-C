/**
 * @ Author: MMMM
 * @ Create Time: 2024-02-25 15:06:24
 * @ Modified time: 2024-03-13 15:18:38
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

  // Font
  char *sPageTitleFont = "body-font";
  char *sPageTitle = "how to play";
  char sPageTitleKey[STRING_KEY_MAX_LENGTH];

  // Component names
  char *sHelpComponent = "help-fixed";

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
      Page_addComponentContainer(this, sHelpComponent, "root", 0, 0);
      Page_addComponentAsset(this, "test", sHelpComponent, 0, 0, "", "", sPageTitleKey);
      
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