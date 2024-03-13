/**
 * @ Author: MMMM
 * @ Create Time: 2024-02-25 15:06:24
 * @ Modified time: 2024-03-14 00:30:40
 * @ Description:
 * 
 * This file defines the page handler for the menu.
 */

#ifndef PAGE_MENU_
#define PAGE_MENU_

#include "../utils/utils.asset.h"
#include "../utils/utils.page.h"
#include "../utils/utils.component.h"

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
  char *sSelectionComponent = "selection-col";
  char *sIconContainerComponent = "icon-fixed-acenter.x-acenter.y";
  char *sSelectorComponent = "selector-acenter.x";
  char *sCategoryTitleContainer = "cat.title-fixed-acenter.x-acenter.y";
  char *sIndicatorContainerComponent = "indicator-row-acenter.x-acenter.y";
  char *sPromptComponent = "prompt-acenter.x-acenter.y";

  // Variables for the title
  char *sTitle = "MINEZ";
  char *sTitleFontName = "header-font";
  int dTitleLength = strlen(sTitle);

  // Refers to assets and components
  char sIndicatorKey[STRING_KEY_MAX_LENGTH];
  char sComponentKey[STRING_KEY_MAX_LENGTH];
  char sIconKey[STRING_KEY_MAX_LENGTH];
  char sIconAssetKey[STRING_KEY_MAX_LENGTH];
  char sAssetKey[STRING_KEY_MAX_LENGTH];

  // User states and the selector assets
  char cMenuSelector;
  int dMenuSelectorLength = 7;
  char *sMenuSelectorFont = "body-font";
  char *sMenuSelectors[7][2] = {
    { "play", "play-acenter.x"  },
    { "custom", "custom-acenter.x" },
    { "account", "account-acenter.x" },
    { "settings", "settings-acenter.x" },
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
      Page_addComponentContext(this, sMenuComponent, "root", 0, 0, dWidth, dHeight, "secondary", "primary");
      Page_addComponentContainer(this, sTitleComponent, sMenuComponent, dWidth / 2, -4);
      
      // Selection area
      Page_addComponentContainer(this, sSelectionComponent, sMenuComponent, 0, 0);
      Page_addComponentContainer(this, sIconContainerComponent, sSelectionComponent, dWidth / 2, dHeight / 2 - 4);
      Page_addComponentContainer(this, sCategoryTitleContainer, sSelectionComponent, dWidth / 2, 8);
      Page_addComponentContainer(this, sIndicatorContainerComponent, sSelectionComponent, dWidth / 2 - 3, 0);
      Page_addComponentText(this, sPromptComponent, sSelectionComponent, dWidth / 2, 0, 
        "secondary-lighten-0.5", "", "[wasd] to browse, [enter] to select");
      Page_addComponentAsset(this, sSelectorComponent, sIndicatorContainerComponent, 0, 0, "accent", "accent", "selector");

      // Add each of the letters to the tree
      for(i = 0; i < dTitleLength; i++) {

        // Generate the keys first
        String_keyAndId(sComponentKey, "title", i);
        String_keyAndChar(sAssetKey, sTitleFontName, sTitle[i]);

        // Add the letters and make them want to go to their targets
        Page_addComponentAsset(this, sComponentKey, sTitleComponent, 0, pow(i + 1, 3) * -10 + 128, "", "", sAssetKey);  
        Page_setComponentTargetPosition(this, sComponentKey, PAGE_NULL_INT, 0, 0.69);
      }

      // Add the indicators and headers
      for(i = 0; i < dMenuSelectorLength; i++) {

        // Create the keys first
        String_keyAndId(sIndicatorKey, "indicator", i);
        String_keyAndStr(sAssetKey, sMenuSelectorFont, sMenuSelectors[i][0]);
        String_keyAndStr(sIconKey, "menu", sMenuSelectors[i][1]);
        String_keyAndStr(sIconAssetKey, "menu", sMenuSelectors[i][0]);

        // Add the component
        Page_addComponentAsset(this, sIndicatorKey, sIndicatorContainerComponent, 3, 0, "secondary", "", "indicator");
        Page_addComponentAsset(this, sMenuSelectors[i][1], sCategoryTitleContainer, 0, -512, "secondary-lighten-0.2", "", sAssetKey);
        Page_addComponentAsset(this, sIconKey, sIconContainerComponent, 0, 0, "accent", "", sIconAssetKey);

        // Make the first component go to the center
        if(i) Page_resetComponentInitialPosition(this, sIconKey, 512, PAGE_NULL_INT);
      }

      // Set the initial location first
      Page_resetComponentInitialPosition(this, sSelectionComponent, PAGE_NULL_INT, 100);
      Page_resetComponentInitialPosition(this, sSelectorComponent, 6, PAGE_NULL_INT);

      // Shift it down a bit
      Page_setComponentTargetPosition(this, sTitleComponent, PAGE_NULL_INT, 5, 0.7);
      Page_setComponentTargetPosition(this, sSelectionComponent, PAGE_NULL_INT, 0, 0.9);
      Page_setComponentTargetPosition(this, sIndicatorContainerComponent, PAGE_NULL_INT, 5, 0.9);
      Page_setComponentTargetPosition(this, sPromptComponent, PAGE_NULL_INT, 7, 0.9);

      // Define initial user states
      Page_setUserState(this, "menu-selector", 0);
      Page_setUserState(this, "menu-selector-length", dMenuSelectorLength);
      
    break;

    case PAGE_ACTIVE_RUNNING:

      // Key handling
      cMenuSelector = Page_getUserState(this, "menu-selector");
      dMenuSelectorLength = Page_getUserState(this, "menu-selector-length");

      // Switch based on what key was last pressed
      if(this->dStage < 2) {
        switch(EventStore_get(this->pSharedEventStore, "key-pressed")) {

          // Increment menu selector
          case 'D': case 'd': case 39:
          case 'S': case 's': case 40:
            Page_setUserState(this, "menu-selector", cMenuSelector - dMenuSelectorLength + 1 ? cMenuSelector + 1 : cMenuSelector);
          break;

          // Decrement menu selector
          case 'A': case 'a': case 37:
          case 'W': case 'w': case 38:
            Page_setUserState(this, "menu-selector", cMenuSelector ? cMenuSelector - 1 : cMenuSelector);
          break;

          case '\n': case '\r':
            this->dStage++;
          break;

          default:

          break;
        }
      }
      
      // Animations
      switch(this->dStage) {
        
        case 0:   // After the title forms, go to next stage
          if(Page_getComponentDist(this, sTitleComponent, 1) < MATH_E_NEG1)
            Page_nextStage(this);
        break;

        case 1:   // Display the selector
          for(i = 0; i < dMenuSelectorLength; i++) {

            // Create the key
            String_keyAndStr(sIconKey, "menu", sMenuSelectors[i][1]);
            
            // Put the component out of view
            Page_resetComponentInitialPosition(this, sMenuSelectors[i][1], PAGE_NULL_INT, -512);

            // Hide the components to the left
            if(i < cMenuSelector) {
              Page_setComponentTargetPosition(this, sIconKey, -512, PAGE_NULL_INT, -0.9995);

            // Put the component in view
            } else if(i == cMenuSelector) {
              Page_setComponentTargetPosition(this, sIconKey, 0, PAGE_NULL_INT, 0.83);
              Page_resetComponentInitialPosition(this, sMenuSelectors[i][1], PAGE_NULL_INT, 1);
              Page_resetComponentInitialPosition(this, sSelectorComponent, i * 4 + 6, PAGE_NULL_INT);
            
            // Hide the components to the right
            } else {
              Page_setComponentTargetPosition(this, sIconKey, 512, PAGE_NULL_INT, -0.9995);
            }
          }
        break;

        case 2:   // User has chosen a page
          Page_setComponentTargetPosition(this, sTitleComponent, PAGE_NULL_INT, -100, -0.984);
          Page_setComponentTargetPosition(this, sSelectionComponent, PAGE_NULL_INT, 100, -0.984);

          if(Page_getComponentDist(this, sSelectionComponent, 1) < MATH_E_NEG1)
            this->dStage++;
        break;

        default:  // The outro transition has ended
          Page_idle(this);
          Page_setNext(this, sMenuSelectors[cMenuSelector][0]);
        break;
      }

    break;

    default:
      // ! exit the page
    break;
  }
}

#endif