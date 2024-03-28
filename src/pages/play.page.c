/**
 * @ Author: MMMM
 * @ Create Time: 2024-02-25 15:06:24
 * @ Modified time: 2024-03-29 01:14:50
 * @ Description:
 * 
 * This file defines the page handler for the help page.
 */

#ifndef PAGE_PLAY_
#define PAGE_PLAY_

#include "../game/game.c"

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
  Game *pGame = (Game *) this->pSharedObject;
  int dWidth, dHeight, dMargin;

  // Header details
  char *sHeader = "play a game";
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

  // Input fields
  char *sTypeField;
  char *sFileordiffField;
  char cPlayCurrentField = 0;
  char cPlayFieldCount = 2;

  // Do stuff based on page status
  switch(this->ePageStatus) {

    case PAGE_ACTIVE_INIT:

      // Get the dimensions 
      dWidth = IO_getWidth();
      dHeight = IO_getHeight();
      dMargin = 42;

      // Create the header
      String_keyAndStr(sHeaderKey, sHeaderFont, sHeader);
      AssetManager_createTextAsset(this->pSharedAssetManager, sHeader, sHeaderFont);

      // Create component tree
      Page_addComponentContext(this, sPlayComponent, "root", 0, 0, dWidth, dHeight, "primary", "secondary");
      Page_addComponentContainer(this, sPlayFormComponent, sPlayComponent, 0, 0);
      Page_addComponentAsset(this, sHeaderComponent, sPlayFormComponent, dWidth / 2, 6, "", "", sHeaderKey);
      Page_addComponentContainer(this, sFieldContainerComponent, sPlayFormComponent, dWidth / 2 - dMargin / 2, 4);
      Page_addComponentText(this, sTypePromptComponent, sFieldContainerComponent, 1, 0, "", "", "Enter game type:");
      Page_addComponentText(this, sTypeComponent, sFieldContainerComponent, 1, 0, "", "", "");
      Page_addComponentText(this, sFileordiffPromptComponent, sFieldContainerComponent, 1, 1, "", "", "Enter difficulty - if classic:\n      filename   - if custom:");
      Page_addComponentText(this, sFileordiffComponent, sFieldContainerComponent, 1, 0, "", "", "");
      Page_addComponentText(this, sErrorPromptComponent, sFieldContainerComponent, 1, 1, "secondary", "accent", "");
      Page_addComponentText(this, sFieldPromptComponent, sFieldContainerComponent, 1, 1, "primary-darken-0.5", "", "[tab] to switch fields; [enter] to submit");
      
      // Define initial user states
      if(Page_getUserState(this, "play-current-field") == -1) Page_setUserState(this, "play-current-field", cPlayCurrentField);
      if(Page_getUserState(this, "play-field-count") == -1) Page_setUserState(this, "play-field-count", cPlayFieldCount);
      
    break;

    case PAGE_ACTIVE_RUNNING:
      
      // Key handling
      cPlayCurrentField = Page_getUserState(this, "play-current-field");
      cPlayFieldCount = Page_getUserState(this, "play-field-count");

      // Retrieve the user input 
      sTypeField = EventStore_getString(this->pSharedEventStore, "type-input");
      sFileordiffField = EventStore_getString(this->pSharedEventStore, "fileordiff-input");

      // Switch based on what key was last pressed
      switch(EventStore_get(this->pSharedEventStore, "key-pressed")) {

        // Escape character to go back
        case 27:
          Page_idle(this);
          Page_setNext(this, "menu");
        break;

        // Switch fields
        case '\t':
          Page_setUserState(this, "play-current-field", ((int) cPlayCurrentField + 1) % (int) cPlayFieldCount);
        break;

        // Do input checking then go to level editor when valid
        case '\n': case '\r':

          // If one of the fields are empty
          if(!strlen(sTypeField) || 
            !strlen(sFileordiffField)) {
            Page_setComponentText(this, sErrorPromptComponent, "Error: some fields are empty.");

          // If invalid mode
          } else if(strcmp(sTypeField, "classic") && strcmp(sTypeField, "custom")) {
            Page_setComponentText(this, sErrorPromptComponent, "Error: invalid game type; check whitespaces.");

          // If classic mode
          } else if(!strcmp(sTypeField, "classic")) {
            
            // It's not easy or difficult
            if(strcmp(sFileordiffField, "easy") && 
              strcmp(sFileordiffField, "difficult")) {
              Page_setComponentText(this, sErrorPromptComponent, "Error: classic has only [easy/difficult].");              
            
            // Proceed to a classic game
            } else {
              if(!strcmp(sFileordiffField, "easy")) Game_setup(pGame, GAME_TYPE_CLASSIC, GAME_DIFFICULTY_EASY);
              if(!strcmp(sFileordiffField, "difficult")) Game_setup(pGame, GAME_TYPE_CLASSIC, GAME_DIFFICULTY_DIFFICULT);
              Game_init(pGame);

              Page_idle(this);
              Page_setNext(this, "play-i"); 
            }
          
          // If custom mode
          } else if(!strcmp(sTypeField, "custom")) {
            
            // Check for valid filename
            if(!String_isValidFilename(sFileordiffField)) {
              Page_setComponentText(this, sErrorPromptComponent, "Error: invalid filename for custom game.");              
            
            // Check if file exists
            // ! todo
            } else if(0) {
              Page_setComponentText(this, sErrorPromptComponent, "Error: file not found for custom game.");              

            // Proceed to custom game
            } else {
              // ! todo, configure game here
              Page_idle(this);
              Page_setNext(this, "menu"); 
            }
          }
        break;

        default:
        
          // Update the values of the current inputted username
          if(cPlayCurrentField == 0) { 
            EventStore_setString(this->pSharedEventStore, "key-pressed", "type-input");

            Page_setComponentColor(this, sTypePromptComponent, "primary", "");
            Page_setComponentColor(this, sTypeComponent, "accent", "");
            Page_setComponentColor(this, sFileordiffPromptComponent, "primary-darken-0.75", "");
            Page_setComponentColor(this, sFileordiffComponent, "primary-darken-0.75", "");

          // Update the values of the current inputted password
          } else if(cPlayCurrentField == 1) {
            EventStore_setString(this->pSharedEventStore, "key-pressed", "fileordiff-input");      

            Page_setComponentColor(this, sTypePromptComponent, "primary-darken-0.75", "");
            Page_setComponentColor(this, sTypeComponent, "primary-darken-0.75", "");
            Page_setComponentColor(this, sFileordiffPromptComponent, "primary", "");
            Page_setComponentColor(this, sFileordiffComponent, "accent", "");
          }

          // Clear the error
          if(EventStore_get(this->pSharedEventStore, "key-pressed"))
            Page_setComponentText(this, sErrorPromptComponent, "");

        break;
      }

      // Indicate the user input on screen
      Page_setComponentText(this, sTypeComponent, strlen(sTypeField) ? sTypeField : "[classic/custom]");
      Page_setComponentText(this, sFileordiffComponent, strlen(sFileordiffField) ? sFileordiffField : "[easy/difficult] OR <filename>");

    break;

    default:

    break;
  }
}

#endif