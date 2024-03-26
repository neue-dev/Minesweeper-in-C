/**
 * @ Author: MMMM
 * @ Create Time: 2024-02-25 15:06:24
 * @ Modified time: 2024-03-26 20:33:18
 * @ Description:
 * 
 * This file defines the page handler for the help page.
 */

#ifndef PAGE_CUSTOM_
#define PAGE_CUSTOM_

#include "../utils/utils.asset.h"
#include "../utils/utils.page.h"
#include "../utils/utils.component.h"

/**
 * Configures the main menu.
 * 
 * @param   { p_obj }   pArgs_Page  The page instance we need to configure.
*/
void PageHandler_custom(p_obj pArgs_Page) {

  Page *this = (Page *) pArgs_Page;
  int dWidth, dHeight, dMargin, i;

  // Header details
  char *sHeader = "login";
  char *sHeaderFont = "body-font";
  char sHeaderKey[STRING_KEY_MAX_LENGTH];

  // Component names
  char *sCustomComponent = "custom.fixed";
  char *sCustomFormComponent = "custom-form.col";
  char *sLogoComponent = "logo.acenter-x.atop-y";
  char *sHeaderComponent = "header.acenter-x.atop-y";
  char *sFieldContainerComponent = "field-container.col.acenter-x.atop-y";
  char *sUsernamePromptComponent = "username-prompt.acenter-x";
  char *sPasswordPromptComponent = "password-prompt.acenter-x";
  char *sUsernameComponent = "username.acenter-x";
  char *sPasswordComponent = "password.acenter-x";
  char *sFieldPromptComponent = "field-prompt.acenter-x";


  // Do stuff based on page status
  switch(this->ePageStatus) {

    case PAGE_ACTIVE_INIT:

            // Get the dimensions 
      dWidth = IO_getWidth();
      dHeight = IO_getHeight();

      // Create the header
      String_keyAndStr(sHeaderKey, sHeaderFont, sHeader);
      AssetManager_createTextAsset(this->pSharedAssetManager, sHeader, sHeaderFont);

      // Create component tree
      Page_addComponentContext(this, sCustomComponent, "root", 0, 0, dWidth, dHeight, "primary", "secondary");
      Page_addComponentContainer(this, sCustomFormComponent, sCustomComponent, 0, 0);
      Page_addComponentAsset(this, sLogoComponent, sCustomFormComponent, dWidth / 2, 5, "primary", "secondary", "logo");
      Page_addComponentAsset(this, sHeaderComponent, sCustomFormComponent, dWidth / 2, 2, "", "", sHeaderKey);
      Page_addComponentContainer(this, sFieldContainerComponent, sCustomFormComponent, dWidth / 2, 5);
      Page_addComponentText(this, sUsernamePromptComponent, sFieldContainerComponent, 1, 0, "", "", "Enter username:");
      Page_addComponentText(this, sUsernameComponent, sFieldContainerComponent, 1, 0, "", "", "");
      Page_addComponentText(this, sPasswordPromptComponent, sFieldContainerComponent, 1, 2, "", "", "Enter password:");
      Page_addComponentText(this, sPasswordComponent, sFieldContainerComponent, 1, 0, "", "", "");
      Page_addComponentText(this, sFieldPromptComponent, sFieldContainerComponent, 1, 2, "primary-darken-0.5", "", "[tab] to switch fields; [enter] to submit");
      
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