/**
 * @ Author: MMMM
 * @ Create Time: 2024-02-25 15:06:24
 * @ Modified time: 2024-03-28 10:54:22
 * @ Description:
 * 
 * This file defines the page handler for the page where the user can actually play minesweeper
 */

#ifndef PAGE_PLAY_INTERACTIVE_
#define PAGE_PLAY_INTERACTIVE_

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
  int dWidth, dHeight, dMargin;

  // Component names
  char *sPlayIComponent = "play-i.fixed";

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
      Page_addComponentText(this, "test", sPlayIComponent, 10, 5, "accent", "secondary", "test");

      // Define initial user states
      if(Page_getUserState(this, "play-i-cursor-x") == -1) Page_setUserState(this, "play-i-cursor-x", cCursorX);
      if(Page_getUserState(this, "play-i-cursor-y") == -1) Page_setUserState(this, "play-i-cursor-y", cCursorY);
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

        // Check the field if valid then save file after
        case '\n': case '\r':

        break;

        default:


        break;
      }


    break;

    default:

    break;
  }
}

#endif