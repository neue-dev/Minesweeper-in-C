/**
 * @ Author: MMMM
 * @ Create Time: 2024-02-25 15:06:24
 * @ Modified time: 2024-04-01 06:46:57
 * @ Description:
 * 
 * This file defines the page handler for the login.
 */

#ifndef PAGE_LOGIN_
#define PAGE_LOGIN_

#include "../game/profile.game.c"
#include "../game/stats.game.c"

#include "../utils/utils.asset.h"
#include "../utils/utils.page.h"
#include "../utils/utils.component.h"

#include <string.h>

/**
 * Configures the logjn page.
 * 
 * @param   { p_obj }   pArgs_Page  The instance page we need to configure.
*/
void PageHandler_login(p_obj pArgs_Page) {
  int i;
  
  Page *this = (Page *) pArgs_Page;
  Profile *pProfile = (Profile *) this->pSharedObject;
  int dWidth, dHeight, dMargin;

  // Title details
  char *sTitle = "profile login";
  char *sTitleFont = "body-font";
  char sTitleKey[STRING_KEY_MAX_LENGTH];

  // Divider text
  char *sDividerText;

  // Component names
  char *sLoginComponent = "login.fixed";
  char *sLoginFormComponent = "login-form.col";
  char *sTitleComponent = "title.aleft-x.abottom-y";
  char *sDividerComponent = "divider.aleft-x.abottom-y";
  char *sProfileListComponent = "profile-list.aleft-x.atop-y";
  char *sFieldContainerComponent = "field-container.col.aleft-x.atop-y";
  char *sUsernamePromptComponent = "username-prompt.aleft-x";
  char *sPasswordPromptComponent = "password-prompt.aleft-x";
  char *sUsernameComponent = "username.aleft-x";
  char *sPasswordComponent = "password.aleft-x";
  char *sFieldPromptComponent = "field-prompt.aright-x.abottom-y";
  char *sErrorPromptComponent = "error-prompt.aleft-x";
  char *sPopupComponent = "popup.fixed";

  // Profile list stuff
  char *sProfileListText;
  int nProfileListLength;
  char *sProfileListArray[PROFILES_MAX_NUM + 1];

  // Input fields
  char *sUsernameField;
  char *sPasswordField;
  char cLoginCurrentField = 0;
  char cLoginFieldCount = 2;
  char cKeyPressed = 0;

  // Do stuff based on page status
  switch(this->ePageStatus) {

    case PAGE_ACTIVE_INIT:

      // Get the dimensions 
      dWidth = IO_getWidth();
      dHeight = IO_getHeight();
      dMargin = 10;

      // Create the header
      String_keyAndStr(sTitleKey, sTitleFont, sTitle);
      AssetManager_createTextAsset(this->pSharedAssetManager, sTitle, sTitleFont);

      // Create divider
      sDividerText = String_repeat("▄", dWidth - dMargin * 2);

      // Create component tree
      Page_addComponentContext(this, sLoginComponent, "root", 0, 0, dWidth, dHeight, "primary", "secondary");
      Page_addComponentContainer(this, sLoginFormComponent, sLoginComponent, dMargin, dMargin / 2);
      Page_addComponentAsset(this, sTitleComponent, sLoginFormComponent, -1, 1, "", "", sTitleKey);
      Page_addComponentText(this, sDividerComponent, sLoginFormComponent, 0, 0, "accent", "", sDividerText);
      Page_addComponentText(this, sProfileListComponent, sLoginComponent, 87, dMargin + 1, "primary-darken-0.5", "", "");
      Page_addComponentContainer(this, sFieldContainerComponent, sLoginFormComponent, -1, 2);
      Page_addComponentText(this, sUsernamePromptComponent, sFieldContainerComponent, 1, 0, "", "", "Enter username:");
      Page_addComponentText(this, sUsernameComponent, sFieldContainerComponent, 1, 0, "", "", "");
      Page_addComponentText(this, sPasswordPromptComponent, sFieldContainerComponent, 1, 1, "", "", "Enter password:");
      Page_addComponentText(this, sPasswordComponent, sFieldContainerComponent, 1, 0, "", "", "");
      Page_addComponentText(this, sErrorPromptComponent, sFieldContainerComponent, 1, 2, "primary-darken-0.75", "secondary", "type ANY username to create a new profile\ntype EXIT to terminate the program");
      Page_addComponentText(this, sFieldPromptComponent, sLoginComponent, dWidth - dMargin - 1, dHeight - dMargin / 2, "primary-darken-0.5", "", "[tab]    to switch between fields\n[enter]  to submit\n[esc]    to delete a profile");      
      Page_addComponentPopup(this, sPopupComponent, dWidth / 2, dHeight / 2, 56, 14, "secondary", "accent", "", "", "");

      // Garbage collection
      String_kill(sDividerText);

      // Define initial user states
      if(Page_getUserState(this, "login-current-field") == -1) Page_setUserState(this, "login-current-field", cLoginCurrentField);
      if(Page_getUserState(this, "login-field-count") == -1) Page_setUserState(this, "login-field-count", cLoginFieldCount);
    break;

    case PAGE_ACTIVE_RUNNING:

      // Key handling
      cLoginCurrentField = Page_getUserState(this, "login-current-field");
      cLoginFieldCount = Page_getUserState(this, "login-field-count");
      cKeyPressed = EventStore_get(this->pSharedEventStore, "key-pressed");

      // Retrieve the user input 
      sUsernameField = String_toUpper(EventStore_getString(this->pSharedEventStore, "username-input"));
      sPasswordField = String_toUpper(EventStore_getString(this->pSharedEventStore, "password-input"));

      // There's a popup
      if(Page_getUserState(this, "is-popup")) {

        // Switch between options
        if(cKeyPressed == '\t') {
          Page_toggleComponentPopup(this, sPopupComponent, "secondary", "accent");

        // Submit popup
        } else if(cKeyPressed == '\n' || cKeyPressed == '\r') {
          Page_disableComponentPopup(this, sPopupComponent);

          // Switch between which popup is active
          switch(Page_getUserState(this, "popup-action")) {
            
            // Deleting a profile
            case 0:

              // Yes was selected
              if(Page_readComponentPopup(this, sPopupComponent) == 0) {
              
                // If successful
                if(Profile_delete(pProfile, sUsernameField)) {
                  EventStore_clearString(this->pSharedEventStore, "username-input");
                  EventStore_clearString(this->pSharedEventStore, "password-input");
                  Page_setComponentText(this, sErrorPromptComponent, "Success: profile deleted.");

                // Not successful
                } else {
                  Page_setComponentText(this, sErrorPromptComponent, Profile_getErrorMessage(pProfile));
                }

                Page_setComponentColor(this, sErrorPromptComponent, "secondary", "accent");
              }
            break;
            
            // Creating a new profile
            case 1:

              // Yes was selected
              if(Page_readComponentPopup(this, sPopupComponent) == 0) {

                // Check if success
                if(Profile_register(pProfile, sUsernameField, sPasswordField)) {
                  EventStore_clearString(this->pSharedEventStore, "username-input");
                  EventStore_clearString(this->pSharedEventStore, "password-input");
                  Page_setComponentText(this, sErrorPromptComponent, "Success: profile created.");

                // Otherwise, display new error
                } else {
                  Page_setComponentText(this, sErrorPromptComponent, Profile_getErrorMessage(pProfile));
                }

                Page_setComponentColor(this, sErrorPromptComponent, "secondary", "accent");
              }
            break;
          }
        }

      // Proceed with other input handling
      } else {

        // Switch based on what key was last pressed
        switch(cKeyPressed) {

          // Switch fields
          case '\t':
            Page_setUserState(this, "login-current-field", ((int) cLoginCurrentField + 1) % (int) cLoginFieldCount);
          break;

          // Do input checking, then go to menu if successful
          case '\n': case '\r': case 27:
            
            // Exits the program when inputting quit or exit
            if(!strcmp(sUsernameField, "QUIT") || !strcmp(sUsernameField, "quit") ||
              !strcmp(sUsernameField, "EXIT") || !strcmp(sUsernameField, "exit")) {
              EventStore_set(this->pSharedEventStore, "terminate", 'y');
            }
            
            // Some fields are empty
            if(!strlen(sUsernameField) ||
              !strlen(sPasswordField)) {
              Page_setComponentText(this, sErrorPromptComponent, "Error: some fields are empty.");
              Page_setComponentColor(this, sErrorPromptComponent, "secondary", "accent");
              
            // If successful
            } else {
              
              // Login was successful
              if(Profile_login(pProfile, sUsernameField, sPasswordField)) {
                
                // Deleting account tho
                if(cKeyPressed == 27) {
                  Page_enableComponentPopup(this, sPopupComponent);
                  Page_setComponentPopupText(this, sPopupComponent, "Are.you.sure.you.want.to.delete.the.profile?\nThis.action.cannot.be.undone.");
                  Page_setComponentPopupOptions(this, sPopupComponent, "yes", "no.", "secondary", "accent");
                  Page_setUserState(this, "popup-action", 0);
                  
                  // Clear error
                  Profile_setErrorId(pProfile, PROFILE_ERROR_NONE);

                // Logging in
                } else {

                  // Read the profile data
                  Stats_readProfile(pProfile);

                  Page_idle(this);
                  Page_setNext(this, "menu");
                }
              
              // Login was NOT successful
              } else {
                Page_setComponentText(this, sErrorPromptComponent, Profile_getErrorMessage(pProfile));
                Page_setComponentColor(this, sErrorPromptComponent, "secondary", "accent");

                // Create a new account perhaps?
                if(Profile_getErrorId(pProfile) == PROFILE_ERROR_NOT_FOUND && cKeyPressed != 27) {
                  Page_enableComponentPopup(this, sPopupComponent);
                  Page_setComponentPopupText(this, sPopupComponent, "This.account.does.not.yet.exist.\nWould.you.like.to.register.it?");
                  Page_setComponentPopupOptions(this, sPopupComponent, "yes", "no.", "secondary", "accent");
                  Page_setUserState(this, "popup-action", 1);
                }
              }
            }

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

            // Clear the error
            if(cKeyPressed) {
              Page_setComponentText(this, sErrorPromptComponent, "type ANY username to create a new profile\ntype EXIT to terminate the program");
              Page_setComponentColor(this, sErrorPromptComponent, "primary-darken-0.75", "secondary");
            }

          break;
        }
      }
      
      // List all the profiles
      sProfileListText = String_alloc(1024);

      // Grab the profile list
      Profile_getList(&nProfileListLength, sProfileListArray);
      strcat(sProfileListText, "Available profiles: \n\n");

      // None available
      if(!nProfileListLength)
        strcat(sProfileListText, "(none)");

      // Iterate through the profiles
      for(i = 0; i < nProfileListLength; i++) {
        strcat(sProfileListText, sProfileListArray[i]);
        strcat(sProfileListText, "\n");
      }

      // Update the UI text
      Page_setComponentText(this, sProfileListComponent, sProfileListText);

      // Indicate the user input on screen
      Page_setComponentText(this, sUsernameComponent, strlen(sUsernameField) ? sUsernameField : "____________________");
      Page_setComponentText(this, sPasswordComponent, strlen(sPasswordField) ? sPasswordField : "____________________");

      // Garbage collection
      String_kill(sProfileListText);

    break;

    default: break;
  }
}

#endif