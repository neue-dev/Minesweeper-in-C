/**
 * @ Author: MMMM
 * @ Create Time: 2024-03-02 21:58:49
 * @ Modified time: 2024-03-04 14:57:16
 * @ Description:
 * 
 * The page class bundles together a buffer, shared assets, shared event stores, and an runner manager. 
 * The class handles interactions between these three components and helps decouple our classes.
 */

#ifndef UTILS_PAGE_
#define UTILS_PAGE_

#include "./utils.event.h"
#include "./utils.component.h"
#include "./utils.buffer.h"
#include "./utils.asset.h"
#include "./utils.runner.h"
#include "./utils.types.h"

#define PAGE_MAX_COUNT (1 << 4)
#define PAGE_MAX_NAME_LEN (1 << 8)

typedef enum PageState PageState;

typedef struct Page Page;
typedef struct PageManager PageManager;

enum PageState {
  PAGE_ACTIVE_INIT,       // The page is currently initializing
  PAGE_ACTIVE_RUNNING,    // The page is currently being rendered and updated
  PAGE_ACTIVE_IDLE,       // The page has nothing else to update but is currently active
  PAGE_INACTIVE,          // The page is currently just residing in memory
};

/**
 * //
 * ////
 * //////    Page class
 * ////////
 * ////////// 
*/

/**
 * The page class bundles together a buffer, a shared asset manager, and its own runner manager.
 * 
 * @class
*/
struct Page {
  char *sName;                        // An identifier for the page
  char *sNextName;                    // The name of the next page to render
  PageState ePageState;               // The current state of the page

  RunnerManager runnerManager;        // An runner manager so we can handle the "backend" of each page
  AssetManager *pSharedAssetManager;  // A reference to a shared asset manager so we can access all assets
  EventStore *pSharedEventStore;      // Where we can access values modified by events

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
 * @param   { EventStore * }    pSharedEventStore     A reference to an event store instance that helps use deal with shared event states.
 * @return	{ Page * }					                      A pointer to the initialized instance.
*/
Page *Page_init(Page *this, AssetManager *pSharedAssetManager, EventStore *pSharedEventStore) {
  RunnerManager_init(&this->runnerManager);

  this->pSharedAssetManager = pSharedAssetManager;
  this->pSharedEventStore = pSharedEventStore;

  return this;
}

/**
 * Creates an initialized instance of the Page class.
 * 
 * @param   { AssetManager * }      pSharedAssetManager   A reference to the manager that stores all our assets.
 * @param   { EventStore * }        pSharedEventStore     A reference to an event store instance that helps use deal with shared event states.
 * @return	{ Page * }					                          A pointer to the initialized instance.
*/
Page *Page_create(AssetManager *pSharedAssetManager, EventStore *pSharedEventStore) {
  return Page_init(Page_new(), pSharedAssetManager, pSharedEventStore);
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

  // The page isn't active
  if(this->ePageState == PAGE_INACTIVE)
    return;

  // If the page stopped after that runner update
  if(this->ePageState == PAGE_ACTIVE_IDLE)
    return;

  // Initialize the page buffer first
  this->pPageBuffer = Buffer_create(
    IO_getWidth(), 
    IO_getHeight(),
    0x000000,
    0xffffff);

  // Update the runners
  RunnerManager_updateAll(&this->runnerManager);

  // The page is currently initializing
  if(this->ePageState == PAGE_ACTIVE_INIT) {
    this->ePageState = PAGE_ACTIVE_RUNNING;
    Buffer_kill(this->pPageBuffer);

    return;
  }

  // Reset the cursor home position
  IO_resetCursor();
  
  // Print the buffer
  Buffer_print(this->pPageBuffer);
  Buffer_kill(this->pPageBuffer);
}

/**
 * Sets the page state to PAGE_ACTIVE.
 * 
 * @param   { Page * }  this  The page we're going to activate.
*/
void Page_activate(Page *this) {
  this->ePageState = PAGE_ACTIVE_INIT;
}

/**
 * Sets the page state to PAGE_INACTIVE.
 * 
 * @param   { Page * }  this  The page we're going to deactivate.
*/
void Page_deactivate(Page *this) {
  this->ePageState = PAGE_INACTIVE;
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
 * The manager also ensures we only have one active page at a time.
 * 
 * @struct
*/
struct PageManager {
  AssetManager *pSharedAssetManager;  // A reference to the shared asset manager so we only have to pass it during init
  EventStore *pSharedEventStore;      // A reference to a shared event store so we can access things changed by events

  HashMap *pPageMap;                  // Stores all the pages we have
  int dPageCount;                     // How many pages we have

  char *sPageKeys[PAGE_MAX_COUNT];    // We need this for certain operations.
  char *sActivePage;                  // An identifier to the active page.

  // ! create a pointer to a shared state object here
  // ! that way, we can pass around key events without actually having to pass around keyevents
};

/**
 * Initializes the page manager.
 * 
 * @param   { PageManager * }   this                  The page manager object.
 * @param   { AssetManager * }  pSharedAssetManager   The asset manager shared by all pages.
 * @param   { EventStore * }    pSharedEventStore     A reference to an event store instance that helps use deal with shared event states.
*/
void PageManager_init(PageManager *this, AssetManager *pSharedAssetManager, EventStore *pSharedEventStore) {
  this->pPageMap = HashMap_create();
  this->dPageCount = 0;

  // Max page name is 256 characters
  this->sActivePage = String_alloc(PAGE_MAX_NAME_LEN);

  // So we dont need to pass it each time
  this->pSharedAssetManager = pSharedAssetManager;

  // So we dont need to pass a lot of things around
  this->pSharedEventStore = pSharedEventStore;
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
  Page *pPage = Page_create(this->pSharedAssetManager, this->pSharedEventStore);

  // Set the created page as the active page (by default)
  strcpy(this->sActivePage, sPageKey);

  // Configure the page
  fPageConfigurer(pPage);

  // Add it to the hashmap and update our count
  HashMap_add(this->pPageMap, sPageKey, pPage);

  // Store the key and increment the count
  this->sPageKeys[this->dPageCount] = sPageKey;
  this->dPageCount++;
}

/**
 * Returns the state of the active page.
 * 
 * @param   { PageManager * }   this  The page manager we want to read.
 * @return  { int }                   An enum indicating the state of the page.
*/
int PageManager_getActiveState(PageManager *this) {
  return ((Page *) HashMap_get(this->pPageMap, this->sActivePage))->ePageState;
}

/**
 * Sets the active page.
 * 
 * @param   { PageManager * }   this      The page manager we want to update.
 * @param   { char * }          sPageKey  The key of the page we want to select.
*/
void PageManager_setActive(PageManager *this, char *sPageKey) {

  // Deactivate the active page first
  Page_deactivate((Page *) HashMap_get(this->pPageMap, this->sActivePage));

  // Set the active page to the one we want
  strcpy(this->sActivePage, sPageKey);

  // Activate our page
  Page_activate((Page *) HashMap_get(this->pPageMap, this->sActivePage));
}

/**
 * Updates the active page.
 * 
 * @param   { PageManager * }   this      The page manager object.
*/
void PageManager_update(PageManager *this) {
  Page_update((Page *) HashMap_get(this->pPageMap, this->sActivePage));
}

// !
// /**
//  * Returns the state of a page to us.
//  * 
//  * 
// */
// int PageManager_getPageState(PageManager *this, char *sPageKey) {
//   return ((Page * ) HashMap_get(this->pPageMap, sPageKey))->eState;
// }

#endif
