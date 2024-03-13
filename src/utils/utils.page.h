/**
 * @ Author: MMMM
 * @ Create Time: 2024-03-02 21:58:49
 * @ Modified time: 2024-03-14 00:50:35
 * @ Description:
 * 
 * The page class bundles together a buffer, shared assets, shared event stores, and an runner manager. 
 * The class handles interactions between these three components and helps decouple our classes.
 */

#ifndef UTILS_PAGE_
#define UTILS_PAGE_

#include "./utils.event.h"
#include "./utils.theme.h"
#include "./utils.component.h"
#include "./utils.buffer.h"
#include "./utils.asset.h"
#include "./utils.types.h"

// This is used as a parameter to "setComponentTarget()"
// It signifies that an int value will not be modified
#define PAGE_NULL_INT -999

// Some other page state related stuff
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
  
  char *sNextName;                                              // The name of the next page to render
  PageStatus ePageStatus;                                       // The current status of the page
  f_page_handler fHandler;                                      // Handles the update of the page
                    
  AssetManager *pSharedAssetManager;                            // A reference to a shared asset manager so we can access all assets
  EventStore *pSharedEventStore;                                // Where we can access values modified by events
  ThemeManager *pSharedThemeManager;                            // What we use to manager the colors across pages
                    
  int dComponentCount;                                          // How many components we have
  ComponentManager componentManager;                            // We store the components both through a tree and a hashmap
  Buffer *pBuffer;                                              // This is where all our content will be displayed
  HashMap *pUserStates;                                         // This are custom user-defined states (which we can use for selectors, etc.)                    

  HashMap *pColorStates;                                        // The foreground and background colors for each component
  HashMap *pRenderStates;                                       // Float states that describe the positions of different components
  HashMap *pPixelStates;                                        // These are the rounded off versions of the pRenderStates above
                      
  HashMap *pColorTargetStates;                                  // What colors to be approached
  HashMap *pRenderTargetStates;                                 // These are what the render states try to approach
  HashMap *pTransitionSpeeds;                                   // How fast each component should switch between states

  char sStateKeyArray[PAGE_MAX_STATES][STRING_KEY_MAX_LENGTH];  // The keys to all our states
  int dStateCount;                                              // How many states we have at the moment
                    
  unsigned long long dT;                                        // A variable that stores the current frame number
  unsigned int dStage;                                          // An int that tells us what stage anims are in
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
 * @param   { ThemeManager * }  pSharedThemeManager   A reference to a shared theme manager.
 * @param   { f_page_handler }  fHandler              A function that updates the page over time.
 * @return	{ Page * }					                      A pointer to the initialized instance.
*/
Page *Page_init(Page *this, AssetManager *pSharedAssetManager, EventStore *pSharedEventStore, ThemeManager *pSharedThemeManager, f_page_handler fHandler) {
  
  // Initialize the handler and component manager
  this->fHandler = fHandler;
  this->dComponentCount = 1;                        // The root component
  ComponentManager_init(&this->componentManager);   // Init the component tree and other stuff
  
  // Currently none
  this->sNextName = NULL;

  // Init the shared resources
  this->pSharedAssetManager = pSharedAssetManager;
  this->pSharedEventStore = pSharedEventStore;
  this->pSharedThemeManager = pSharedThemeManager;

  // Empty hashmaps
  this->pUserStates = HashMap_create();
  this->pColorStates = HashMap_create();
  this->pRenderStates = HashMap_create();
  this->pPixelStates = HashMap_create();
  this->pColorTargetStates = HashMap_create();
  this->pRenderTargetStates = HashMap_create();
  this->pTransitionSpeeds = HashMap_create();

  // Currently no states
  this->dStateCount = 0;
  
  // Set dT to 0 and dStage to 0
  this->dT = 0ULL;
  this->dStage = 0;

  return this;
}

/**
 * Creates an initialized instance of the Page class.
 * 
 * @param   { AssetManager * }      pSharedAssetManager   A reference to the manager that stores all our assets.
 * @param   { EventStore * }        pSharedEventStore     A reference to an event store instance that helps use deal with shared event states.
 * @param   { ThemeManager * }      pSharedThemeManager   A reference to something to manage our themes for us.
 * @return	{ Page * }					                          A pointer to the initialized instance.
*/
Page *Page_create(AssetManager *pSharedAssetManager, EventStore *pSharedEventStore, ThemeManager *pSharedThemeManager, f_page_handler fHandler) {
  return Page_init(Page_new(), pSharedAssetManager, pSharedEventStore, pSharedThemeManager, fHandler);
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
 * Sets a specific user-defined state to the value provided.
 * Note that user states can only store char data.
 * 
 * @param   { Page * }  this        The page to modify.
 * @param   { char * }  sStateKey   The key of state to modify.
 * @param   { char }    cData       The new data to store for the state.
*/
void Page_setUserState(Page *this, char *sStateKey, char cData) {
  char *cDataEntry = HashMap_get(this->pUserStates, sStateKey);

  // The entry doesnt exist yet
  if(cDataEntry == NULL) {
    cDataEntry = calloc(1, sizeof(char));  
    HashMap_add(this->pUserStates, sStateKey, cDataEntry);
  }

  // Store the data in the hashmap
  *cDataEntry = cData;
}

/**
 * Gets a specific user-defined state.
 * Note that user states can only store char data.
 * 
 * @param   { Page * }  this        The page to read.
 * @param   { char * }  sStateKey   The key of state to read.
*/
char Page_getUserState(Page *this, char *sStateKey) {
  if(HashMap_get(this->pUserStates, sStateKey) == NULL)
    return -1;

  return *(char *) HashMap_get(this->pUserStates, sStateKey);
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
      *pColorState = Math_easeIn(*pColorState * 1.0, *pColorTargetState * 1.0, *pTransitionSpeed * -1.0);
    }

    // Update the state values
    *pPixelState = (int) round(*pRenderState);
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
 * @param   { char * }                sColorFGKey   A color key for the foreground from the theme manager.
 * @param   { char * }                sColorBGKey   A color key for the background from the theme manager.
*/
void Page_addComponent(Page *this, char *sKey, char *sParentKey, int x, int y, int w, int h, int dAssetHeight, char **aAsset, char *sColorFGKey, char *sColorBGKey) {
  int i;

  // The different component states
  float *pRenderState, *pRenderTargetState, *pTransitionSpeed;
  int *pColorState, *pColorTargetState, *pPixelState;
  char sStateKey[STRING_KEY_MAX_LENGTH];

  // The colors we want from the theme  
  color colorFG = ThemeManager_getActive(this->pSharedThemeManager, sColorFGKey);
  color colorBG = ThemeManager_getActive(this->pSharedThemeManager, sColorBGKey);

  // Not too many components
  if(this->dComponentCount >= PAGE_MAX_COMPONENTS)
    return;

  // Add a component; exit if not successful
  if(!ComponentManager_add(&this->componentManager, sKey, sParentKey, x, y, w, h, dAssetHeight, aAsset, colorFG, colorBG))
    return;

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
        *pRenderTargetState = *pRenderState = (*pPixelState = w) * 1.0;
      break;

      case 3:
        *pRenderTargetState = *pRenderState = (*pPixelState = h) * 1.0;
      break;
    }

    // Append the state key to the array
    strcpy(this->sStateKeyArray[this->dStateCount++], sStateKey);    
  }

  this->dComponentCount++;
}

/**
 * Adds a component with the asset of a given key.
 * Note that the width and height of the component are automatically determined by the asset.
 * 
 * @param   { Page * }                this          The page to modify.
 * @param   { char * }                sKey          An identifier for the component.
 * @param   { char * }                sParentKey    The key of the component to append to.
 * @param   { int }                   x             The x-coordinate of the component.
 * @param   { int }                   y             The y-coordinate of the component.
 * @param   { char * }                sColorFGKey   A color key for the foreground from the theme manager.
 * @param   { char * }                sColorBGKey   A color key for the background from the theme manager.
 * @param   { char * }                sAssetKey     The asset to be rendered by the component.
*/
void Page_addComponentAsset(Page *this, char *sKey, char *sParentKey, int x, int y, char *sColorFGKey, char *sColorBGKey, char *sAssetKey) {
  Page_addComponent(this, sKey, sParentKey, x, y, 
    AssetManager_getAssetWidth(this->pSharedAssetManager, sAssetKey),
    AssetManager_getAssetHeight(this->pSharedAssetManager, sAssetKey),
    AssetManager_getAssetHeight(this->pSharedAssetManager, sAssetKey),
    AssetManager_getAssetText(this->pSharedAssetManager, sAssetKey),
    sColorFGKey, sColorBGKey);
}

/**
 * Makes it easier to insert plain text into the page without having to convert the text into an asset.
 * 
 * @param   { Page * }                this          The page to modify.
 * @param   { char * }                sKey          An identifier for the component.
 * @param   { char * }                sParentKey    The key of the component to append to.
 * @param   { int }                   x             The x-coordinate of the component.
 * @param   { int }                   y             The y-coordinate of the component.
 * @param   { char * }                sColorFGKey   A color key for the foreground from the theme manager.
 * @param   { char * }                sColorBGKey   A color key for the background from the theme manager.
 * @param   { int }                   dWrapLength   How wide the text block can be. If 0, then 256 is the default width.
 * @param   { char * }                sText         The text we want to add to the page.
*/
void Page_addComponentText(Page *this, char *sKey, char *sParentKey, int x, int y, char *sColorFGKey, char *sColorBGKey, char *sText) {
  int dChar = 0, dLines = 0, dLineLength = 1024; 
  char **aAsset = calloc(256, sizeof(*aAsset));

  // Init the first line
  aAsset[dLines] = String_alloc(dLineLength);

  // Peruse the text
  while(*sText) {
    
    // Copy character to current line
    if(*sText != '\n' && dChar < dLineLength) {
      aAsset[dLines][dChar++] = *sText;
    
    // New line
    } else {
      dLines++;
      dChar = 0;

      aAsset[dLines] = String_alloc(dLineLength);
    }

    sText++;
  }
  
  Page_addComponent(this, sKey, sParentKey, x, y, 
    String_charCount(aAsset[0]), dLines + 1, dLines + 1, aAsset, sColorFGKey, sColorBGKey);
}

/**
 * Adds a component with the no asset and no colors.
 * Note that the width and height of the component may be determined by its children,
 *    depending on its alignment type.
 * 
 * @param   { Page * }                this          The page to modify.
 * @param   { char * }                sKey          An identifier for the component.
 * @param   { char * }                sParentKey    The key of the component to append to.
 * @param   { int }                   x             The x-coordinate of the component.
 * @param   { int }                   y             The y-coordinate of the component.
*/
void Page_addComponentContainer(Page *this, char *sKey, char *sParentKey, int x, int y) {
  Page_addComponent(this, sKey, sParentKey, x, y, 0, 0, 0, NULL, "", "");
}

/**
 * Adds a component with the no asset and just colors.
 * Note that the width and height of the component may be determined by its children,
 *    depending on its alignment type.
 * 
 * @param   { Page * }                this          The page to modify.
 * @param   { char * }                sKey          An identifier for the component.
 * @param   { char * }                sParentKey    The key of the component to append to.
 * @param   { int }                   x             The x-coordinate of the component.
 * @param   { int }                   y             The y-coordinate of the component.
 * @param   { int }                   w             The width of the component.
 * @param   { int }                   h             The height of the component.
 * @param   { char * }                sColorFGKey   A color key for the foreground from the theme manager.
 * @param   { char * }                sColorBGKey   A color key for the background from the theme manager.
*/
void Page_addComponentContext(Page *this, char *sKey, char *sParentKey, int x, int y, int w, int h, char *sColorFGKey, char *sColorBGKey) {
  Page_addComponent(this, sKey, sParentKey, x, y, w, h, 0, NULL, sColorFGKey, sColorBGKey);
}

/**
 * Sets the target position for a certain component.
 * Also requires to specify the speed at which the component should get there.
 * 
 * @param   { Page * }                this              The page to modify.
 * @param   { char * }                sKey              An identifier for the component.
 * @param   { int }                   x                 The x-coordinate the component will go to. PAGE_INT_NULL if none.
 * @param   { int }                   y                 The y-coordinate the component will go to. PAGE_INT_NULL if none.
 * @param   { int }                   w                 The width the component will approach. -1 if none.
 * @param   { int }                   h                 The height the component will approach. -1 if none.
 * @param   { char * }                sColorFGKey       A color key for the foreground from the theme manager.
 * @param   { char * }                sColorBGKey       A color key for the background from the theme manager.
 * @param   { float }                 fTransitionSpeed  How fast the component will go its target values.
*/
void Page_setComponentTarget(Page *this, char *sKey, int x, int y, int w, int h, char *sColorFGKey, char *sColorBGKey, float fTransitionSpeed) {
  int i;

  // The target and transition parameters
  float *pRenderTargetState, *pTransitionSpeed;
  int *pColorTargetState;
  char sStateKey[STRING_KEY_MAX_LENGTH];

  // The colors we want from the theme  
  color colorFG = ThemeManager_getActive(this->pSharedThemeManager, sColorFGKey);
  color colorBG = ThemeManager_getActive(this->pSharedThemeManager, sColorBGKey);
  
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
        if(x > PAGE_NULL_INT)
          *pRenderTargetState = x * 1.0;

        if(colorFG > -1)
          *pColorTargetState = colorFG;
      break;
      
      case 1:
        if(y > PAGE_NULL_INT)
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
          *pRenderTargetState = h * 1.0;
      break;
    }
  }
}

/**
 * A shorthand function for _setComponentTarget().
 * Only accepts parameters for changing position.
 * 
 * @param   { Page * }  this              The page to modify.
 * @param   { char * }  sKey              The component to modify.
 * @param   { int }     x                 The target x of the component.
 * @param   { int }     y                 The target y of the component.
 * @param   { float }   fTransitionSpeed  How fast the component should move.
*/
void Page_setComponentTargetPosition(Page *this, char *sKey, int x, int y, float fTransitionSpeed) {
  Page_setComponentTarget(this, sKey, x, y, -1, -1, "", "", fTransitionSpeed);
}

/**
 * A shorthand function for _setComponentTarget().
 * Only accepts parameters for changing size.
 * 
 * @param   { Page * }  this              The page to modify.
 * @param   { char * }  sKey              The component to modify.
 * @param   { int }     w                 The target width of the component.
 * @param   { int }     h                 The target height of the component.
 * @param   { float }   fTransitionSpeed  How fast the component should move.
*/
void Page_setComponentTargetSize(Page *this, char *sKey, int w, int h, float fTransitionSpeed) {
  Page_setComponentTarget(this, sKey, PAGE_NULL_INT, PAGE_NULL_INT, w, h, "", "", fTransitionSpeed);
}

/**
 * A shorthand function for _setComponentTarget().
 * Only accepts parameters for changing color.
 * 
 * @param   { Page * }  this              The page to modify.
 * @param   { char * }  sKey              The component to modify.
 * @param   { char * }  sColorFGKey       A color key for the foreground from the theme manager.
 * @param   { char * }  sColorBGKey       A color key for the background from the theme manager.
 * @param   { float }   fTransitionSpeed  How fast the component should move.
*/
void Page_setComponentTargetColor(Page *this, char *sKey, char *sColorFGKey, char *sColorBGKey, float fTransitionSpeed) {
  Page_setComponentTarget(this, sKey, PAGE_NULL_INT, PAGE_NULL_INT, -1, -1, sColorFGKey, sColorBGKey, fTransitionSpeed);
}

/**
 * A shorthand function for _setComponentTarget().
 * Only accepts parameters for transition speed.
 * 
 * @param   { Page * }  this              The page to modify.
 * @param   { char * }  sKey              The component to modify.
 * @param   { float }   fTransitionSpeed  How fast the component should move.
*/
void Page_setComponentTransitionSpeed(Page *this, char *sKey, float fTransitionSpeed) {
  Page_setComponentTarget(this, sKey, PAGE_NULL_INT, PAGE_NULL_INT, -1, -1, "", "", fTransitionSpeed);
}

/**
 * Returns the distance of the component from its target.
 * 
 * @param   { Page * }  this    The page to modify.
 * @param   { char * }  sKey    The component to read data from.
 * @param   { int }     dParam  Which value we're specifically getting the distances of.
*/
float Page_getComponentDist(Page *this, char *sKey, int dParam) {
  int i;
  float *pRenderState, *pRenderTargetState;
  int *pColorState, *pColorTargetState;
  char sStateKey[STRING_KEY_MAX_LENGTH];
  
  // A for loop makes things easier here
  for(i = 0; i < 4; i++) {

    // Create the key
    String_keyAndId(sStateKey, sKey, i);

    // Retrieve the values
    pColorState = HashMap_get(this->pColorStates, sStateKey);
    pRenderState = HashMap_get(this->pRenderStates, sStateKey);

    pColorTargetState = HashMap_get(this->pColorTargetStates, sStateKey);
    pRenderTargetState = HashMap_get(this->pRenderTargetStates, sStateKey);
    
    // Set appropriate values
    // I KNOW this switch statement couldve been simplified but Id rather keep it explicit for my own sake
    switch(dParam) {
      case 0:   // Distance of x values
        if(i == 0) { return Math_dist1d(*pRenderState, *pRenderTargetState); } break;
      case 1:   // Distance of y values
        if(i == 1) { return Math_dist1d(*pRenderState, *pRenderTargetState); } break;
      case 2:   // Distance of w values
        if(i == 2) { return Math_dist1d(*pRenderState, *pRenderTargetState); } break;
      case 3:   // Distance of h values
        if(i == 3) { return Math_dist1d(*pRenderState, *pRenderTargetState); } break;
      
      case 5:   // Distance of FG color values
        if(i == 0) { return Graphics_getColorDist(*pColorState, *pColorTargetState); } break;
      case 6:   // Distance of FG color values
        if(i == 1) { return Graphics_getColorDist(*pColorState, *pColorTargetState); } break;
    }
  }

  return 0.0;
}

/**
 * Sets the initial starting values of the component.
 * 
 * @param   { Page * }                this              The page to modify.
 * @param   { char * }                sKey              An identifier for the component.
 * @param   { int }                   x                 The x-coordinate the component will start at. PAGE_INT_NULL if none.
 * @param   { int }                   y                 The y-coordinate the component will start at. PAGE_INT_NULL if none.
 * @param   { int }                   w                 The width the component will start with. -1 if none.
 * @param   { int }                   h                 The height the component will start with. -1 if none.
 * @param   { char * }                sColorFGKey       A color key for the foreground from the theme manager.
 * @param   { char * }                sColorBGKey       A color key for the background from the theme manager.
*/
void Page_resetComponentInitial(Page *this, char *sKey, int x, int y, int w, int h, char *sColorFGKey, char *sColorBGKey) {
  int i;
  
  // The initial states
  float *pRenderState, *pRenderTargetState;
  int *pColorState, *pColorTargetState;
  char sStateKey[STRING_KEY_MAX_LENGTH];

  // The colors we want from the theme  
  color colorFG = ThemeManager_getActive(this->pSharedThemeManager, sColorFGKey);
  color colorBG = ThemeManager_getActive(this->pSharedThemeManager, sColorBGKey);
  
  for(i = 0; i < 4; i++) {

    // Create the key
    String_keyAndId(sStateKey, sKey, i);

    // Retrieve the values
    pColorState = HashMap_get(this->pColorStates, sStateKey);
    pRenderState = HashMap_get(this->pRenderStates, sStateKey);

    pColorTargetState = HashMap_get(this->pColorTargetStates, sStateKey);
    pRenderTargetState = HashMap_get(this->pRenderTargetStates, sStateKey);
    
    // Set appropriate values
    switch(i) {
      case 0:
        if(x > PAGE_NULL_INT)
          *pRenderTargetState = *pRenderState = x * 1.0;

        if(colorFG > -1)
          *pColorTargetState = *pColorState = colorFG;
      break;
      
      case 1:
        if(y > PAGE_NULL_INT)
          *pRenderTargetState = *pRenderState = y * 1.0;

        if(colorBG > -1)
          *pColorTargetState = *pColorState = colorBG;
      break;
      
      case 2:
        if(w > -1)
          *pRenderTargetState = *pRenderState = w * 1.0;
      break;

      case 3:
        if(h > -1)
          *pRenderTargetState = *pRenderState = h * 1.0;
      break;
    }
  }
}

/**
 * A shorthand function for _resetComponentInitial().
 * Sets only the position of a component.
 * 
 * @param   { Page * }                this              The page to modify.
 * @param   { char * }                sKey              An identifier for the component.
 * @param   { int }                   x                 The x-coordinate the component will start at. PAGE_INT_NULL if none.
 * @param   { int }                   y                 The y-coordinate the component will start at. PAGE_INT_NULL if none.
*/
void Page_resetComponentInitialPosition(Page *this, char *sKey, int x, int y) {
  Page_resetComponentInitial(this, sKey, x, y, -1, -1, "", "");
}

/**
 * A shorthand function for _resetComponentInitial().
 * Sets only the size of a component.
 * 
 * @param   { Page * }                this              The page to modify.
 * @param   { char * }                sKey              An identifier for the component.
 * @param   { int }                   w                 The width the component will start at. -1 if none.
 * @param   { int }                   h                 The width the component will start at. -1 if none
*/
void Page_resetComponentInitialSize(Page *this, char *sKey, int w, int h) {
  Page_resetComponentInitial(this, sKey, PAGE_NULL_INT, PAGE_NULL_INT, w, h, "", "");
}

/**
 * A shorthand function for _resetComponentInitial().
 * Sets only the color of a component.
 * 
 * @param   { Page * }                this              The page to modify.
 * @param   { char * }                sKey              An identifier for the component.
 * @param   { char * }                sColorFGKey       A color key for the foreground from the theme manager.
 * @param   { char * }                sColorBGKey       A color key for the background from the theme manager.
*/
void Page_resetComponentInitialColor(Page *this, char *sKey, char *sColorFGKey, char *sColorBGKey) {
  Page_resetComponentInitial(this, sKey, PAGE_NULL_INT, PAGE_NULL_INT, -1, -1, sColorFGKey, sColorBGKey);
}

/**
 * Sets what page will be rendered next after the current page finishes running.
 * 
 * @param   { Page * }  this    The page to modify.
 * @param   { char * }  sNext   The name of the page to be rendered after the current one exits.
*/
void Page_setNext(Page *this, char *sNext) {
  this->sNextName = sNext;
}

/**
 * Sets the page status to PAGE_ACTIVE_IDLE.
 * 
 * @param   { Page * }  this.
*/
void Page_idle(Page *this) {
  this->ePageStatus = PAGE_ACTIVE_IDLE;  
}

/**
 * Sets the page status to PAGE_ACTIVE.
 * It also resets the dT value of the page so animations can start over.
 * 
 * @param   { Page * }  this  The page we're going to activate.
*/
void Page_activate(Page *this) {
  this->ePageStatus = PAGE_ACTIVE_INIT;
  this->dT = 0;
  this->dStage = 0;
}

/**
 * Sets the page status to PAGE_INACTIVE.
 * 
 * @param   { Page * }  this  The page we're going to deactivate.
*/
void Page_deactivate(Page *this) {
  this->ePageStatus = PAGE_INACTIVE;
}

/**
 * Increments the stage counter of the page.
 * 
 * @param   { Page * }  The page to modify.
*/
void Page_nextStage(Page *this) {
  this->dStage++;
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
  AssetManager *pSharedAssetManager;                          // A reference to the shared asset manager so we only have to pass it during init
  EventStore *pSharedEventStore;                              // A reference to a shared event store so we can access things changed by events
  ThemeManager *pSharedThemeManager;                          // A reference to a shared theme manager which helps us manage the styling of our app

  HashMap *pPageMap;                                          // Stores all the pages we have
  int dPageCount;                                             // How many pages we have

  char sPageKeyArray[PAGE_MAX_COUNT][STRING_KEY_MAX_LENGTH];  // We need this for certain operations.
  char sActivePage[STRING_KEY_MAX_LENGTH];                    // An identifier to the active page.
};

/**
 * Initializes the page manager.
 * 
 * @param   { PageManager * }   this                  The page manager object.
 * @param   { AssetManager * }  pSharedAssetManager   The asset manager shared by all pages.
 * @param   { ThemeManager * }  pSharedThemeManager   The theme manager shared by all pages.
 * @param   { EventStore * }    pSharedEventStore     A reference to an event store instance that helps use deal with shared event states.
*/
void PageManager_init(PageManager *this, AssetManager *pSharedAssetManager, EventStore *pSharedEventStore, ThemeManager *pSharedThemeManager) {
  this->pPageMap = HashMap_create();
  this->dPageCount = 0;

  // So we dont need to pass it each time
  this->pSharedAssetManager = pSharedAssetManager;

  // So we dont need to pass a lot of things around
  this->pSharedEventStore = pSharedEventStore;

  // So we don't have to interact with this all the time
  this->pSharedThemeManager = pSharedThemeManager;
}

/**
 * Exits the page manager.
 * 
 * @param   { PageManager * }   this  The page manager to exit.
*/
void PageManager_exit(PageManager *this) {
  HashMap_kill(this->pPageMap);
}

/**
 * Creates a new page instance and stores it in the hashmap.
 * 
 * @param   { PageManager * }       this                  The page manager object.
 * @param   { char * }              sPageKey              An identifier for the page.
 * @param   { f_page_configurer }   fPageConfigurer       A function that configures a page.
*/
void PageManager_createPage(PageManager *this, char *sPageKey, f_page_handler fHandler) {
  Page *pPage;

  // We have a duplicate
  if(HashMap_get(this->pPageMap, sPageKey) != NULL)
    return;

  // Create the page
  pPage = Page_create(this->pSharedAssetManager, this->pSharedEventStore, this->pSharedThemeManager, fHandler);

  // Set the created page as the active page (by default)
  strcpy(this->sActivePage, sPageKey);

  // Add it to the hashmap and update our count
  HashMap_add(this->pPageMap, sPageKey, pPage);

  // Store the key and increment the count
  strcpy(this->sPageKeyArray[this->dPageCount], sPageKey);
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
 * 
 * @param   { PageManager * }   this      The page manager object.
*/
void PageManager_update(PageManager *this) {
  Page *pPage = HashMap_get(this->pPageMap, this->sActivePage);

  if(Page_update(pPage))
    Page_render(pPage);

  if(pPage->ePageStatus == PAGE_ACTIVE_IDLE && pPage->sNextName != NULL)
    PageManager_setActive(this, pPage->sNextName);
}

#endif
