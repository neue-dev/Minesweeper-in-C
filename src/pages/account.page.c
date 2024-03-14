/**
 * @ Author: MMMM
 * @ Create Time: 2024-02-25 15:06:24
 * @ Modified time: 2024-03-14 11:30:45
 * @ Description:
 * 
 * This file defines the page handler for the help page.
 */

#ifndef PAGE_ACCOUNT_
#define PAGE_ACCOUNT_

#include "../utils/utils.asset.h"
#include "../utils/utils.page.h"
#include "../utils/utils.component.h"

/**
 * Configures the main menu.
 * 
 * @param   { p_obj }   pArgs_Page  The page instance we need to configure.
*/
void PageHandler_account(p_obj pArgs_Page) {

  Page *this = (Page *) pArgs_Page;
  int dWidth, dHeight, dMargin, i;

  // Components
  char *sAccountComponent = "account.fixed";
  char *sAccountContainerComponent = "account-container.col";
  char *sTitleComponent = "title.aleft-x.abottom-y";
  char *sDividerComponent = "divider.aleft-x.abottom-y";
  char *sBodyTextComponent = "body-text.aleft-x.atop-y";
  char *sPromptTextComponent = "prompt-text.aright-x.atop-y";

  // Page title and asset
  char *sPageTitleFont = "body-font";
  char *sPageTitle = "account info";
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
      Page_addComponentContext(this, sAccountComponent, "root", 0, 0, dWidth, dHeight, "secondary", "primary");
      Page_addComponentContainer(this, sAccountContainerComponent, sAccountComponent, dMargin, 0);
      Page_addComponentAsset(this, sTitleComponent, sAccountContainerComponent, -100, 6, "", "", sPageTitleKey);
      Page_addComponentText(this, sDividerComponent, sAccountContainerComponent, 0, 0, "accent", "", String_repeat("▄", dWidth - dMargin * 2));
      Page_addComponentText(this, sPromptTextComponent, sAccountComponent, 999, dHeight - dMargin / 2, 
        "secondary-lighten-0.5", "", "[backspace] or [esc] to go back");

      // Start animations
      Page_resetComponentInitialPosition(this, sDividerComponent, -100, PAGE_NULL_INT);

      Page_setComponentTargetPosition(this, sTitleComponent, -1, PAGE_NULL_INT, 0.7);
      Page_setComponentTargetPosition(this, sPromptTextComponent, dWidth - dMargin, PAGE_NULL_INT, 0.0);
      Page_setComponentTargetPosition(this, sDividerComponent, 0, PAGE_NULL_INT, 0.0);
      
    break;

    case PAGE_ACTIVE_RUNNING:

      // Switch based on what key was last pressed
      switch(EventStore_get(this->pSharedEventStore, "key-pressed")) {

        // Exit the page
        case 8: case 27:
          this->dStage = 999;
        break;

        default:

        break;
      }
      
      // Animations
      switch(this->dStage) {
        
        case 0:     // The header
          if(Page_getComponentDist(this, sTitleComponent, 0) < MATH_E_POS1)
            Page_nextStage(this);
        break;

        case 1:     // The divider
          Page_setComponentTransitionSpeed(this, sDividerComponent, 0.8);
          
          if(Page_getComponentDist(this, sDividerComponent, 0) < MATH_E_POS1)
            Page_nextStage(this);
        break;

        case 2:     // The settings text 
          Page_nextStage(this);
        break;

        case 3:     // The exit prompt
          Page_setComponentTransitionSpeed(this, sPromptTextComponent, 0.8);
        break;

        default:    // Exit the page
          Page_idle(this);
          Page_setNext(this, "menu");
        break;
      }

    break;

    default:
      // ! exit the page
    break;
  }
}

#endif