/**
 * @ Author: MMMM
 * @ Create Time: 2024-02-25 15:06:24
 * @ Modified time: 2024-03-25 12:25:17
 * @ Description:
 * 
 * This file defines the page handler for the help page.
 */

#ifndef PAGE_SETTINGS_
#define PAGE_SETTINGS_

#include "../utils/utils.asset.h"
#include "../utils/utils.page.h"
#include "../utils/utils.component.h"

/**
 * Configures the main menu.
 * 
 * @param   { p_obj }   pArgs_Page  The page instance we need to configure.
*/
void PageHandler_settings(p_obj pArgs_Page) {

  Page *this = (Page *) pArgs_Page;
  int dWidth, dHeight, dMargin;

  // Components
  char *sSettingsComponent = "settings.fixed";
  char *sSettingsContainerComponent = "settings-container.col";
  char *sTitleComponent = "title.aleft-x.abottom-y";
  char *sDividerComponent = "divider.aleft-x.abottom-y";
  char *sPromptTextComponent = "prompt-text.aright-x.atop-y";

  // Page title and asset
  char *sPageTitleFont = "body-font";
  char *sPageTitle = "settings";
  char sPageTitleKey[STRING_KEY_MAX_LENGTH];

  // Do stuff based on page status
  switch(this->ePageStatus) {

    case PAGE_ACTIVE_INIT:

      // Get the dimensions 
      dWidth = IO_getWidth();
      dHeight = IO_getHeight();
      dMargin = 10;

      // Create text assets
      AssetManager_createTextAsset(this->pSharedAssetManager, sPageTitle, sPageTitleFont);

      // Create keys
      String_keyAndStr(sPageTitleKey, sPageTitleFont, sPageTitle);

      // Create component tree
      Page_addComponentContext(this, sSettingsComponent, "root", 0, 0, dWidth, dHeight, "secondary", "primary");
      Page_addComponentContainer(this, sSettingsContainerComponent, sSettingsComponent, dMargin, dMargin / 2);
      Page_addComponentAsset(this, sTitleComponent, sSettingsContainerComponent, -1, 1, "", "", sPageTitleKey);
      Page_addComponentText(this, sDividerComponent, sSettingsContainerComponent, 0, 0, "accent", "", String_repeat("▄", dWidth - dMargin * 2));
      Page_addComponentText(this, sPromptTextComponent, sSettingsComponent, dWidth - dMargin, dHeight - dMargin / 2, 
        "secondary-lighten-0.5", "", "[backspace] or [esc] to go back");
      
    break;

    case PAGE_ACTIVE_RUNNING:

      // Switch based on what key was last pressed
      switch(EventStore_get(this->pSharedEventStore, "key-pressed")) {

        // Exit the page
        case 8: case 27:
          Page_idle(this);
          Page_setNext(this, "menu");
        break;

        default:

        break;
      }

    break;

    default:
      //  At this point, the page manager should detect that the page has become idle
      //  After that, the page manager should switch control to the page indicated by this->sNextPage
    break;
  }
}

#endif