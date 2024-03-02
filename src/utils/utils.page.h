/**
 * @ Author: MMMM
 * @ Create Time: 2024-03-02 21:58:49
 * @ Modified time: 2024-03-02 23:29:02
 * @ Description:
 * 
 * The page class bundles together a buffer, shared assets, and an animation manager. 
 * The class handles interactions between these three components and helps decouple our classes.
 */

#ifndef UTILS_PAGE_
#define UTILS_PAGE_

#include "./utils.buffer.h"
#include "./utils.asset.h"
#include "./utils.animation.h"
#include "./utils.types.h"

typedef struct Page Page;
typedef struct PageManager PageManager;

/**
 * //
 * ////
 * //////    Page class
 * ////////
 * ////////// 
*/

/**
 * The page class bundles together a buffer, a shared asset manager, and its own animation manager.
 * 
 * @class
*/
struct Page {
  char *sName;                        // An identifier for the page

  AnimationManager animationManager;  // An animation manager so we can create animations
  AssetManager *pSharedAssetManager;  // A reference to a shared asset manager so we can access all assets

  Buffer *pPageBuffer;                // This is where all our content will be displayed
};

/**
 * Allocates memory for an instance of the Page class.
 * 
 * @return	{ Page * }		A pointer to the allocated memory.
*/
Page *Page_new() {
  Page *pPage = calloc(1, sizeof(*pPage));
  return pPage;
}

/**
 * Initializes an instance of the Page class.
 * 
 * @param		{ Page * }		      this	                A pointer to the instance to initialize.
 * @param   { AssetManager * }  pSharedAssetManager   A reference to the manager that stores all our assets.
 * @return	{ Page * }					                      A pointer to the initialized instance.
*/
Page *Page_init(Page *this, AssetManager *pSharedAssetManager) {
  AnimationManager_init(&this->animationManager);
  this->pSharedAssetManager = pSharedAssetManager;

  return this;
}

/**
 * Creates an initialized instance of the Page class.
 * 
 * @param   { AssetManager * }      pSharedAssetManager   A reference to the manager that stores all our assets.
 * @return	{ Page * }					                          A pointer to the initialized instance.
*/
Page *Page_create(AssetManager *pSharedAssetManager) {
  return Page_init(Page_new(), pSharedAssetManager);
}

/**
 * Deallocates the memory of an instance of the Page class.
 * 
 * @param		{ Page * }		this	A pointer to the instance to deallocate.
*/
void Page_kill(Page *this) {
  if(this->pPageBuffer != NULL)
    Buffer_kill(this->pPageBuffer);
  
  free(this);
}

/**
 * Performs a single frame update of our page instance.
 * 
 * @param		{ Page * }		this	A pointer to the instance to update.
*/
void Page_update(Page *this) {

  // Initialize the page buffer first
  this->pPageBuffer = Buffer_create(
    IO_getWidth(), 
    IO_getHeight(),
    0x000000,
    0xffffff);

  // Update the animations
  AnimationManager_updateAll(&this->animationManager);

  // Print the buffer
  IO_resetCursor();
  Buffer_print(this->pPageBuffer);
  Buffer_kill(this->pPageBuffer);
}

/**
 * //
 * ////
 * //////    PageManager struct
 * ////////
 * ////////// 
*/

/**
 * The page manager struct just abstracts page creation and handling for us.
 * 
 * @struct
*/
struct PageManager {
  AssetManager *pSharedAssetManager;  // A reference to the shared asset manager so we only have to pass it during init

  HashMap *pPageMap;                  // Stores all the pages we have
  int dPageCount;                     // How many pages we have
                                      // Note that we don't have a PAGE_MAX_COUNT because we prolly won't overload 
                                      //    ourselves in this area.    
};

/**
 * Initializes the page manager.
 * 
 * @param   { PageManager * }   this                  The page manager object.
 * @param   { AssetManager * }  pSharedAssetManager   The asset manager shared by all pages.
*/
void PageManager_init(PageManager *this, AssetManager *pSharedAssetManager) {
  this->pPageMap = HashMap_create();
  this->dPageCount = 0;

  // So we dont need to pass it each time
  this->pSharedAssetManager = pSharedAssetManager;
}

/**
 * // ! todo
*/
void PageManager_exit(PageManager *this) {
  
}

/**
 * Creates a new page instance and stores it in the hashmap.
 * 
 * @param   { PageManager * }       this                  The page manager object.
 * @param   { char * }              sPageKey              An identifier for the page.
 * @param   { f_page_configurer }   fPageConfigurer       A function that configures a page.
*/
void PageManager_createPage(PageManager *this, char *sPageKey, f_page_configurer fPageConfigurer) {
  Page *pPage = Page_create(this->pSharedAssetManager);

  // Configure the page
  fPageConfigurer(pPage);

  // Add it to the hashmap and update our count
  HashMap_add(this->pPageMap, sPageKey, pPage);
  this->dPageCount++;
}

/**
 * Update the page we specified.
 * @param   { PageManager * }   this      The page manager object.
 * @param   { char * }          sPageKey  An identifier for the page.
*/
void PageManager_updatePage(PageManager *this, char *sPageKey) {
  Page_update((Page *) HashMap_get(this->pPageMap, sPageKey));
}

#endif
