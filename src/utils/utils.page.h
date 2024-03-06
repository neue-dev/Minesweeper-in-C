/**
 * @ Author: MMMM
 * @ Create Time: 2024-03-02 21:58:49
 * @ Modified time: 2024-03-06 13:33:43
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
#include "./utils.types.h"

#define PAGE_MAX_COMPONENTS (1 << 10)
#define PAGE_MAX_STATES (1 << 10)
#define PAGE_MAX_COUNT (1 << 4)
#define PAGE_MAX_NAME_LEN (1 << 8)

typedef enum PageStatus PageStatus;

typedef struct Page Page;
typedef struct PageManager PageManager;

enum PageStatus {
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
  char *sName[STRING_KEY_MAX_LENGTH];                           // An identifier for the page
  char *sNextName;                                              // The name of the next page to render
                      
  PageStatus ePageStatus;                                       // The current status of the page
  f_page_handler fHandler;                                      // Handles the update of the page
                    
  AssetManager *pSharedAssetManager;                            // A reference to a shared asset manager so we can access all assets
  EventStore *pSharedEventStore;                                // Where we can access values modified by events
                    
  int dComponentCount;                                          // How many components we have
  ComponentManager componentManager;                            // We store the components both through a tree and a hashmap
  Buffer *pBuffer;                                              // This is where all our content will be displayed
                    
  HashMap *pColorStates;                                        // The foreground and background colors for each component
  HashMap *pRenderStates;                                       // Float states that describe the positions of different components
  HashMap *pPixelStates;                                        // These are the rounded off versions of the pRenderStates above
                      
  HashMap *pColorTargetStates;                                  // What colors to be approached
  HashMap *pRenderTargetStates;                                 // These are what the render states try to approach
  HashMap *pTransitionSpeeds;                                   // How fast each component should switch between states

  char sStateKeyArray[PAGE_MAX_STATES][STRING_KEY_MAX_LENGTH];  // The keys to all our states
  int dStateCount;                                              // How many states we have at the moment
                    
  unsigned long long dT;                                        // A variable that stores the current frame number
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
 * @param   { f_page_handler }  fHandler              A function that updates the page over time.
 * @return	{ Page * }					                      A pointer to the initialized instance.
*/
Page *Page_init(Page *this, AssetManager *pSharedAssetManager, EventStore *pSharedEventStore, f_page_handler fHandler) {
  
  // Initialize the handler and component manager
  this->fHandler = fHandler;
  this->dComponentCount = 1;                        // The root component
  ComponentManager_init(&this->componentManager);   // Init the component tree and other stuff

  // Init the shared resources
  this->pSharedAssetManager = pSharedAssetManager;
  this->pSharedEventStore = pSharedEventStore;

  // Empty hashmaps
  this->pColorStates = HashMap_create();
  this->pRenderStates = HashMap_create();
  this->pPixelStates = HashMap_create();
  
  this->pColorTargetStates = HashMap_create();
  this->pRenderTargetStates = HashMap_create();
  this->pTransitionSpeeds = HashMap_create();

  // Set dT to 0 and state count to 0
  this->dStateCount = 0;
  this->dT = 0ULL;

  return this;
}

/**
 * Creates an initialized instance of the Page class.
 * 
 * @param   { AssetManager * }      pSharedAssetManager   A reference to the manager that stores all our assets.
 * @param   { EventStore * }        pSharedEventStore     A reference to an event store instance that helps use deal with shared event states.
 * @return	{ Page * }					                          A pointer to the initialized instance.
*/
Page *Page_create(AssetManager *pSharedAssetManager, EventStore *pSharedEventStore, f_page_handler fHandler) {
  return Page_init(Page_new(), pSharedAssetManager, pSharedEventStore, fHandler);
}

/**
 * Deallocates the memory of an instance of the Page class.
 * 
 * @param		{ Page * }		this	A pointer to the instance to deallocate.
*/
void Page_kill(Page *this) {
  if(this->pBuffer != NULL)
    Buffer_kill(this->pBuffer);
  
  free(this);
}

/**
 * Actually place things into the buffer
 * 
 * @param   { Page * }  this  The page to render.
*/
void Page_render(Page *this) {
  ComponentManager_render(&this->componentManager, this->pBuffer);
}

/**
 * Performs a single frame update of our page instance.
 * Returns a boolean that indicates whether or not the page was able to update.
 * 
 * @param		{ Page * }		this	A pointer to the instance to update.
 * @param   { int }             Whether or not the page was able to update.
*/
int Page_update(Page *this) {
  
  int i, j;
  float *pRenderState, *pRenderTargetState, *pTransitionSpeed;
  int *pColorState, *pColorTargetState, *pPixelState;
  char sStateKey[STRING_KEY_MAX_LENGTH];
  
  Component *pComponent;
  char *sComponentKeyArray[PAGE_MAX_COMPONENTS];

  // The page isn't active
  if(this->ePageStatus == PAGE_INACTIVE)
    return 0;

  // If the page stopped after that runner update
  if(this->ePageStatus == PAGE_ACTIVE_IDLE)
    return 0;

  // Update the page states
  this->fHandler(this);
  
  // Perform some fixins
  for(i = 0; i < this->dStateCount; i++) {

    // Get the key
    strcpy(sStateKey, this->sStateKeyArray[i]);

    // Retrieve the values
    pColorState = HashMap_get(this->pColorStates, sStateKey);
    pRenderState = HashMap_get(this->pRenderStates, sStateKey);
    pPixelState = HashMap_get(this->pPixelStates, sStateKey);

    pColorTargetState = HashMap_get(this->pColorTargetStates, sStateKey);
    pRenderTargetState = HashMap_get(this->pRenderTargetStates, sStateKey);
    pTransitionSpeed = HashMap_get(this->pTransitionSpeeds, sStateKey);

    // Ease out
    if(*pTransitionSpeed > 0) {
      *pRenderState = Math_easeOut(*pRenderState, *pRenderTargetState, *pTransitionSpeed);
      *pColorState = Math_easeOut(*pColorState * 1.0, *pColorTargetState * 1.0, *pTransitionSpeed);

    // Ease in
    } else if(*pTransitionSpeed < 0) {
      *pRenderState = Math_easeIn(*pRenderState, *pRenderTargetState, *pTransitionSpeed * -1.0);
      *pColorState = Math_easeOut(*pColorState * 1.0, *pColorTargetState * 1.0, *pTransitionSpeed * -1.0);
    }

    // Update the state values
    *pPixelState = round(*pRenderState);
  }

  // Update component states
  HashMap_getKeys(this->componentManager.pComponentMap, sComponentKeyArray);
  
  for(i = 0; i < this->dComponentCount; i++) {

    // Get the component first
    pComponent = HashMap_get(this->componentManager.pComponentMap, sComponentKeyArray[i]);

    // If we're not dealing witht the root
    if(strcmp(pComponent->sName, "root")) {

      // For each of the associated states
      for(j = 0; j < 4; j++) {

        // Create the key
        String_keyAndId(sStateKey, pComponent->sName, j);
        
        // Retrieve the values
        pColorState = HashMap_get(this->pColorStates, sStateKey);
        pRenderState = HashMap_get(this->pRenderStates, sStateKey);
        pPixelState = HashMap_get(this->pPixelStates, sStateKey);

        // Put the values into the component
        switch(j) {
          case 0: pComponent->x = *pPixelState; break;
          case 1: pComponent->y = *pPixelState; break;
          case 2: pComponent->w = *pPixelState; break;
          case 3: pComponent->h = *pPixelState; break;
        }
      }
    }
  }
    
  // Increment time state
  this->dT++;

  // The page is currently initializing
  if(this->ePageStatus == PAGE_ACTIVE_INIT) {
    this->ePageStatus = PAGE_ACTIVE_RUNNING;

    // We return 0 because we don't want to render until after initting
    return 0;
  }

  return 1;
}

/**
 * Adds a new component to the page.
 * 
 * @param   { char * }                sKey          An identifier for the component.
 * @param   { char * }                sParentKey    The key of the component to append to.
 * @param   { int }                   x             The x-coordinate of the component.
 * @param   { int }                   y             The y-coordinate of the component.
 * @param   { int }                   w             The width of the component.
 * @param   { int }                   h             The height of the component.
 * @param   { int }                   dAssetHeight  The height of the asset. This can be 0.
 * @param   { char ** }               aAsset        The asset to be rendered by the component. This may be NULL.
 * @param   { int }                   colorFG       A foreground color for the component.
 * @param   { int }                   colorBG       A background color for the component.
*/
void Page_addComponent(Page *this, char *sKey, char *sParentKey, int x, int y, int w, int h, int dAssetHeight, char **aAsset, int colorFG, int colorBG) {
  int i;
  float *pRenderState, *pRenderTargetState, *pTransitionSpeed;
  int *pColorState, *pColorTargetState, *pPixelState;
  char sStateKey[STRING_KEY_MAX_LENGTH];

  if(this->dComponentCount >= PAGE_MAX_COMPONENTS)
    return;

  // Add a component
  ComponentManager_add(&this->componentManager, sKey, sParentKey, x, y, w, h, dAssetHeight, aAsset, colorFG, colorBG);

  // Create a new state for that component
  // Each component is always given 4 states
  for(i = 0; i < 4; i++) {

    // We don't kill the string because its added to the hashmaps below
    String_keyAndId(sStateKey, sKey, i);

    // Create pointers
    pRenderState = calloc(1, sizeof(float));
    pColorState = calloc(1, sizeof(int));
    pPixelState = calloc(1, sizeof(int));

    pRenderTargetState = calloc(1, sizeof(float));
    pColorTargetState = calloc(1, sizeof(int));
    pTransitionSpeed = calloc(1, sizeof(float));

    // Generate allocations for each state
    HashMap_add(this->pColorStates, sStateKey, pColorState);
    HashMap_add(this->pRenderStates, sStateKey, pRenderState);
    HashMap_add(this->pPixelStates, sStateKey, pPixelState);

    HashMap_add(this->pColorTargetStates, sStateKey, pColorTargetState);
    HashMap_add(this->pRenderTargetStates, sStateKey, pRenderTargetState);
    HashMap_add(this->pTransitionSpeeds, sStateKey, pTransitionSpeed);

    // Set the values of the states
    *pTransitionSpeed = 0.75;
    
    // Set appropriate values
    switch(i) {
      case 0:
        *pRenderTargetState = *pRenderState = (*pPixelState = x) * 1.0;
        *pColorTargetState = *pColorState = colorFG;
      break;
      
      case 1:
        *pRenderTargetState = *pRenderState = (*pPixelState = y) * 1.0;
        *pColorTargetState = *pColorState = colorBG;
      break;
      
      case 2:
        *pRenderTargetState = *pRenderState = (*pPixelState = y) * 1.0;
      break;

      case 3:
        *pRenderTargetState = *pRenderState = (*pPixelState = y) * 1.0;
      break;
    }

    // Append the state key to the array
    strcpy(this->sStateKeyArray[this->dStateCount++], sStateKey);    
  }

  this->dComponentCount++;
}

/**
 * Sets the target position for a certain component.
 * Also requires to specify the speed at which the component should get there.
*/
void Page_setComponentTarget(Page *this, char *sKey, int x, int y, int w, int h, int colorFG, int colorBG, float fTransitionSpeed) {
  int i;
  float *pRenderTargetState, *pTransitionSpeed;
  int *pColorTargetState;
  char sStateKey[STRING_KEY_MAX_LENGTH];
  
  for(i = 0; i < 4; i++) {

    // Create the key
    String_keyAndId(sStateKey, sKey, i);

    // Retrieve the values
    pColorTargetState = HashMap_get(this->pColorTargetStates, sStateKey);
    pRenderTargetState = HashMap_get(this->pRenderTargetStates, sStateKey);
    pTransitionSpeed = HashMap_get(this->pTransitionSpeeds, sStateKey);

    // Set the values of the states
    *pTransitionSpeed = fTransitionSpeed;
    
    // Set appropriate values
    switch(i) {
      case 0:
        *pRenderTargetState = x * 1.0;

        if(colorFG > -1)
          *pColorTargetState = colorFG;
      break;
      
      case 1:
        *pRenderTargetState = y * 1.0;

        if(colorBG > -1)
          *pColorTargetState = colorBG;
      break;
      
      case 2:
        if(w > -1)
          *pRenderTargetState = w * 1.0;
      break;

      case 3:
        if(h > -1)
          *pRenderTargetState = w * 1.0;
      break;
    }
  }
}

/**
 * Sets the page state to PAGE_ACTIVE.
 * 
 * @param   { Page * }  this  The page we're going to activate.
*/
void Page_activate(Page *this) {
  this->ePageStatus = PAGE_ACTIVE_INIT;
}

/**
 * Sets the page state to PAGE_INACTIVE.
 * 
 * @param   { Page * }  this  The page we're going to deactivate.
*/
void Page_deactivate(Page *this) {
  this->ePageStatus = PAGE_INACTIVE;
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

  char *sPageKeyArray[PAGE_MAX_COUNT];    // We need this for certain operations.
  char *sActivePage;                  // An identifier to the active page.
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
void PageManager_createPage(PageManager *this, char *sPageKey, f_page_handler fHandler) {
  Page *pPage = Page_create(this->pSharedAssetManager, this->pSharedEventStore, fHandler);

  // Set the created page as the active page (by default)
  strcpy(this->sActivePage, sPageKey);

  // Add it to the hashmap and update our count
  HashMap_add(this->pPageMap, sPageKey, pPage);

  // Store the key and increment the count
  this->sPageKeyArray[this->dPageCount] = sPageKey;
  this->dPageCount++;
}

/**
 * Returns the state of the active page.
 * 
 * @param   { PageManager * }   this  The page manager we want to read.
 * @return  { int }                   An enum indicating the state of the page.
*/
int PageManager_getActiveState(PageManager *this) {
  return ((Page *) HashMap_get(this->pPageMap, this->sActivePage))->ePageStatus;
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
 * // ! rename this function to something other than update
 * 
 * @param   { PageManager * }   this      The page manager object.
*/
void PageManager_update(PageManager *this) {
  Page *pPage = HashMap_get(this->pPageMap, this->sActivePage);

  if(Page_update(pPage))
    Page_render(pPage);
}

#endif
