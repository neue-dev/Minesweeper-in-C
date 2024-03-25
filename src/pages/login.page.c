/**
 * @ Author: MMMM
 * @ Create Time: 2024-02-25 15:06:24
 * @ Modified time: 2024-03-25 12:58:16
 * @ Description:
 * 
 * This file defines the page handler for the login.
 */

#ifndef PAGE_INTRO_
#define PAGE_INTRO_

#include "../utils/utils.asset.h"
#include "../utils/utils.page.h"
#include "../utils/utils.component.h"

/**
 * Configures the logjn page.
 * 
 * @param   { p_obj }   pArgs_Page  The instance page we need to configure.
*/
void PageHandler_login(p_obj pArgs_Page) {
  
  Page *this = (Page *) pArgs_Page;
  int dWidth, dHeight;

  // Header details
  char *sHeader = "login";
  char *sHeaderFont = "body-font";
  char sHeaderKey[STRING_KEY_MAX_LENGTH];

  // Component names
  char *sLoginComponent = "login.col";
  char *sLogoComponent = "logo.acenter-x.atop-y";
  char *sHeaderComponent = "header.acenter-x.atop-y";
  char *sFieldContainerComponent = "field-container.col.acenter-x.atop-y";
  char *sUsernameComponent = "username.acenter-x";
  char *sPasswordComponent = "password.acenter-x";

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
      Page_addComponentContext(this, sLoginComponent, "root", 0, 0, dWidth, dHeight, "primary", "secondary");
      Page_addComponentAsset(this, sLogoComponent, sLoginComponent, dWidth / 2, 5, "primary", "secondary", "logo");
      Page_addComponentAsset(this, sHeaderComponent, sLoginComponent, dWidth / 2, 2, "", "", sHeaderKey);
      Page_addComponentContainer(this, sFieldContainerComponent, sLoginComponent, dWidth / 2, 5);
      Page_addComponentText(this, sUsernameComponent, sFieldContainerComponent, 0, 0, "", "", "Enter username: ");
      Page_addComponentText(this, sPasswordComponent, sFieldContainerComponent, 0, 0, "", "", "Enter password: ");

    break;

    case PAGE_ACTIVE_RUNNING:


    break;

    default: break;
  }
}

#endif