/**
 * @ Author: MMMM
 * @ Create Time: 2024-02-25 15:06:24
 * @ Modified time: 2024-03-25 21:54:14
 * @ Description:
 * 
 * This file defines the page handler for the help page.
 */

#ifndef PAGE_SETTINGS_
#define PAGE_SETTINGS_

#include "../utils/utils.asset.h"
#include "../utils/utils.page.h"
#include "../utils/utils.component.h"

#include "../settings.c"

/**
 * Configures the main menu.
 * 
 * @param   { p_obj }   pArgs_Page  The page instance we need to configure.
*/
void PageHandler_settings(p_obj pArgs_Page) {

  Page *this = (Page *) pArgs_Page;
  int i, dWidth, dHeight, dMargin;

  // Components
  char *sSettingsComponent = "settings.fixed";
  char *sSettingsContainerComponent = "settings-container.col";
  char *sTitleComponent = "title.aleft-x.abottom-y";
  char *sDividerComponent = "divider.aleft-x.abottom-y";
  char *sThemeSelectorComponent = "theme-selector.aleft-x.atop-y";
  char *sSelectorPromptComponent = "selector-prompt.aleft-x.atop-y";
  char *sPromptTextComponent = "prompt-text.aright-x.atop-y";

  // Page title and asset
  char *sPageTitleFont = "body-font";
  char *sPageTitle = "settings";
  char sPageTitleKey[STRING_KEY_MAX_LENGTH];

  // Where we store the keybinds, and a temp var for keybind keys
  int dKeybindCount = 0;
  char *sKeybindArray[32];
  char sKeybindKey[STRING_KEY_MAX_LENGTH];
  char sKeybindDisplay[STRING_KEY_MAX_LENGTH];

  // Setting parameters
  char cSettingsSelectorCount = 0;
  char cSettingsSelector = 0;

  // Retrieve the keybinds
  Settings_getKeybinds(&dKeybindCount, sKeybindArray);

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
      Page_addComponentText(this, sPromptTextComponent, sSettingsComponent, dWidth - dMargin, dHeight - dMargin / 2, "secondary-lighten-0.5", "", "[backspace] or [esc] to go back");
      Page_addComponentText(this, sSelectorPromptComponent, sSettingsContainerComponent, 0, 1, "", "", "[tab] to navigate; [any key] to change settings");
      
      // Append the compoennts for the keybind settings
      for(i = 0; i < dKeybindCount; i++) {
        String_keyAndStr(sKeybindKey, "keybind", sKeybindArray[i]);
        sprintf(sKeybindDisplay, "%-29s %s", sKeybindArray[i], String_renderEscChar(EventStore_get(this->pSharedEventStore, sKeybindArray[i])));
        Page_addComponentText(this, sKeybindKey, sSettingsContainerComponent, 0, 1, "", "", sKeybindDisplay);
      }

      // Set selector states; note that we have dKeybindCount + 1 because we also have a theming setting
      if(Page_getUserState(this, "settings-selector") == -1) Page_setUserState(this, "settings-selector", 0);
      if(Page_getUserState(this, "settings-selector-count") == -1) Page_setUserState(this, "settings-selector-count", dKeybindCount);
    
    break;

    case PAGE_ACTIVE_RUNNING:

      // Retrieve some information
      cSettingsSelector = Page_getUserState(this, "settings-selector");
      cSettingsSelectorCount = Page_getUserState(this, "settings-selector-count");

      // Switch based on what key was last pressed
      switch(EventStore_get(this->pSharedEventStore, "key-pressed")) {

        // Exit the page
        case 8: case 27:
          Page_idle(this);
          Page_setNext(this, "menu");
        break;

        // Select a different setting
        case '\t':
          Page_setUserState(this, "settings-selector", ((int) cSettingsSelector + 1) % ((int) cSettingsSelectorCount));

        default:
          
          // Ligten all components
          for(i = 0; i < dKeybindCount; i++) {
            String_keyAndStr(sKeybindKey, "keybind", sKeybindArray[i]);
            Page_setComponentColor(this, sKeybindKey, "secondary-lighten-0.5", "primary");
          }

          // Emphasize selected keybind
          String_keyAndStr(sKeybindKey, "keybind", sKeybindArray[(int) cSettingsSelector]);
          Page_setComponentColor(this, sKeybindKey, "secondary", "accent");

          // If a valid key is pressed
          if(EventStore_get(this->pSharedEventStore, "key-pressed") >= 32 || 
            EventStore_get(this->pSharedEventStore, "key-pressed") == 10 ||
            EventStore_get(this->pSharedEventStore, "key-pressed") == 13) {
            
            // We update the keybind
            EventStore_set(this->pSharedEventStore, sKeybindArray[(int) cSettingsSelector], 
              EventStore_get(this->pSharedEventStore, "key-pressed"));

            // We also update the UI to reflect this
            sprintf(sKeybindDisplay, "%-29s %s", sKeybindArray[(int) cSettingsSelector], 
              String_renderEscChar(EventStore_get(this->pSharedEventStore, sKeybindArray[(int) cSettingsSelector])));
            Page_setComponentText(this, sKeybindKey, sKeybindDisplay);
          }

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