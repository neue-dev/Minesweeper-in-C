/**
 * @ Author: MMMM
 * @ Create Time: 2024-02-25 15:06:24
 * @ Modified time: 2024-03-28 08:47:47
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
  char *sMenuComponent = "menu.fixed";
  char *sTitleComponent = "title.row.aleft-x";
  char *sLogoComponent = "logo.acenter-x.atop-y";
  char *sSelectionComponent = "selection.col";
  char *sSelectorComponent = "selector.aleft-x";
  char *sCategoryTitleContainer = "cat-title.col.aleft-x.atop-y";
  char *sIndicatorContainerComponent = "indicator.col.aleft-x.atop-y";
  char *sPromptComponent = "prompt.aleft-x.atop-y";

  // Variables for the title
  char *sTitle = "minez";
  char *sTitleFont = "header-font";

  // Refers to assets and components
  char sLogoKey[STRING_KEY_MAX_LENGTH] = "logo";
  char sTitleKey[STRING_KEY_MAX_LENGTH];
  char sIndicatorKey[STRING_KEY_MAX_LENGTH];
  char sAssetKey[STRING_KEY_MAX_LENGTH];

  // User states and the selector assets
  char cMenuSelector;
  int dMenuSelectorLength = 6;
  char *sMenuSelectorFont = "body-font";
  char *sMenuSelectors[6][2] = {
    { "play", "play.aleft-x"  },
    { "editor", "editor.aleft-x" },
    { "account", "account.aleft-x" },
    { "settings", "settings.aleft-x" },
    { "help", "help.aleft-x" },
    { "logout", "logout.aleft-x" },
  };

  // Do stuff based on page status
  switch(this->ePageStatus) {

    case PAGE_ACTIVE_INIT:

      // Get the dimensions 
      dWidth = IO_getWidth();
      dHeight = IO_getHeight();

      // Create the title
      String_keyAndStr(sTitleKey, sTitleFont, sTitle);
      AssetManager_createTextAsset(this->pSharedAssetManager, sTitle, sTitleFont);

      // Create the assets we need
      for(i= 0; i < dMenuSelectorLength; i++)
        AssetManager_createTextAsset(this->pSharedAssetManager, sMenuSelectors[i][0], sMenuSelectorFont);

      // Add the primary components to the tree
      Page_addComponentContext(this, sMenuComponent, "root", 0, 0, dWidth, dHeight, "secondary", "primary");
      Page_addComponentAsset(this, sTitleComponent, sMenuComponent, 14, 6, "", "", sTitleKey);
      Page_addComponentAsset(this, sLogoComponent, sMenuComponent, 75, 8, "", "", sLogoKey);

      // Selection area
      Page_addComponentContainer(this, sSelectionComponent, sMenuComponent, 92, 4);
      Page_addComponentContainer(this, sCategoryTitleContainer, sSelectionComponent, 2, 0);
      Page_addComponentContainer(this, sIndicatorContainerComponent, sSelectionComponent, 0, 0);
      Page_addComponentAsset(this, sSelectorComponent, sIndicatorContainerComponent, 0, -2, "accent", "accent", "selector");
      Page_addComponentText(this, sPromptComponent, sSelectionComponent, 0, 27, 
        "secondary-lighten-0.5", "", "[tab] to browse, [enter] to select");

      // Add the indicators and headers
      for(i = 0; i < dMenuSelectorLength; i++) {

        // Create the keys first
        String_keyAndId(sIndicatorKey, "indicator", i);
        String_keyAndStr(sAssetKey, sMenuSelectorFont, sMenuSelectors[i][0]);

        // Add the component
        Page_addComponentAsset(this, sIndicatorKey, sIndicatorContainerComponent, 0, 3, "secondary-lighten-0.5", "", "indicator");
        Page_addComponentAsset(this, sMenuSelectors[i][1], sCategoryTitleContainer, 1, 2, "secondary-lighten-0.75", "", sAssetKey);
      }

      // Define initial user states
      if(Page_getUserState(this, "menu-selector") == -1) Page_setUserState(this, "menu-selector", 0);
      if(Page_getUserState(this, "menu-selector-length") == -1) Page_setUserState(this, "menu-selector-length", dMenuSelectorLength);
      
    break;

    case PAGE_ACTIVE_RUNNING:

      // Key handling
      cMenuSelector = Page_getUserState(this, "menu-selector");
      dMenuSelectorLength = Page_getUserState(this, "menu-selector-length");

      // Switch based on what key was last pressed
      switch(EventStore_get(this->pSharedEventStore, "key-pressed")) {

        // Increment menu selector
        case '\t':
          Page_setUserState(this, "menu-selector", ((int) cMenuSelector + 1) % dMenuSelectorLength);
        break;

        // Move to page
        case '\n': case '\r':
          Page_idle(this);

          // Bring the user to the login page when they logout
          if(!strcmp(sMenuSelectors[(int) cMenuSelector][0], "logout"))
            Page_setNext(this, "login");
            
          // Otherwise, bring the user to the page they selected
          else
            Page_setNext(this, sMenuSelectors[(int) cMenuSelector][0]);
        break;

        default:

        break;
      }

      // Update the selector
      Page_setComponentPos(this, sSelectorComponent, -1, ((int) cMenuSelector) * 4 + 2);

      // Update the links
      for(i = 0; i < dMenuSelectorLength; i++)
        Page_setComponentColor(this, sMenuSelectors[i][1], "secondary-lighten-0.5", "");      
      Page_setComponentColor(this, sMenuSelectors[(int) cMenuSelector][1], "secondary-lighten-0.08", "");

    break;

    default:
      //  At this point, the page manager should detect that the page has become idle
      //  After that, the page manager should switch control to the page indicated by this->sNextPage
    break;
  }
}

#endif