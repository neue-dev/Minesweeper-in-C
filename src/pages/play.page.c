/**
 * @ Author: MMMM
 * @ Create Time: 2024-02-25 15:06:24
 * @ Modified time: 2024-03-31 21:17:02
 * @ Description:
 * 
 * This file defines the page handler for the help page.
 */

#ifndef PAGE_PLAY_
#define PAGE_PLAY_

#include "../game/game.c"
#include "../game/editor.game.c"

#include "../utils/utils.asset.h"
#include "../utils/utils.page.h"
#include "../utils/utils.component.h"

#include <string.h>

/**
 * Configures the main menu.
 * 
 * @param   { p_obj }   pArgs_Page  The page instance we need to configure.
*/
void PageHandler_play(p_obj pArgs_Page) {

  Page *this = (Page *) pArgs_Page;
  Game *pGame = (Game *) this->pSharedObject;
  int dWidth, dHeight, dMargin;

  // Title details
  char *sTitle = "play a game";
  char *sTitleFont = "body-font";
  char sTitleKey[STRING_KEY_MAX_LENGTH];

  // Component names
  char *sPlayComponent = "play.fixed";
  char *sPlayFormComponent = "play-form.col";
  char *sTitleComponent = "title.aleft-x.abottom-y";
  char *sDividerComponent = "divider.aleft-x.abottom-y";
  char *sFieldContainerComponent = "field-container.col.aleft-x.atop-y";
  char *sFileordiffPromptComponent = "fileordiff-prompt.aleft-x";
  char *sTypePromptComponent = "type-prompt.aleft-x";
  char *sFileordiffComponent = "fileordiff.aleft-x";
  char *sTypeComponent = "type.aleft-x";
  char *sFieldPromptComponent = "field-prompt.aright-x.abottom-y";
  char *sErrorPromptComponent = "error-prompt.aleft-x";

  // Input fields
  char *sTypeField;
  char *sFileordiffField;
  char cPlayCurrentField = 0;
  char cPlayFieldCount = 2;

  // Divider text
  char *sDividerText;

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
      Page_addComponentContext(this, sPlayComponent, "root", 0, 0, dWidth, dHeight, "primary", "secondary");
      Page_addComponentContainer(this, sPlayFormComponent, sPlayComponent, dMargin, dMargin / 2);
      Page_addComponentAsset(this, sTitleComponent, sPlayFormComponent, -1, 1, "", "", sTitleKey);
      Page_addComponentText(this, sDividerComponent, sPlayFormComponent, 0, 0, "accent", "", sDividerText);
      Page_addComponentContainer(this, sFieldContainerComponent, sPlayFormComponent, -1, 2);
      Page_addComponentText(this, sTypePromptComponent, sFieldContainerComponent, 1, 0, "", "", "Enter game type:");
      Page_addComponentText(this, sTypeComponent, sFieldContainerComponent, 1, 0, "", "", "");
      Page_addComponentText(this, sFileordiffPromptComponent, sFieldContainerComponent, 1, 1, "", "", "Enter difficulty or filename:");
      Page_addComponentText(this, sFileordiffComponent, sFieldContainerComponent, 1, 0, "", "", "");
      Page_addComponentText(this, sErrorPromptComponent, sFieldContainerComponent, 1, 3, "primary-darken-0.75", "secondary", "> type CLASSIC or CUSTOM under game type           \n\n> type EASY or DIFFICULT under difficulty (CLASSIC)\n> type FILENAME          under filename   (CUSTOM)");
      Page_addComponentText(this, sFieldPromptComponent, sPlayComponent, dWidth - dMargin - 1, dHeight - dMargin / 2, "primary-darken-0.5", "", "[tab]    to switch between fields\n[enter]  to submit\n[esc]   to go back");
      
      // Garbage collection
      String_kill(sDividerText);

      // Define initial user states
      if(Page_getUserState(this, "play-current-field") == -1) Page_setUserState(this, "play-current-field", cPlayCurrentField);
      if(Page_getUserState(this, "play-field-count") == -1) Page_setUserState(this, "play-field-count", cPlayFieldCount);
      
    break;

    case PAGE_ACTIVE_RUNNING:
      
      // Key handling
      cPlayCurrentField = Page_getUserState(this, "play-current-field");
      cPlayFieldCount = Page_getUserState(this, "play-field-count");

      // Retrieve the user input 
      sTypeField = String_toUpper(EventStore_getString(this->pSharedEventStore, "type-input"));
      sFileordiffField = String_toUpper(EventStore_getString(this->pSharedEventStore, "fileordiff-input"));

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
            Page_setComponentColor(this, sErrorPromptComponent, "secondary", "accent");

          // If invalid mode
          } else if(strcmp(sTypeField, "CLASSIC") && strcmp(sTypeField, "CUSTOM")) {
            Page_setComponentText(this, sErrorPromptComponent, "Error: invalid game type; check whitespaces.");
            Page_setComponentColor(this, sErrorPromptComponent, "secondary", "accent");

          // If classic mode
          } else if(!strcmp(sTypeField, "CLASSIC")) {
            
            // It's not easy or difficult
            if(strcmp(sFileordiffField, "EASY") && 
              strcmp(sFileordiffField, "DIFFICULT")) {
              Page_setComponentText(this, sErrorPromptComponent, "Error: CLASSIC has only EASY or DIFFICULT.");
              Page_setComponentColor(this, sErrorPromptComponent, "secondary", "accent");              
            
            // Proceed to a classic game
            } else {
              if(!strcmp(sFileordiffField, "EASY")) Game_setup(pGame, GAME_TYPE_CLASSIC, GAME_DIFFICULTY_EASY);
              if(!strcmp(sFileordiffField, "DIFFICULT")) Game_setup(pGame, GAME_TYPE_CLASSIC, GAME_DIFFICULTY_DIFFICULT);
              Game_init(pGame);

              Page_idle(this);
              Page_setNext(this, "play-i"); 
            }
          
          // If custom mode
          } else if(!strcmp(sTypeField, "CUSTOM")) {
            
            // Check for valid filename
            if(!String_isValidFilename(sFileordiffField)) {
              Page_setComponentText(this, sErrorPromptComponent, "Error: invalid filename for custom game.");   
              Page_setComponentColor(this, sErrorPromptComponent, "secondary", "accent");           
            
            // Check if file exists
            } else if(!Editor_levelExists(pGame, sFileordiffField)) {
              Page_setComponentText(this, sErrorPromptComponent, "Error: file not found for custom game.");    
              Page_setComponentColor(this, sErrorPromptComponent, "secondary", "accent");          

            // Proceed to custom game
            } else {
              Game_setup(pGame, GAME_TYPE_CUSTOM, GAME_DIFFICULTY_NONE);
              Editor_setSaveName(pGame, sFileordiffField);
              Editor_loadLevel(pGame);
              
              Page_idle(this);
              Page_setNext(this, "play-i"); 
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
          if(EventStore_get(this->pSharedEventStore, "key-pressed")) {
            Page_setComponentText(this, sErrorPromptComponent, "> type CLASSIC or CUSTOM under game type           \n\n> type EASY or DIFFICULT under difficulty (CLASSIC)\n> type FILENAME          under filename   (CUSTOM)");
            Page_setComponentColor(this, sErrorPromptComponent, "primary-darken-0.75", "secondary");
          }

        break;
      }

      // Indicate the user input on screen
      Page_setComponentText(this, sTypeComponent, strlen(sTypeField) ? sTypeField : "____________________");
      Page_setComponentText(this, sFileordiffComponent, strlen(sFileordiffField) ? sFileordiffField : "____________________");

    break;

    default:

    break;
  }
}

#endif