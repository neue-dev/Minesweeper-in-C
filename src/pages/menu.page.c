/**
 * @ Author: MMMM
 * @ Create Time: 2024-02-25 15:06:24
 * @ Modified time: 2024-03-10 09:10:13
 * @ Description:
 * 
 * This file defines the page handler for the menu.
 */

#ifndef PAGE_MENU_
#define PAGE_MENU_

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
void PageHandler_menu(p_obj pArgs_Page) {

  Page *this = (Page *) pArgs_Page;
  int dWidth, dHeight, i;

  // Component names
  char *sMenuComponent = "menu-fixed";
  char *sTitleComponent = "title-row-acenter.x";
  char *sSelectorComponent = "selector-acenter.x";
  char *sHeaderContainer = "header-fixed-acenter.x-acenter.y";
  char *sIndicatorContainerComponent = "indicator-row-acenter.x-acenter.y";
  char *sPromptComponent = "prompt-acenter.x-acenter.y";

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

      // Get the dimensions 
      dWidth = IO_getWidth();
      dHeight = IO_getHeight();

      // Create the assets we need
      for(i= 0; i < dMenuSelectorLength; i++)
        AssetManager_createTextAsset(this->pSharedAssetManager, sMenuSelectors[i][0], sMenuSelectorFont);

      // Add the primary components to the tree
      Page_addComponentContext(this, sMenuComponent, "root", 0, 0, dWidth, dHeight, FG_1, BG_1);
      Page_addComponentContainer(this, sTitleComponent, sMenuComponent, dWidth / 2, -4);
      Page_addComponentContainer(this, sHeaderContainer, sMenuComponent, dWidth / 2, dHeight / 2 + 5);
      Page_addComponentContainer(this, sIndicatorContainerComponent, sMenuComponent, dWidth / 2 - 3, 100);
      Page_addComponentAsset(this, sSelectorComponent, sIndicatorContainerComponent, 0, 0, FG_ACC_Y, FG_ACC_Y, "selector");
      Page_addComponentText(this, sPromptComponent, sMenuComponent, dWidth / 2, 100, 0x888888, -1, "arrow keys or WASD, enter to select");

      // Add each of the letters to the tree
      for(i = 0; i < dTitleLength; i++) {

        // Generate the keys first
        String_keyAndId(sComponentKey, "title", i);
        String_keyAndChar(sAssetKey, sTitleFontName, sTitle[i]);

        // Add the letters and make them want to go to their targets
        Page_addComponentAsset(this, sComponentKey, sTitleComponent, 0, pow(i + 1, 3) * -10 + 128, -1, -1, sAssetKey);  
        Page_setComponentTargetPosition(this, sComponentKey, PAGE_NULL_INT, 0, 0.69);
      }
      Page_addComponentAsset(this, "logo-acenter.x-acenter.y", sMenuComponent, dWidth / 2, dHeight * 2, 0xbbbbbb, -1, "logo");  
      Page_setComponentTargetPosition(this, "logo-acenter.x-acenter.y", PAGE_NULL_INT, dHeight / 2 - 1, 0.7);

      // Add the indicators and headers
      for(i = 0; i < dMenuSelectorLength; i++) {

        // Create the keys first
        String_keyAndId(sIndicatorKey, "indicator", i);
        String_keyAndStr(sAssetKey, sMenuSelectorFont, sMenuSelectors[i][0]);

        // Add the component
        Page_addComponentAsset(this, sMenuSelectors[i][1], sHeaderContainer, 0, -100, -1, -1, sAssetKey);
        Page_addComponentAsset(this, sIndicatorKey, sIndicatorContainerComponent, 3, 0, FG_1, -1, "indicator");
      }

      // Shift it down a bit
      Page_setComponentTargetPosition(this, sTitleComponent, PAGE_NULL_INT, 5, 0.7);
      Page_setComponentTargetPosition(this, sIndicatorContainerComponent, PAGE_NULL_INT, dHeight / 2 + 9, 0.5);
      Page_setComponentTargetPosition(this, sPromptComponent, PAGE_NULL_INT, dHeight / 2 + 12, 0.5);
      Page_resetComponentInitialPosition(this, sSelectorComponent, 6, PAGE_NULL_INT);

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
            Page_resetComponentInitialPosition(this, sMenuSelectors[i][1], PAGE_NULL_INT, -100);

            // Put the component in view
            if(i == cMenuSelector) {
              Page_resetComponentInitialPosition(this, sMenuSelectors[i][1], PAGE_NULL_INT, 0);
              Page_resetComponentInitialPosition(this, sSelectorComponent, i * 4 + 6, PAGE_NULL_INT);
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