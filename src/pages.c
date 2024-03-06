/**
 * @ Author: MMMM
 * @ Create Time: 2024-02-25 15:06:24
 * @ Modified time: 2024-03-06 12:28:08
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
  char *sTitle = "minesweeper";
  int dTitleLength = strlen(sTitle);
  int dCumulativeLength, dTotalLength;
  
  // Used for assets and stuff
  int dAssetHeight;
  char *sAssetKey;
  char **aAsset;
  char *sComponentKey;

  // Do stuff based on page status
  switch(this->ePageStatus) {

    case PAGE_ACTIVE_INIT:

      // A container for the title
      Page_addComponent(this, "title", "root",
        0, 0, 0, 0, 0, NULL, -1, -1);

      // Since this isn't getting assigned, we only need this once
      sAssetKey = String_alloc(256);

      // Get the total length
      for(i = 0, dTotalLength = 0; i < 11; i++) {
        
        // Get the key
        snprintf(sAssetKey, 256, "main-font-%c", sTitle[i]);

        // Compute the total length
        dTotalLength += AssetManager_getAssetWidth(this->pSharedAssetManager, sAssetKey);
      }

      // Add each of the letters to the tree
      for(i = 0, dCumulativeLength = 0; i < dTitleLength; i++) {

        // Generate the key first
        sComponentKey = String_alloc(256);
        snprintf(sComponentKey, 256, "title-%d", i);
        snprintf(sAssetKey, 256, "main-font-%c", sTitle[i]);

        // Get the asset and its height
        dAssetHeight = AssetManager_getAssetHeight(this->pSharedAssetManager, sAssetKey);
        aAsset = AssetManager_getAssetText(this->pSharedAssetManager, sAssetKey);

        // Add the component
        Page_addComponent(this, sComponentKey, "title",
          dCumulativeLength + dWidth / 2 - dTotalLength / 2, 5, 0, 0, 
          dAssetHeight,
          aAsset,
          -1, -1);  

        // Increment the cumulative length
        dCumulativeLength += String_charCount(aAsset[0]);
      }

      String_kill(sAssetKey);
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