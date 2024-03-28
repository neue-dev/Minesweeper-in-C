/**
 * @ Author: MMMM
 * @ Create Time: 2024-02-25 15:06:24
 * @ Modified time: 2024-03-28 12:04:10
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
  
  // Do stuff based on page status
  switch(this->ePageStatus) {

    case PAGE_ACTIVE_INIT:

      // Get the dimensions 
      dWidth = IO_getWidth();
      dHeight = IO_getHeight();
      dMargin = 44;

      // Create component tree
      Page_addComponentContext(this, sPlayIComponent, "root", 0, 0, dWidth, dHeight, "seconadary", "primary");
      Page_addComponentText(this, "test.acenter-x.acenter-y", sPlayIComponent, dWidth / 2, dHeight / 2, "secondary", "primary", "test");
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
      cCursorX = Page_getUserState(this, "play-i-cursor-x");
      cCursorY = Page_getUserState(this, "play-i-cursor-y");

      // Switch based on what key was last pressed
      switch(EventStore_get(this->pSharedEventStore, "key-pressed")) {

        // Escape character to go back
        case 27:
          Page_idle(this);
          Page_setNext(this, "menu");
        break;

        // WASD movement
        case 'S': case 's': Page_setUserState(this, "play-i-cursor-y", (cCursorY + 1) % pGame->gameField.dHeight); break;
        case 'W': case 'w': Page_setUserState(this, "play-i-cursor-y", (cCursorY + pGame->gameField.dHeight - 1) % pGame->gameField.dHeight); break;
        case 'D': case 'd': Page_setUserState(this, "play-i-cursor-x", (cCursorX + 1) % pGame->gameField.dWidth); break;
        case 'A': case 'a': Page_setUserState(this, "play-i-cursor-x", (cCursorX + pGame->gameField.dWidth - 1) % pGame->gameField.dWidth); break;

        // Check the field if valid then save file after
        case '\n': case '\r':

        break;

        default:

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