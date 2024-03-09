/**
 * @ Author: MMMM
 * @ Create Time: 2024-02-25 15:06:24
 * @ Modified time: 2024-03-09 21:56:52
 * @ Description:
 * 
 * This file defines page configurers so we can define the different pages of our application.
 * Note that these functions are passed as callbacks to the pageManager.
 * Also, a page may have more than one runner.
 */

#ifndef PAGES_
#define PAGES_

#include "./utils/utils.asset.h"
#include "./utils/utils.page.h"
#include "./utils/utils.component.h"

//! remove these later and store colors in a theme class
#define BG_1 0xfef9ff
#define FG_1 0x111317
#define FG_ACC_B 0x4282b3
#define FG_ACC_Y 0xf18f01
#define FG_ACC_R 0xf33016

/**
 * //
 * ////
 * //////    Page configurers
 * ////////
 * ////////// 
*/

/**
 * Configures the title page.
 * 
 * @param   { p_obj }   pArgs_Page  The instance page we need to configure.
*/
void PageHandler_intro(p_obj pArgs_Page) {
  
  Page *this = (Page *) pArgs_Page;
  int dWidth = IO_getWidth();
  int dHeight = IO_getHeight();

  // Component names
  char *sIntroComponent = "intro-fixed";
  char *sOuterBoxComponent = "box.outer-row.col-center.y-center.x-acenter.x-acenter.y";
  char *sInnerBoxComponent = "box.inner-row.col-center.y-center.x-acenter.x-acenter.y";
  char *sLogoComponent = "logo-acenter.x-acenter.y";

  // Do stuff based on page status
  switch(this->ePageStatus) {

    case PAGE_ACTIVE_INIT:

      // Create component tree
      Page_addComponent(this, sIntroComponent, "root", 0, 0, dWidth, dHeight, 0, NULL, 0x000001, 0x000001);
      Page_addComponent(this, sOuterBoxComponent, sIntroComponent, dWidth / 2, dHeight / 2, 160, 80, 0, NULL, FG_ACC_Y, FG_ACC_Y);
      Page_addComponent(this, sInnerBoxComponent, sIntroComponent, dWidth / 2, dHeight / 2, 156, 78, 0, NULL, FG_1, BG_1);
      Page_addComponentAsset(this, sLogoComponent, "root", dWidth / 2, 100, -1, -1, "logo");

      // Set initials
      Page_resetComponentInitialSize(this, sOuterBoxComponent, 0, 0);
      Page_resetComponentInitialSize(this, sInnerBoxComponent, 0, 0);
      
    break;

    case PAGE_ACTIVE_RUNNING:

      // Animations
      switch(this->dStage) {
        
        case 0:   // Currently empty screen  
          if(this->dT > 16) 
            Page_nextStage(this);
        break;

        case 1:   // Make the box enlarge
          Page_setComponentTargetSize(this, sOuterBoxComponent, 30, 15, 0.5);
          Page_setComponentTargetSize(this, sInnerBoxComponent, 26, 13, 0.5);

          if(Page_getComponentDist(this, sInnerBoxComponent, 2) < MATH_E_NEG1)
            Page_nextStage(this);
        break;

        case 2:   // Make the logo fly to the center
          Page_setComponentTargetPosition(this, sLogoComponent, PAGE_NULL_INT, dHeight / 2, 0.84);

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

/**
 * Configures the main menu.
 * 
 * @param   { p_obj }   pArgs_Page  The page instance we need to configure.
*/
void PageHandler_menu(p_obj pArgs_Page) {

  Page *this = (Page *) pArgs_Page;
  int dWidth = IO_getWidth();
  int dHeight = IO_getHeight();
  int i;

  // Component names
  char *sMenuComponent = "menu-fixed";
  char *sTitleComponent = "title-row-acenter.x";
  char *sSelectorComponent = "selector-acenter.x";
  char *sIndicatorContainerComponent = "indicator-row-acenter.x-acenter.y";

  // Variables for the title
  char *sTitle = "MINEZ";
  char *sTitleFontName = "header-font";
  int dTitleLength = strlen(sTitle);

  // Refers to a single asset
  char sIndicatorKey[STRING_KEY_MAX_LENGTH];
  char sComponentKey[STRING_KEY_MAX_LENGTH];
  char sAssetKey[STRING_KEY_MAX_LENGTH];

  // User states and the selector assets
  char cMenuSelector;
  int dMenuSelectorLength = 5;
  char *sMenuSelectorFont = "body-font";
  char *sMenuSelectors[5][2] = {
    { "play", "play-acenter.x" },
    { "custom", "custom-acenter.x" },
    { "help", "help-acenter.x" },
    { "about", "about-acenter.x" },
    { "logout", "logout-acenter.x" },
  };

  // Do stuff based on page status
  switch(this->ePageStatus) {

    case PAGE_ACTIVE_INIT:

      // Create the assets we need
      for(i= 0; i < dMenuSelectorLength; i++)
        AssetManager_createTextAsset(this->pSharedAssetManager, sMenuSelectors[i][0], sMenuSelectorFont);

      // Add the primary components to the tree
      Page_addComponent(this, sMenuComponent, "root", 0, 0, dWidth, dHeight, 0, NULL, FG_1, BG_1);
      Page_addComponent(this, sTitleComponent, sMenuComponent, dWidth / 2, -4, 0, 0, 0, NULL, -1, -1);
      Page_addComponent(this, sIndicatorContainerComponent, sMenuComponent, dWidth / 2 - 1, dHeight / 2 + 5, 0, 0, 0, NULL, -1, -1);
      Page_addComponentAsset(this, sSelectorComponent, sMenuComponent, dWidth / 2, dHeight / 2 + 5, FG_ACC_Y, FG_ACC_Y, "selector");

      // Add each of the letters to the tree
      for(i = 0; i < dTitleLength; i++) {

        // Generate the keys first
        String_keyAndId(sComponentKey, "title", i);
        String_keyAndChar(sAssetKey, sTitleFontName, sTitle[i]);

        // Add the letters and make them want to go to their targets
        Page_addComponentAsset(this, sComponentKey, sTitleComponent, 0, i * i * i * -10 + 128, -1, -1, sAssetKey);  
        Page_setComponentTargetPosition(this, sComponentKey, PAGE_NULL_INT, 0, 0.69);
      }

      // Shift it down a bit
      Page_setComponentTargetPosition(this, sTitleComponent, PAGE_NULL_INT, 4, 0.69);

      // Add the selectors
      for(i = 0; i < dMenuSelectorLength; i++) {

        // Create the keys first
        String_keyAndId(sIndicatorKey, "indicator", i);
        String_keyAndStr(sAssetKey, sMenuSelectorFont, sMenuSelectors[i][0]);

        // Add the component
        Page_addComponentAsset(this, sMenuSelectors[i][1], sMenuComponent, dWidth / 2, -8, -1, -1, sAssetKey);
        Page_addComponentAsset(this, sIndicatorKey, sIndicatorContainerComponent, 3, 0, FG_1, -1, "indicator");
      }

      // Define initial user states
      Page_setUserState(this, "menu-selector", 0);
      Page_setUserState(this, "menu-selector-length", dMenuSelectorLength);
      
    break;

    case PAGE_ACTIVE_RUNNING:

      // Key handling
      cMenuSelector = Page_getUserState(this, "menu-selector");
      dMenuSelectorLength = Page_getUserState(this, "menu-selector-length");

      // Switch based on what key was last pressed
      switch(EventStore_get(this->pSharedEventStore, "key-pressed")) {

        // Increment menu selector
        case 'D': case 'd': case 39:
        case 'S': case 's': case 40:
          Page_setUserState(this, "menu-selector", (cMenuSelector + 1) % dMenuSelectorLength);
        break;

        // Decrement menu selector
        case 'A': case 'a': case 37:
        case 'W': case 'w': case 38:
          Page_setUserState(this, "menu-selector", (cMenuSelector + dMenuSelectorLength - 1) % dMenuSelectorLength);
        break;

        default:

        break;
      }
      
      // Animations
      switch(this->dStage) {
        
        case 0:   // After the title forms, go to next stage
          if(Page_getComponentDist(this, sTitleComponent, 1) < MATH_E_NEG1)
            Page_nextStage(this);
        break;

        case 1:   // Display the selector
          for(i = 0; i < dMenuSelectorLength; i++) {
            
            // Put the component out of view
            Page_resetComponentInitialPosition(this, sMenuSelectors[i][1], PAGE_NULL_INT, -8);

            // Put the component in view
            if(i == cMenuSelector) {
              Page_resetComponentInitialPosition(this, sMenuSelectors[i][1], PAGE_NULL_INT, dHeight / 2);
              Page_resetComponentInitialPosition(this, sSelectorComponent, dWidth / 2 + i * 4 - 8, PAGE_NULL_INT);
            }
          }
        break;
      }

    break;

    default:
      // ! exit the page
    break;
  }
}

#endif