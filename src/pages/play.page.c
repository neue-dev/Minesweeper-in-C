/**
 * @ Author: MMMM
 * @ Create Time: 2024-02-25 15:06:24
 * @ Modified time: 2024-03-26 22:10:59
 * @ Description:
 * 
 * This file defines the page handler for the help page.
 */

#ifndef PAGE_PLAY_
#define PAGE_PLAY_

#include "../utils/utils.asset.h"
#include "../utils/utils.page.h"
#include "../utils/utils.component.h"

/**
 * Configures the main menu.
 * 
 * @param   { p_obj }   pArgs_Page  The page instance we need to configure.
*/
void PageHandler_play(p_obj pArgs_Page) {

  Page *this = (Page *) pArgs_Page;
  int dWidth, dHeight, dMargin;

  // Header details
  char *sHeader = "play game";
  char *sHeaderFont = "body-font";
  char sHeaderKey[STRING_KEY_MAX_LENGTH];

  // Component names
  char *sPlayComponent = "play.fixed";
  char *sPlayFormComponent = "play-form.col";
  char *sHeaderComponent = "header.acenter-x.atop-y";
  char *sFieldContainerComponent = "field-container.col.aleft-x.atop-y";
  char *sFileordiffPromptComponent = "fileordiff-prompt.aleft-x";
  char *sTypePromptComponent = "type-prompt.aleft-x";
  char *sFileordiffComponent = "fileordiff.aleft-x";
  char *sTypeComponent = "type.aleft-x";
  char *sFieldPromptComponent = "field-prompt.aleft-x";
  char *sErrorPromptComponent = "error-prompt.aleft-x";

  // Do stuff based on page status
  switch(this->ePageStatus) {

    case PAGE_ACTIVE_INIT:

      // Get the dimensions 
      dWidth = IO_getWidth();
      dHeight = IO_getHeight();
      dMargin = 48;

      // Create the header
      String_keyAndStr(sHeaderKey, sHeaderFont, sHeader);
      AssetManager_createTextAsset(this->pSharedAssetManager, sHeader, sHeaderFont);

      // Create component tree
      Page_addComponentContext(this, sPlayComponent, "root", 0, 0, dWidth, dHeight, "primary", "secondary");
      Page_addComponentContainer(this, sPlayFormComponent, sPlayComponent, 0, 0);
      Page_addComponentAsset(this, sHeaderComponent, sPlayFormComponent, dWidth / 2, 6, "", "", sHeaderKey);
      Page_addComponentContainer(this, sFieldContainerComponent, sPlayFormComponent, dWidth / 2 - dMargin / 2, 4);
      Page_addComponentText(this, sTypePromptComponent, sFieldContainerComponent, 1, 0, "", "", "Enter game type [classic/custom]:");
      Page_addComponentText(this, sTypeComponent, sFieldContainerComponent, 1, 0, "", "", "");
      Page_addComponentText(this, sFileordiffPromptComponent, sFieldContainerComponent, 1, 1, "", "", "Enter difficulty/filename:");
      Page_addComponentText(this, sFileordiffComponent, sFieldContainerComponent, 1, 0, "", "", "");
      Page_addComponentText(this, sErrorPromptComponent, sFieldContainerComponent, 1, 2, "secondary", "accent", "");
      Page_addComponentText(this, sFieldPromptComponent, sFieldContainerComponent, 1, 1, "primary-darken-0.5", "", "[tab] to switch fields; [enter] to submit");
      
      // Define initial user states
      // if(Page_getUserState(this, "custom-current-field") == -1) Page_setUserState(this, "custom-current-field", cCustomCurrentField);
      // if(Page_getUserState(this, "custom-field-count") == -1) Page_setUserState(this, "custom-field-count", cCustomFieldCount);
      
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