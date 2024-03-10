/**
 * @ Author: MMMM
 * @ Create Time: 2024-02-25 15:06:24
 * @ Modified time: 2024-03-10 11:07:39
 * @ Description:
 * 
 * This file defines the page handler for the about page.
 */

#ifndef PAGE_ABOUT_
#define PAGE_ABOUT_

#include "../utils/utils.asset.h"
#include "../utils/utils.page.h"
#include "../utils/utils.component.h"

//! remove these later and store colors in a theme class
#define BG_1 0xfef9ff
#define FG_1 0x111317
#define FG_ACC_B 0x4282b3
#define FG_ACC_Y 0xf18f01
#define FG_ACC_R 0xf33016

/**
 * Configures the main menu.
 * 
 * @param   { p_obj }   pArgs_Page  The page instance we need to configure.
*/
void PageHandler_about(p_obj pArgs_Page) {

  Page *this = (Page *) pArgs_Page;
  int dWidth, dHeight, i;

  // Do stuff based on page status
  switch(this->ePageStatus) {

    case PAGE_ACTIVE_INIT:

      // Get the dimensions 
      dWidth = IO_getWidth();
      dHeight = IO_getHeight();
      
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