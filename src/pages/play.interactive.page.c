/**
 * @ Author: MMMM
 * @ Create Time: 2024-02-25 15:06:24
 * @ Modified time: 2024-03-28 15:45:26
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

/**
 * Configures the main menu.
 * 
 * @param   { p_obj }   pArgs_Page  The page instance we need to configure.
*/
void PageHandler_playI(p_obj pArgs_Page) {

  Page *this = (Page *) pArgs_Page;
  Game *pGame = (Game *) this->pSharedObject;
  int dWidth, dHeight, dMargin;

  // Component names
  char *sPlayIComponent = "play-i.fixed";
  char *sFieldCursor = "field-cursor.fixed";

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
      dMargin = 44;

      // Create component tree
      Page_addComponentContext(this, sPlayIComponent, "root", 0, 0, dWidth, dHeight, "primary", "secondary");
      Page_addComponentText(this, "test.acenter-x.acenter-y", sPlayIComponent, dWidth / 2, dHeight / 2, "primary-darken-0.75", "", "test");
      Page_addComponentAsset(this, sFieldCursor, sPlayIComponent, 0, 0, "accent", "", "field-cursor");

      // Define initial user states
      if(Page_getUserState(this, "play-i-cursor-x") == -1) Page_setUserState(this, "play-i-cursor-x", cCursorX);
      if(Page_getUserState(this, "play-i-cursor-y") == -1) Page_setUserState(this, "play-i-cursor-y", cCursorY);

      // Initialize the game
      // ! move this elsewhere?
      Game_init(pGame);
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

          // ! remove all things having to do with "test"
          Page_setComponentText(this, "test.acenter-x.acenter-y", Field_displayGrid(&pGame->gameField));

        break;
      }

      // Update UI
      // ! DONT MAKE THE POSITIONING HARDCODED
      Page_setComponentPos(this, sFieldCursor, cCursorX * 4 + 52, cCursorY * 2 + 7);

    break;

    default:

    break;
  }
}

#endif