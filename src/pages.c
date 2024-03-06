/**
 * @ Author: MMMM
 * @ Create Time: 2024-02-25 15:06:24
 * @ Modified time: 2024-03-06 13:45:17
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

  int i;
  char *sTitle = "mmmmones";
  int dTitleLength = strlen(sTitle);
  int dTotalLength;
  
  char sComponentKey[STRING_KEY_MAX_LENGTH];
  char sAssetKey[STRING_KEY_MAX_LENGTH];

  // Do stuff based on page status
  switch(this->ePageStatus) {

    case PAGE_ACTIVE_INIT:

      // A container for the title
      Page_addComponent(this, "title", "root",
        0, 0, 0, 0, 0, NULL, -1, -1);

      // Get the total length
      for(i = 0, dTotalLength = 0; i < dTitleLength; i++) {
        
        // Get the key
        String_keyAndChar(sAssetKey, "main-font", sTitle[i]);

        // Compute the total length
        dTotalLength += AssetManager_getAssetWidth(this->pSharedAssetManager, sAssetKey);
      }

      // Add each of the letters to the tree
      for(i = 0; i < dTitleLength; i++) {

        // Generate the keys first
        String_keyAndId(sComponentKey, "title", i);
        String_keyAndChar(sAssetKey, "main-font", sTitle[i]);

        // Add the component
        Page_addComponent(this, sComponentKey, "title",
          dWidth / 2 - dTotalLength / 2, 5, 
          AssetManager_getAssetWidth(this->pSharedAssetManager, sAssetKey), 
          AssetManager_getAssetHeight(this->pSharedAssetManager, sAssetKey),
          AssetManager_getAssetHeight(this->pSharedAssetManager, sAssetKey),
          AssetManager_getAssetText(this->pSharedAssetManager, sAssetKey),
          -1, -1);  
      }
    break;

    case PAGE_ACTIVE_RUNNING:
      
    break;

    default:
      // ! exit the page
    break;
  }
}

/**
 * Configures the main menu.
 * 
 * @param   { p_obj }   pArgs_Page  The page instance we need to configure.
*/
void PageHandler_menu(p_obj pArgs_Page) {
  Page *this = (Page *) pArgs_Page;

}

#endif