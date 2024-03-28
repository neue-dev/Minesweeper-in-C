/**
 * @ Author: MMMM
 * @ Create Time: 2024-02-25 15:06:24
 * @ Modified time: 2024-03-28 18:02:31
 * @ Description:
 * 
 * This file defines the page handler for the page where the user can actually play minesweeper
 */

#ifndef PAGE_PLAY_INTERACTIVE_
#define PAGE_PLAY_INTERACTIVE_

#include "../game/game.c"

#include "../utils/utils.asset.h"
#include "../utils/utils.page.h"
#include "../utils/utils.component.h"

#include "../settings.c"

#include <string.h>
#include <ctype.h>

/**
 * Configures the main menu.
 * 
 * @param   { p_obj }   pArgs_Page  The page instance we need to configure.
*/
void PageHandler_playI(p_obj pArgs_Page) {

  Page *this = (Page *) pArgs_Page;
  Game *pGame = (Game *) this->pSharedObject;
  int dWidth, dHeight, dMargin, x, y;

  // Component names
  char *sPlayIComponent = "play-i.fixed";
  char *sHeaderComponent = "header.fixed.acenter-x";
  char *sFooterComponent = "footer.fixed.acenter-x.atop-y";
  char *sFieldContainerComponent = "field-container.fixed";
  char *sFieldComponent = "field.acenter-x.acenter-y";
  char *sFieldInspectComponent = "field-inspect.acenter-x.acenter-y";
  char *sFieldCursorComponent = "field-cursor.aleft-x.atop-y";
  char *sGamePrompt = "game-prompt.fixed.acenter-x.abottom-y";

  // For inspect components
  char sInspectKey[STRING_KEY_MAX_LENGTH];

  // Prompt
  char sGamePromptText[STRING_KEY_MAX_LENGTH];
  char *sGridBuffer;

  // The cursor location for changing the mine field
  char cCursorX = 0;
  char cCursorY = 0;

  // Pressed key
  char cKeyPressed = 0;
  
  // Do stuff based on page status
  switch(this->ePageStatus) {

    case PAGE_ACTIVE_INIT:

      // Get the dimensions 
      dWidth = IO_getWidth();
      dHeight = IO_getHeight();
      dMargin = 4;
      
      // Create component tree
      Page_addComponentContext(this, sPlayIComponent, "root", 0, 0, dWidth, dHeight, "primary", "secondary");
      Page_addComponentContainer(this, sHeaderComponent, sPlayIComponent, dWidth / 2, 0);
      Page_addComponentContainer(this, sFooterComponent, sPlayIComponent, dWidth / 2, dHeight - dMargin);
      Page_addComponentContainer(this, sFieldContainerComponent, sPlayIComponent, dWidth / 2, dHeight / 2);
      Page_addComponentText(this, sFieldComponent, sFieldContainerComponent, 0, 0, "primary-darken-0.75", "", "");
      Page_addComponentAsset(this, sFieldCursorComponent, sFieldComponent, 0, 0, "accent", "", "field-cursor");
      Page_addComponentText(this, sGamePrompt, sFooterComponent, 0, 0, "", "", "[]");

      // Define initial user states
      if(Page_getUserState(this, "play-i-cursor-x") == -1) Page_setUserState(this, "play-i-cursor-x", cCursorX);
      if(Page_getUserState(this, "play-i-cursor-y") == -1) Page_setUserState(this, "play-i-cursor-y", cCursorY);

      // Initialize the game
      // ! move this elsewhere?
      Game_init(pGame);
      Gameplay_initClassic(GAMEPLAY_DIFFICULTY_EASY, &pGame->gameField);

      // Display the actual grid
      sGridBuffer = String_alloc(Game_getCharWidth(pGame) * Game_getCharHeight(pGame) * 4);
      Game_displayGrid(pGame, sGridBuffer);
      Page_setComponentText(this, sFieldComponent, sGridBuffer);
      String_kill(sGridBuffer);
    break;

    case PAGE_ACTIVE_RUNNING:
      
      // Key handling
      cKeyPressed = EventStore_get(this->pSharedEventStore, "key-pressed");
      
      // Cursor handling
      cCursorX = Page_getUserState(this, "play-i-cursor-x");
      cCursorY = Page_getUserState(this, "play-i-cursor-y");

      // Switch based on what key was last pressed
      switch(cKeyPressed) {

        // Escape character to go back
        case 27:
          Page_idle(this);
          Page_setNext(this, "menu");
        break;

        // Check the field if valid then save file after
        case '\n': case '\r':
          Gameplay_inspect(&pGame->gameField, (int) cCursorX, (int) cCursorY);

          // Display the actual grid
          sGridBuffer = String_alloc(Game_getCharWidth(pGame) * Game_getCharHeight(pGame) * 4);
          Game_displayGrid(pGame, sGridBuffer);
          Page_setComponentText(this, sFieldComponent, sGridBuffer);
          String_kill(sGridBuffer);
        break;

        default:

          // WASD movement
          if(cKeyPressed == tolower(Settings_getGameMoveUp(this->pSharedEventStore)) ||
            cKeyPressed == toupper(Settings_getGameMoveUp(this->pSharedEventStore)))
            Page_setUserState(this, "play-i-cursor-y", (cCursorY + pGame->gameField.dHeight - 1) % pGame->gameField.dHeight);

          if(cKeyPressed == tolower(Settings_getGameMoveDown(this->pSharedEventStore)) ||
            cKeyPressed == toupper(Settings_getGameMoveDown(this->pSharedEventStore)))
            Page_setUserState(this, "play-i-cursor-y", (cCursorY + 1) % pGame->gameField.dHeight);

          if(cKeyPressed == tolower(Settings_getGameMoveLeft(this->pSharedEventStore)) ||
            cKeyPressed == toupper(Settings_getGameMoveLeft(this->pSharedEventStore)))
            Page_setUserState(this, "play-i-cursor-x", (cCursorX + pGame->gameField.dWidth - 1) % pGame->gameField.dWidth);          
          
          if(cKeyPressed == tolower(Settings_getGameMoveRight(this->pSharedEventStore)) ||
            cKeyPressed == toupper(Settings_getGameMoveRight(this->pSharedEventStore)))
            Page_setUserState(this, "play-i-cursor-x", (cCursorX + 1) % pGame->gameField.dWidth);
        break;
      }

      // Update UI
      Page_setComponentPos(this, sFieldCursorComponent, 
        cCursorX * GAME_CELL_WIDTH, 
        cCursorY * GAME_CELL_HEIGHT);

      // For each cell, check if it's been inspected, and if so, change color
      for(x = 0; x < pGame->gameField.dWidth; x++) {
        for(y = 0; y < pGame->gameField.dHeight; y++) {

          // Check if it's been inspected
          if(Grid_getBit(pGame->gameField.pInspectGrid, x, y)) {

            // Unique key for each component
            sprintf(sInspectKey, "inspector-%d-%d", x, y);

            // Create the component
            Page_addComponentContext(this, sInspectKey, sFieldContainerComponent, 
              x * GAME_CELL_WIDTH - Game_getCharWidth(pGame) / 2, 
              y * GAME_CELL_HEIGHT - Game_getCharHeight(pGame) / 2 - 1, 
              GAME_CELL_WIDTH + 1, 
              GAME_CELL_HEIGHT + 1, 
              "primary-darken-0.25", "");
          }
        }
      }

    break;

    default:

    break;
  }
}

#endif