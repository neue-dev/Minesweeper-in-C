/**
 * @ Author: MMMM
 * @ Create Time: 2024-02-25 15:06:24
 * @ Modified time: 2024-03-28 16:20:47
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
  int dWidth, dHeight;

  // Component names
  char *sPlayIComponent = "play-i.fixed";
  char *sFieldContainerComponent = "field-container.fixed";
  char *sFieldComponent = "field.acenter-x.acenter-y";
  char *sFieldCursorComponent = "field-cursor.aleft-x.atop-y";

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
      
      // Create component tree
      Page_addComponentContext(this, sPlayIComponent, "root", 0, 0, dWidth, dHeight, "primary", "secondary");
      Page_addComponentContainer(this, sFieldContainerComponent, sPlayIComponent, dWidth / 2, dHeight / 2);
      Page_addComponentText(this, sFieldComponent, sFieldContainerComponent, 0, 0, "primary-darken-0.75", "", "test");
      Page_addComponentAsset(this, sFieldCursorComponent, sFieldContainerComponent, 0, 0, "accent", "", "field-cursor");

      // Define initial user states
      if(Page_getUserState(this, "play-i-cursor-x") == -1) Page_setUserState(this, "play-i-cursor-x", cCursorX);
      if(Page_getUserState(this, "play-i-cursor-y") == -1) Page_setUserState(this, "play-i-cursor-y", cCursorY);

      // Initialize the game
      // ! move this elsewhere?
      Game_init(pGame);
      Gameplay_initClassic(GAMEPLAY_DIFFICULTY_EASY, &pGame->gameField);
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

          // !fix this
          Gameplay_inspect(&pGame->gameField, (int) cCursorX, (int) cCursorY);
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

          // Display the actual grid
          Page_setComponentText(this, sFieldComponent, Game_displayGrid(pGame));
        break;
      }

      // Update UI
      Page_setComponentPos(this, sFieldCursorComponent, 
        cCursorX * GAME_CELL_WIDTH - Game_getCharWidth(pGame) / 2, 
        cCursorY * GAME_CELL_HEIGHT - Game_getCharHeight(pGame) / 2 - 1);

    break;

    default:

    break;
  }
}

#endif