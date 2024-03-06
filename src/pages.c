/**
 * @ Author: MMMM
 * @ Create Time: 2024-02-25 15:06:24
 * @ Modified time: 2024-03-06 11:56:32
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

  switch(this->ePageStatus) {

    case PAGE_ACTIVE_INIT:
      Page_addComponent(this, "test", "root",
        0, 0, 0, 0, 
        AssetManager_getAssetHeight(this->pSharedAssetManager, "main-font-m"),
        AssetManager_getAssetText(this->pSharedAssetManager, "main-font-m"),
        -1, -1);

      Page_addComponent(this, "test89", "root", 
        0, 20, 0, 0, 
        AssetManager_getAssetHeight(this->pSharedAssetManager, "main-font-w"),
        AssetManager_getAssetText(this->pSharedAssetManager, "main-font-w"), 
        -1, -1);

      Page_addComponent(this, "test2", "test", 
        10, 5, 10, 10, 
        AssetManager_getAssetHeight(this->pSharedAssetManager, "main-font-d"),
        AssetManager_getAssetText(this->pSharedAssetManager, "main-font-d"), 
        0x123456, 0x654321);

      Page_addComponent(this, "test2-child", "test2", 
        10, 5, 10, 10, 
        AssetManager_getAssetHeight(this->pSharedAssetManager, "main-font-c"),
        AssetManager_getAssetText(this->pSharedAssetManager, "main-font-c"),
        0xaabbcc, 0x112233);

      Page_setComponentTarget(this, "test2-child", 0, 0, -1, -1, -1, -1, 0.8);

      // this->
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