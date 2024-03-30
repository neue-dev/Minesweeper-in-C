/**
 * @ Author: MMMM
 * @ Create Time: 2024-02-25 15:06:24
 * @ Modified time: 2024-03-30 12:08:33
 * @ Description:
 * 
 * This file defines the page handler for the page where the user can actually edit a minesweeper game
 */

#ifndef PAGE_EDITOR_INTERACTIVE_
#define PAGE_EDITOR_INTERACTIVE_

#include "../game/game.c"
#include "../game/editor.game.c"

#include "../utils/utils.asset.h"
#include "../utils/utils.page.h"
#include "../utils/utils.component.h"

/**
 * Configures the main menu.
 * 
 * @param   { p_obj }   pArgs_Page  The page instance we need to configure.
*/
void PageHandler_editorI(p_obj pArgs_Page) {

  Page *this = (Page *) pArgs_Page;
  Game *pGame = (Game *) this->pSharedObject;
  int dWidth, dHeight, dMargin, x, y;

  // Component names
  char *sEditorIComponent = "editor-i.fixed";
  char *sLefterComponent = "lefter.fixed.aright-x.atop-y";
  char *sFooterComponent = "footer.fixed.aleft-x.atop-y";
  char *sFieldContainerComponent = "field-container.fixed";
  char *sFieldComponent = "field.acenter-x.acenter-y";
  char *sFieldCursorComponent = "field-cursor.aleft-x.atop-y";
  char *sEditorPromptComponent = "editor-prompt.fixed.aleft-x.atop-y";
  char *sEditorInfoComponent = "editor-info.fixed.aleft-x.atop-y";
  char *sPopupComponent = "popup.fixed";

  // For inspect components
  char sMineKey[STRING_KEY_MAX_LENGTH];

  // Some of the component contents
  char sEditorInfoText[STRING_KEY_MAX_LENGTH];
  char sEditorPromptText[STRING_KEY_MAX_LENGTH];

  // Buffer for minesweeper grid 
  char *sGridBuffer;

  // Pressed key
  char cKeyPressed = 0;

  // Do stuff based on page status
  switch(this->ePageStatus) {

    case PAGE_ACTIVE_INIT:

      // Get the dimensions 
      dWidth = IO_getWidth();
      dHeight = IO_getHeight();
      dMargin = 3;

      // Create component tree
      Page_addComponentContext(this, sEditorIComponent, "root", 0, 0, dWidth, dHeight, "primary", "secondary");
      Page_addComponentContainer(this, sLefterComponent, sEditorIComponent, dWidth / 2 + Game_getCharWidth(pGame) / 2 + dMargin * 2, dHeight / 2 - Game_getCharHeight(pGame) / 2 - 1);
      Page_addComponentContainer(this, sFooterComponent, sEditorIComponent, dWidth / 2 - Game_getCharWidth(pGame) / 2, dHeight / 2 + Game_getCharHeight(pGame) / 2 + dMargin / 2);
      Page_addComponentContainer(this, sFieldContainerComponent, sEditorIComponent, dWidth / 2, dHeight / 2);
      Page_addComponentText(this, sFieldComponent, sFieldContainerComponent, 0, 0, "primary-darken-0.75", "", "");
      Page_addComponentAsset(this, sFieldCursorComponent, sFieldComponent, 0, 0, "accent", "", "field-cursor");
      // Page_addComponentText(this, sProfileInfoComponent, sLefterComponent, 0, 0, "", "", "");
      // Page_addComponentText(this, sGamePromptComponent, sFooterComponent, 0, 0, "", "", "");
      Page_addComponentPopup(this, sPopupComponent, dWidth / 2, dHeight / 2, 56, 14, "secondary", "accent", "", "", "");

      // This is stupid but LMAO
      for(x = 0; x < pGame->field.dWidth; x++) {
        for(y = 0; y < pGame->field.dHeight; y++) {

          // The key
          sprintf(sMineKey, "flag-%d-%d", x, y);

          // Create the component
          Page_addComponentText(this, 
            sMineKey, 
            sFieldContainerComponent, 
            
            x * GAME_CELL_WIDTH - Game_getCharWidth(pGame) / 2 + 1, 
            y * GAME_CELL_HEIGHT - Game_getCharHeight(pGame) / 2,  
            
            "accent2", "", "");

          sprintf(sMineKey, "flag-darken-%d-%d", x, y);

          Page_addComponentText(this, 
            sMineKey, 
            sFieldContainerComponent, 
            
            x * GAME_CELL_WIDTH - Game_getCharWidth(pGame) / 2 + 1, 
            y * GAME_CELL_HEIGHT - Game_getCharHeight(pGame) / 2,  
            
            "accent2-darken-0.36", "", " ");
        }
      }

      // Display the actual grid
      sGridBuffer = String_alloc(Game_getCharWidth(pGame) * Game_getCharHeight(pGame) * 4);
      Game_displayGrid(pGame, sGridBuffer);
      Page_setComponentText(this, sFieldComponent, sGridBuffer);
      String_kill(sGridBuffer);
    break;

    case PAGE_ACTIVE_RUNNING:
      
      // Key handling
      cKeyPressed = EventStore_get(this->pSharedEventStore, "key-pressed");

      // If no popup is active
      if(Page_getUserState(this, "is-popup")) {

        // Switch between options
        if(cKeyPressed == '\t')
          Page_toggleComponentPopup(this, sPopupComponent, "secondary", "accent");

        // Submit popup
        else if(cKeyPressed == '\n' || cKeyPressed == '\r') {
          Page_disableComponentPopup(this, sPopupComponent);

          // First option
          if(Page_readComponentPopup(this, sPopupComponent) == 0) {

            // Reset component tree since the game UI needs that
            Page_resetComponents(this);
            Page_idle(this);
            Page_setNext(this, "menu");

            // ! save the game here

            // Make sure the function doesn't try to access the borked component tree down there.
            return;
          
          // Second option
          } else {

            // Reset component tree since the game UI needs that
            Page_resetComponents(this);

            // Go to menu next
            Page_idle(this);
            Page_setNext(this, "menu");
          }
        }

      } else {

        // Switch based on what key was last pressed
        switch(cKeyPressed) {

          // Escape character to go back
          case 27:
            Page_enableComponentPopup(this, sPopupComponent);
            Page_setComponentPopupText(this, sPopupComponent, "Clear.the.grid?");
            Page_setComponentPopupOptions(this, sPopupComponent, "yes", "no.");

            // CLear mines
            // !
          break;

          // Save to file or discard
          case '\n': case '\r':
            Page_enableComponentPopup(this, sPopupComponent);
            Page_setComponentPopupText(this, sPopupComponent, "Exit.and.save.the.file?");
            Page_setComponentPopupOptions(this, sPopupComponent, "yes", "no.");
          break;

          default:

            // WASD movement
            if(cKeyPressed == tolower(Settings_getGameMoveUp(this->pSharedEventStore)) ||
              cKeyPressed == toupper(Settings_getGameMoveUp(this->pSharedEventStore)))
              Game_decrementY(pGame);
              
            if(cKeyPressed == tolower(Settings_getGameMoveDown(this->pSharedEventStore)) ||
              cKeyPressed == toupper(Settings_getGameMoveDown(this->pSharedEventStore)))
              Game_incrementY(pGame);

            if(cKeyPressed == tolower(Settings_getGameMoveLeft(this->pSharedEventStore)) ||
              cKeyPressed == toupper(Settings_getGameMoveLeft(this->pSharedEventStore)))
              Game_decrementX(pGame);
            
            if(cKeyPressed == tolower(Settings_getGameMoveRight(this->pSharedEventStore)) ||
              cKeyPressed == toupper(Settings_getGameMoveRight(this->pSharedEventStore)))
              Game_incrementX(pGame);

            // Mine placement
            if(cKeyPressed == tolower(Settings_getGameToggleFlag(this->pSharedEventStore)) ||
              cKeyPressed == toupper(Settings_getGameToggleFlag(this->pSharedEventStore))) {

              // If does not have a flag
              if(!Grid_getBit(pGame->field.pMineGrid, pGame->dCursorX, pGame->dCursorY))
                Editor_addMine(pGame);
              
              // If already has a flag
              else Editor_removeMine(pGame);
            }
            
            // For each cell, check if it's been inspected, and if so, change color
            for(x = 0; x < pGame->field.dWidth; x++) {
              for(y = 0; y < pGame->field.dHeight; y++) {
                
                // The key
                sprintf(sMineKey, "flag-%d-%d", x, y);
                
                // If there's a flag on it
                if(Grid_getBit(pGame->field.pMineGrid, x, y))
                  Page_setComponentText(this, sMineKey, "╶╳╴");
                
                // Remove flag if it exists
                else
                  Page_setComponentText(this, sMineKey, "");
              }
            }

          break;
        }

        // Display the actual grid
        sGridBuffer = String_alloc(Game_getCharWidth(pGame) * Game_getCharHeight(pGame) * 4);
        Game_displayGrid(pGame, sGridBuffer);
        Page_setComponentText(this, sFieldComponent, sGridBuffer);
        String_kill(sGridBuffer);

        // Update UI
        Page_setComponentPos(this, sFieldCursorComponent, 
          pGame->dCursorX * GAME_CELL_WIDTH, 
          pGame->dCursorY * GAME_CELL_HEIGHT);

        // Game information text
        // sprintf(sGameInfoText, "frame rate:      %s\ntime elapsed:    %s\nmines left:      %s\n",
        //   Game_getFPS(pGame),
        //   Game_getTime(pGame),
        //   Game_getMinesLeft(pGame));
        // Page_setComponentText(this, sGameInfoComponent, sGameInfoText);

        // // Player information text
        // sprintf(sProfileInfoText, "%s\n%s, %s\n%s (best)",
        //   Profile_getCurrent(pProfile),
        //   "classic", "difficult",     // ! change this
        //   "0:59");                    // ! and this
        // Page_setComponentText(this, sProfileInfoComponent, sProfileInfoText);

        // // Prompt text
        // sprintf(sGamePromptText, "[%s%s%s%s]   to move\n[enter]  to inspect a tile\n[%s]      to place a flag\n[esc]    to go back to menu",
        //   String_renderEscChar(Settings_getGameMoveUp(this->pSharedEventStore)),
        //   String_renderEscChar(Settings_getGameMoveLeft(this->pSharedEventStore)),
        //   String_renderEscChar(Settings_getGameMoveDown(this->pSharedEventStore)),
        //   String_renderEscChar(Settings_getGameMoveRight(this->pSharedEventStore)),
        //   String_renderEscChar(Settings_getGameToggleFlag(this->pSharedEventStore)));
        // Page_setComponentText(this, sGamePromptComponent, sGamePromptText);
      }

    break;

    default:

    break;
  }
}

#endif