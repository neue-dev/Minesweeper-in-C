/**
 * @ Author: MMMM
 * @ Create Time: 2024-02-25 15:06:24
 * @ Modified time: 2024-03-10 08:56:08
 * @ Description:
 * 
 * This file defines the page handler for the intro.
 */

#ifndef PAGE_INTRO_
#define PAGE_INTRO_

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
 * Configures the title page.
 * 
 * @param   { p_obj }   pArgs_Page  The instance page we need to configure.
*/
void PageHandler_intro(p_obj pArgs_Page) {
  
  Page *this = (Page *) pArgs_Page;
  int dWidth, dHeight;

  // Component names
  char *sIntroComponent = "intro-fixed";
  char *sOuterBoxComponent = "box.outer-row.col-center.y-center.x-acenter.x-acenter.y";
  char *sInnerBoxComponent = "box.inner-row.col-center.y-center.x-acenter.x-acenter.y";
  char *sLogoComponent = "logo-acenter.x-acenter.y";

  // Do stuff based on page status
  switch(this->ePageStatus) {

    case PAGE_ACTIVE_INIT:

      // Get the dimensions 
      dWidth = IO_getWidth();
      dHeight = IO_getHeight();

      // Create component tree
      Page_addComponentContext(this, sIntroComponent, "root", 0, 0, dWidth, dHeight, 0x000001, 0x000001);
      Page_addComponentContext(this, sOuterBoxComponent, sIntroComponent, dWidth / 2, dHeight / 2, 160, 80, FG_ACC_Y, FG_ACC_Y);
      Page_addComponentContext(this, sInnerBoxComponent, sIntroComponent, dWidth / 2, dHeight / 2, 156, 78, FG_1, BG_1);
      Page_addComponentAsset(this, sLogoComponent, "root", dWidth / 2, 100, -1, -1, "logo");

      // Set initials
      Page_resetComponentInitialSize(this, sOuterBoxComponent, 0, 0);
      Page_resetComponentInitialSize(this, sInnerBoxComponent, 0, 0);
      Page_setComponentTargetPosition(this, sLogoComponent, PAGE_NULL_INT, dHeight / 2, 0);
      
    break;

    case PAGE_ACTIVE_RUNNING:

      // Animations
      switch(this->dStage) {
        
        case 0:   // Currently empty screen  
          if(this->dT > 16) 
            Page_nextStage(this);
        break;

        case 1:   // Make the box enlarge
          Page_setComponentTargetSize(this, sOuterBoxComponent, 25, 12, 0.5);
          Page_setComponentTargetSize(this, sInnerBoxComponent, 21, 10, 0.5);

          if(Page_getComponentDist(this, sInnerBoxComponent, 2) < MATH_E_NEG1)
            Page_nextStage(this);
        break;

        case 2:   // Make the logo fly to the center
          Page_setComponentTargetPosition(this, sLogoComponent, PAGE_NULL_INT, PAGE_NULL_INT, 0.84);

          if(Page_getComponentDist(this, sLogoComponent, 1) < MATH_E_NEG5)
            Page_nextStage(this);
        break;

        case 3:   // Make the screen white
          Page_setComponentTargetPosition(this, sLogoComponent, PAGE_NULL_INT, -16, -0.9);
          Page_setComponentTargetSize(this, sOuterBoxComponent, 160, 80, 0.45);
          Page_setComponentTargetSize(this, sInnerBoxComponent, 156, 78, 0.45);

          if(Page_getComponentDist(this, sLogoComponent, 1) < MATH_E_NEG1)
            Page_nextStage(this);
        break;
        
        default:  // Make the page idle
          Page_idle(this);
        break;
      }

    break;

    default: break;
  }
}

#endif