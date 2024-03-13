/**
 * @ Author: MMMM
 * @ Create Time: 2024-02-25 15:06:24
 * @ Modified time: 2024-03-13 15:07:11
 * @ Description:
 * 
 * This file defines the page handler for the help page.
 */

#ifndef PAGE_CUSTOM_
#define PAGE_CUSTOM_

#include "../utils/utils.asset.h"
#include "../utils/utils.page.h"
#include "../utils/utils.component.h"

/**
 * Configures the main menu.
 * 
 * @param   { p_obj }   pArgs_Page  The page instance we need to configure.
*/
void PageHandler_custom(p_obj pArgs_Page) {

  Page *this = (Page *) pArgs_Page;
  int dWidth, dHeight, i;

  // Component names
  char *sHelpComponent = "help-fixed";

  // Do stuff based on page status
  switch(this->ePageStatus) {

    case PAGE_ACTIVE_INIT:

      // Get the dimensions 
      dWidth = IO_getWidth();
      dHeight = IO_getHeight();

      Page_addComponentContainer(this, sHelpComponent, "root", 0, 0);
      Page_addComponentAsset(this, "test", sHelpComponent, 0, 0, "", "", "menu-help");
      
    break;

    case PAGE_ACTIVE_RUNNING:
      
      // Animations
      switch(this->dStage) {
        
        case 0: 
          
        break;
      }

    break;

    default:
      // ! exit the page
    break;
  }
}

#endif