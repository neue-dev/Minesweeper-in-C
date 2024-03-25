/**
 * @ Author: MMMM
 * @ Create Time: 2024-02-25 15:06:24
 * @ Modified time: 2024-03-25 19:14:37
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
  char *sLoginComponent = "login.fixed";
  char *sLoginFormComponent = "login-form.col";
  char *sLogoComponent = "logo.acenter-x.atop-y";
  char *sHeaderComponent = "header.acenter-x.atop-y";
  char *sFieldContainerComponent = "field-container.col.acenter-x.atop-y";
  char *sUsernamePromptComponent = "username-prompt.acenter-x";
  char *sPasswordPromptComponent = "password-prompt.acenter-x";
  char *sUsernameComponent = "username.acenter-x";
  char *sPasswordComponent = "password.acenter-x";
  char *sFieldPromptComponent = "field-prompt.acenter-x";

  // Input fields
  char *sUsernameField;
  char *sPasswordField;
  char cLoginCurrentField;

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
      Page_addComponentContainer(this, sLoginFormComponent, sLoginComponent, 0, 0);
      Page_addComponentAsset(this, sLogoComponent, sLoginFormComponent, dWidth / 2, 5, "primary", "secondary", "logo");
      Page_addComponentAsset(this, sHeaderComponent, sLoginFormComponent, dWidth / 2, 2, "", "", sHeaderKey);
      Page_addComponentContainer(this, sFieldContainerComponent, sLoginFormComponent, dWidth / 2, 5);
      Page_addComponentText(this, sUsernamePromptComponent, sFieldContainerComponent, 1, 0, "", "", "Enter username:");
      Page_addComponentText(this, sUsernameComponent, sFieldContainerComponent, 1, 0, "", "", "");
      Page_addComponentText(this, sPasswordPromptComponent, sFieldContainerComponent, 1, 2, "", "", "Enter password:");
      Page_addComponentText(this, sPasswordComponent, sFieldContainerComponent, 1, 0, "", "", "");
      Page_addComponentText(this, sFieldPromptComponent, sFieldContainerComponent, 1, 2, "primary-darken-0.5", "", "[tab] to switch fields; [enter] to submit");

      // Define initial user states
      if(Page_getUserState(this, "login-current-field") == -1) Page_setUserState(this, "login-current-field", 0);

    break;

    case PAGE_ACTIVE_RUNNING:

      // Key handling
      cLoginCurrentField = Page_getUserState(this, "login-current-field");

      // Switch based on what key was last pressed
      switch(EventStore_get(this->pSharedEventStore, "key-pressed")) {

        case '\t':
          Page_setUserState(this, "login-current-field", (cLoginCurrentField + 1) % 2);
        break;

        case '\n': case '\r':
          Page_idle(this);
          Page_setNext(this, "menu");
        break;

        default:
        
          // Update the values of the current inputted username
          if(cLoginCurrentField == 0) { 
            EventStore_setString(this->pSharedEventStore, "key-pressed", "username-input");

            Page_setComponentColor(this, sUsernamePromptComponent, "primary", "");
            Page_setComponentColor(this, sUsernameComponent, "accent", "");
            Page_setComponentColor(this, sPasswordPromptComponent, "primary-darken-0.75", "");
            Page_setComponentColor(this, sPasswordComponent, "primary-darken-0.75", "");

          // Update the values of the current inputted password
          } else if(cLoginCurrentField == 1) {
            EventStore_setString(this->pSharedEventStore, "key-pressed", "password-input");      

            Page_setComponentColor(this, sUsernamePromptComponent, "primary-darken-0.75", "");
            Page_setComponentColor(this, sUsernameComponent, "primary-darken-0.75", "");
            Page_setComponentColor(this, sPasswordPromptComponent, "primary", "");
            Page_setComponentColor(this, sPasswordComponent, "accent", "");          
          }

        break;
      }

      // Retrieve the user input 
      sUsernameField = EventStore_getString(this->pSharedEventStore, "username-input");
      sPasswordField = EventStore_getString(this->pSharedEventStore, "password-input");

      // Indicate the user input on screen
      Page_setComponentText(this, sUsernameComponent, strlen(sUsernameField) ? sUsernameField : "<username>");
      Page_setComponentText(this, sPasswordComponent, strlen(sPasswordField) ? sPasswordField : "<password>");

    break;

    default: break;
  }
}

#endif