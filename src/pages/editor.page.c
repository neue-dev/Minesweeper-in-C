/**
 * @ Author: MMMM
 * @ Create Time: 2024-02-25 15:06:24
 * @ Modified time: 2024-03-31 20:59:54
 * @ Description:
 * 
 * This file defines the page handler for the editor page.
 */

#ifndef PAGE_EDITOR_
#define PAGE_EDITOR_

#include "../game/game.c"
#include "../game/editor.game.c"

#include "../utils/utils.asset.h"
#include "../utils/utils.page.h"
#include "../utils/utils.string.h"
#include "../utils/utils.component.h"

/**
 * Configures the main menu.
 * 
 * @param   { p_obj }   pArgs_Page  The page instance we need to configure.
*/
void PageHandler_editor(p_obj pArgs_Page) {

  Page *this = (Page *) pArgs_Page;
  Game *pGame = (Game *) this->pSharedObject;
  int dWidth, dHeight, dMargin;

  // Title details
  char *sTitle = "level editor";
  char *sTitleFont = "body-font";
  char sTitleKey[STRING_KEY_MAX_LENGTH];

  // Component names
  char *sEditorComponent = "editor.fixed";
  char *sEditorFormComponent = "editor-form.col";
  char *sTitleComponent = "title.aleft-x.abottom-y";
  char *sDividerComponent = "divider.aleft-x.abottom-y";
  char *sFieldContainerComponent = "field-container.col.aleft-x.atop-y";
  char *sFilenamePromptComponent = "filename-prompt.aleft-x";
  char *sWidthPromptComponent = "width-prompt.aleft-x";
  char *sHeightPromptComponent = "height-prompt.aleft-x";
  char *sFilenameComponent = "filename.aleft-x";
  char *sWidthComponent = "width.aleft-x";
  char *sHeightComponent = "height.aleft-x";
  char *sFieldPromptComponent = "field-prompt.aright-x.abottom-y";
  char *sErrorPromptComponent = "error-prompt.aleft-x";

  // Input fields
  char *sFilenameField;
  char *sWidthField;
  char *sHeightField;
  char cEditorCurrentField = 0;
  char cEditorFieldCount = 3;

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
      Page_addComponentContext(this, sEditorComponent, "root", 0, 0, dWidth, dHeight, "primary", "secondary");
      Page_addComponentContainer(this, sEditorFormComponent, sEditorComponent, dMargin, dMargin / 2);
      Page_addComponentAsset(this, sTitleComponent, sEditorFormComponent, -1, 1, "", "", sTitleKey);
      Page_addComponentText(this, sDividerComponent, sEditorFormComponent, 0, 0, "accent", "", sDividerText);
      Page_addComponentContainer(this, sFieldContainerComponent, sEditorFormComponent, -1, 2);
      Page_addComponentText(this, sFilenamePromptComponent, sFieldContainerComponent, 1, 0, "", "", "Enter filename:");
      Page_addComponentText(this, sFilenameComponent, sFieldContainerComponent, 1, 0, "", "", "");
      Page_addComponentText(this, sWidthPromptComponent, sFieldContainerComponent, 1, 1, "", "", "Enter number of cols (5-15):");
      Page_addComponentText(this, sWidthComponent, sFieldContainerComponent, 1, 0, "", "", "");
      Page_addComponentText(this, sHeightPromptComponent, sFieldContainerComponent, 1, 1, "", "", "Enter number of rows (5-10):");
      Page_addComponentText(this, sHeightComponent, sFieldContainerComponent, 1, 0, "", "", "");
      Page_addComponentText(this, sErrorPromptComponent, sFieldContainerComponent, 1, 3, "secondary", "accent", "");
      Page_addComponentText(this, sFieldPromptComponent, sEditorComponent, dWidth - dMargin - 1, dHeight - dMargin / 2, "primary-darken-0.5", "", "[tab]    to switch between fields\n[enter]  to submit\n[esc]   to go back");
      
      // Garbage collection
      String_kill(sDividerText);

      // Define initial user states
      if(Page_getUserState(this, "editor-current-field") == -1) Page_setUserState(this, "editor-current-field", cEditorCurrentField);
      if(Page_getUserState(this, "editor-field-count") == -1) Page_setUserState(this, "editor-field-count", cEditorFieldCount);
    break;

    case PAGE_ACTIVE_RUNNING:
      
      // Key handling
      cEditorCurrentField = Page_getUserState(this, "editor-current-field");
      cEditorFieldCount = Page_getUserState(this, "editor-field-count");

      // Retrieve the user input 
      sFilenameField = String_toUpper(EventStore_getString(this->pSharedEventStore, "filename-input"));
      sWidthField = String_toUpper(EventStore_getString(this->pSharedEventStore, "width-input"));
      sHeightField = String_toUpper(EventStore_getString(this->pSharedEventStore, "height-input"));

      // Switch based on what key was last pressed
      switch(EventStore_get(this->pSharedEventStore, "key-pressed")) {

        // Escape character to go back
        case 27:
          Page_idle(this);
          Page_setNext(this, "menu");
        break;

        // Switch fields
        case '\t':
          Page_setUserState(this, "editor-current-field", ((int) cEditorCurrentField + 1) % (int) cEditorFieldCount);
        break;

        // Do input checking then go to level editor when valid
        case '\n': case '\r':

          // If one of the fields are empty
          if(!strlen(sFilenameField) || 
            !strlen(sWidthField) || 
            !strlen(sHeightField)) {
            Page_setComponentText(this, sErrorPromptComponent, "Error: some fields are empty.");

          // If the filename is invalid            
          } else if(!String_isValidFilename(sFilenameField)) {
            Page_setComponentText(this, sErrorPromptComponent, "Error: filename has invalid characters.");

          // If the width or height is invalid
          } else if(!atoi(sWidthField) || !atoi(sHeightField)) {
            Page_setComponentText(this, sErrorPromptComponent, "Error: invalid width or height.");

          // If the width or height is out of range
          } else if(
            atoi(sWidthField) > GAME_MAX_COLUMNS || atoi(sWidthField) < GAME_MIN_COLUMNS || 
            atoi(sHeightField) > GAME_MAX_ROWS || atoi(sHeightField) < GAME_MIN_ROWS) {
            Page_setComponentText(this, sErrorPromptComponent, "Error: width or height is out of range.");

          // Duplicate name
          } else if(Editor_levelExists(pGame, sFilenameField)) {
            Page_setComponentText(this, sErrorPromptComponent, "Error: level file already exists.");
            
          // Too many level files are registered
          } else if(!Editor_levelAddable(pGame, sFilenameField)) {
            Page_setComponentText(this, sErrorPromptComponent, "Error: too many level files exist already.");

          // If all the information is valid
          } else {
            Editor_setup(pGame);
            Editor_setSaveName(pGame, sFilenameField);
            Editor_init(pGame, atoi(sWidthField), atoi(sHeightField));

            Page_idle(this);
            Page_setNext(this, "editor-i"); 
          }
        break;

        default:
        
          // Update the values of the current inputted username
          if(cEditorCurrentField == 0) { 
            EventStore_setString(this->pSharedEventStore, "key-pressed", "filename-input");

            Page_setComponentColor(this, sFilenamePromptComponent, "primary", "");
            Page_setComponentColor(this, sFilenameComponent, "accent", "");
            Page_setComponentColor(this, sWidthPromptComponent, "primary-darken-0.75", "");
            Page_setComponentColor(this, sWidthComponent, "primary-darken-0.75", "");
            Page_setComponentColor(this, sHeightPromptComponent, "primary-darken-0.75", "");
            Page_setComponentColor(this, sHeightComponent, "primary-darken-0.75", "");

          // Update the values of the current inputted password
          } else if(cEditorCurrentField == 1) {
            EventStore_setString(this->pSharedEventStore, "key-pressed", "width-input");      

            Page_setComponentColor(this, sFilenamePromptComponent, "primary-darken-0.75", "");
            Page_setComponentColor(this, sFilenameComponent, "primary-darken-0.75", "");
            Page_setComponentColor(this, sWidthPromptComponent, "primary", "");
            Page_setComponentColor(this, sWidthComponent, "accent", "");    
            Page_setComponentColor(this, sHeightPromptComponent, "primary-darken-0.75", "");
            Page_setComponentColor(this, sHeightComponent, "primary-darken-0.75", "");      
          
          } else if(cEditorCurrentField == 2) {
            EventStore_setString(this->pSharedEventStore, "key-pressed", "height-input");      

            Page_setComponentColor(this, sFilenamePromptComponent, "primary-darken-0.75", "");
            Page_setComponentColor(this, sFilenameComponent, "primary-darken-0.75", "");
            Page_setComponentColor(this, sWidthPromptComponent, "primary-darken-0.75", "");
            Page_setComponentColor(this, sWidthComponent, "primary-darken-0.75", "");    
            Page_setComponentColor(this, sHeightPromptComponent, "primary", "");
            Page_setComponentColor(this, sHeightComponent, "accent", "");     
          }

          // Clear the error
          if(EventStore_get(this->pSharedEventStore, "key-pressed"))
            Page_setComponentText(this, sErrorPromptComponent, "");

        break;
      }

      // Indicate the user input on screen
      Page_setComponentText(this, sFilenameComponent, strlen(sFilenameField) ? sFilenameField : "____________________");
      Page_setComponentText(this, sWidthComponent, strlen(sWidthField) ? sWidthField : "____________________");
      Page_setComponentText(this, sHeightComponent, strlen(sHeightField) ? sHeightField : "____________________");

    break;

    default:

    break;
  }
}

#endif