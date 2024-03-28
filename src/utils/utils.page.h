/**
 * @ Author: MMMM
 * @ Create Time: 2024-03-02 21:58:49
 * @ Modified time: 2024-03-29 02:17:47
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
  p_obj pSharedObject;                                          // Can refer to any piece of shared state the page might need
                                                                // We use this primarily for the game objects
                    
  int dComponentCount;                                          // How many components we have
  ComponentManager componentManager;                            // We store the components both through a tree and a hashmap
  Buffer *pBuffer;                                              // This is where all our content will be displayed
  HashMap *pUserStates;                                         // This are custom user-defined states (which we can use for selectors, etc.)                    
                    
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
  this->dComponentCount = 1;  // The root component

  // Init the component tree and other stuff
  ComponentManager_init(&this->componentManager);
  
  // Currently none
  this->sNextName = NULL;

  // Init the shared resources
  this->pSharedAssetManager = pSharedAssetManager;
  this->pSharedEventStore = pSharedEventStore;
  this->pSharedThemeManager = pSharedThemeManager;
  this->pSharedObject = NULL;

  // Empty hashmaps
  this->pUserStates = HashMap_create();
  
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

  // The page isn't active
  if(this->ePageStatus == PAGE_INACTIVE)
    return 0;

  // If the page stopped after that runner update
  if(this->ePageStatus == PAGE_ACTIVE_IDLE)
    return 0;

  // Update the page states
  this->fHandler(this);
    
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

  // The colors we want from the theme  
  color colorFG = ThemeManager_getActive(this->pSharedThemeManager, sColorFGKey);
  color colorBG = ThemeManager_getActive(this->pSharedThemeManager, sColorBGKey);

  // Not too many components
  if(this->dComponentCount >= PAGE_MAX_COMPONENTS)
    return;

  // Add a component; exit if not successful
  if(!ComponentManager_add(&this->componentManager, sKey, sParentKey, x, y, w, h, dAssetHeight, aAsset, colorFG, colorBG))
    return;

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
 * Creates a popup component.
 * Appends the component to the root element by default.
 * The component is also screen centered by default.
 * 
 * //!jsdoc
*/
void Page_addComponentPopup(Page *this, char *sKey, int x, int y, int w, int h, char *sColorFGKey, char *sColorBGKey, char *sBodyText, char *sOption1, char *sOption2) {
  int i;

  // Holds the component keys
  char sPopupComponent[STRING_KEY_MAX_LENGTH];
  char sPopupBackgroundComponent[STRING_KEY_MAX_LENGTH];
  char sPopupTextComponent[STRING_KEY_MAX_LENGTH];
  char sPopupOptionsComponent[STRING_KEY_MAX_LENGTH];
  char sPopupOption1Component[STRING_KEY_MAX_LENGTH];
  char sPopupOption2Component[STRING_KEY_MAX_LENGTH];
  char sPopupButtonCurrentKey[STRING_KEY_MAX_LENGTH]; 
  char sPopupButtonCountKey[STRING_KEY_MAX_LENGTH];

  // Define the component keys
  sprintf(sPopupComponent, "popup-%s.fixed.acenter-x.acenter-y", sKey);
  sprintf(sPopupTextComponent, "popup-text-%s.acenter-x.atop-y", sKey);
  sprintf(sPopupOptionsComponent, "popup-options-%s.fixed.acenter-x.abottom-y", sKey);
  sprintf(sPopupOption1Component, "popup-option1-%s.aright-x.abottom-y", sKey);
  sprintf(sPopupOption2Component, "popup-option2-%s.aleft-x.abottom-y", sKey);

  // For handling the component
  sprintf(sPopupButtonCurrentKey, "popup-button-current-%s", sKey);
  sprintf(sPopupButtonCountKey, "popup-button-count-%s", sKey);

  // Create the popup and its background
  Page_addComponentContainer(this, sPopupComponent, "root", x, y);

  for(i = 0; i < h; i++) {
    sprintf(sPopupBackgroundComponent, "popup-%s-bg-clear-%d.acenter-x.atop-y", sKey, i);
    Page_addComponentText(this, sPopupBackgroundComponent, sPopupComponent, 0, i - h / 2, sColorFGKey, sColorFGKey, String_repeat(" ", w + 2));
    
    sprintf(sPopupBackgroundComponent, "popup-%s-bg-border-%d.acenter-x.atop-y", sKey, i);
    Page_addComponentText(this, sPopupBackgroundComponent, sPopupComponent, 0, i - h / 2, sColorBGKey, sColorBGKey, String_repeat(" ", w));
    
    if(i > 0 && i < h - 1) {
      sprintf(sPopupBackgroundComponent, "popup-%s-bg-%d.acenter-x.atop-y", sKey, i);
      Page_addComponentText(this, sPopupBackgroundComponent, sPopupComponent, 0, i - h / 2, sColorFGKey, sColorFGKey, String_repeat(" ", w - 4));
    }
  }
  
  // The popup body text and options
  Page_addComponentText(this, sPopupTextComponent, sPopupComponent, 0, -2, sColorBGKey, "", sBodyText);
  Page_addComponentContainer(this, sPopupOptionsComponent, sPopupComponent, 0, 2);
  Page_addComponentText(this, sPopupOption1Component, sPopupOptionsComponent, -2, 0, sColorFGKey, sColorBGKey, sOption1);
  Page_addComponentText(this, sPopupOption2Component, sPopupOptionsComponent, 2, 0, sColorBGKey, sColorFGKey, sOption2);

  // Make it hidden but above all elements
  ComponentManager_setZIndex(&this->componentManager, sPopupComponent, 1);
  ComponentManager_setHidden(&this->componentManager, sPopupComponent, 1);

  // Create the states of the component
  Page_setUserState(this, sPopupButtonCurrentKey, 0);
  Page_setUserState(this, sPopupButtonCountKey, strlen(sOption2) ? 2 : 1);
  Page_setUserState(this, "is-popup", 0);
}

/**
 * Changes the position of the component.
 * 
 * @param   { Page * }  this          The page we want to modify.
 * @param   { char * }  sKey          An identifier for the component we want to modify.
 * @param   { int }     x             The x-coordinate of the component.
 * @param   { int }     y             The y-coordinate of the component.
*/
void Page_setComponentPos(Page *this, char *sKey, int x, int y) {
  ComponentManager_setPos(&this->componentManager, sKey, x, y);
}

/**
 * Changes the size of the component.
 * 
 * @param   { Page * }  this          The page we want to modify.
 * @param   { char * }  sKey          An identifier for the component we want to modify.
 * @param   { int }     w             The width of the component.
 * @param   { int }     h             The height of the component.
*/
void Page_setComponentSize(Page *this, char *sKey, int w, int h) {
  ComponentManager_setSize(&this->componentManager, sKey, w, h);
}

/**
 * Changes the color of a component.
 * 
 * @param   { Page * }  this          The page we want to modify.
 * @param   { char * }  sKey          An identifier for the component we want to modify.
 * @param   { char * }  sColorFGKey   The foreground color of the component, based on theme.
 * @param   { char * }  sColorBGKey   The background color of the component, based on theme.
*/
void Page_setComponentColor(Page *this, char *sKey, char *sColorFGKey, char *sColorBGKey) {

  // The colors we want from the theme
  color colorFG = ThemeManager_getActive(this->pSharedThemeManager, sColorFGKey);
  color colorBG = ThemeManager_getActive(this->pSharedThemeManager, sColorBGKey);

  ComponentManager_setColor(&this->componentManager, sKey, 
    colorFG < 0 ? COMPONENT_NO_CHANGE : colorFG, 
    colorBG < 0 ? COMPONENT_NO_CHANGE : colorBG);
}

/**
 * Changes the text stored by a component.
 * 
 * @param   { Page * }  this          The page we want to modify.
 * @param   { char * }  sKey          An identifier for the component we want to modify.
 * @param   { char * }  sColorFGKey   The foreground color of the component, based on theme.
 * @param   { char * }  sColorBGKey   The background color of the component, based on theme.
*/
void Page_setComponentText(Page *this, char *sKey, char *sText) {

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
  
  // Update the size and content of the component
  ComponentManager_setSize(&this->componentManager, sKey, String_charCount(aAsset[0]), dLines + 1);
  ComponentManager_setAsset(&this->componentManager, sKey, dLines + 1, aAsset);
}

/**
 * Changes the text stored by a popup.
 * 
 * @param   { Page * }  this          The page we want to modify.
 * @param   { char * }  sKey          An identifier for the component we want to modify.
 * @param   { char * }  sText         The text for the popup.
*/
void Page_setComponentPopupText(Page *this, char *sKey, char *sText) {
  char sPopupTextComponent[STRING_KEY_MAX_LENGTH];
  sprintf(sPopupTextComponent, "popup-text-%s.acenter-x.atop-y", sKey);

  Page_setComponentText(this, sPopupTextComponent, sText);
}

/**
 * Resets the component tree of the page.
 * 
 * @param   { Page * }  this  The page to reset.
*/
void Page_resetComponents(Page *this) {
  ComponentManager_reset(&this->componentManager);
  this->dComponentCount = 1;
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
 * Enables a popup.
 * 
 * @param   { Page * }  this          The page we want to modify.
 * @param   { char * }  sKey          An identifier for the component we want to enable.
*/
void Page_enableComponentPopup(Page *this, char *sKey) {
  char sPopupComponent[STRING_KEY_MAX_LENGTH];
  sprintf(sPopupComponent, "popup-%s.fixed.acenter-x.acenter-y", sKey);

  // Make the component visible
  ComponentManager_setHidden(&this->componentManager, sPopupComponent, 0);

  // Enable component boolean
  Page_setUserState(this, "is-popup", 1);
}

/**
 * Disables a popup.
 * Also saves the response of the user in the page state.
 * 
 * @param   { Page * }  this          The page we want to modify.
 * @param   { char * }  sKey          An identifier for the component we want to disable.
*/
void Page_disableComponentPopup(Page *this, char *sKey) {
  char sPopupComponent[STRING_KEY_MAX_LENGTH];
  sprintf(sPopupComponent, "popup-%s.fixed.acenter-x.acenter-y", sKey);

  // Make the component visible
  ComponentManager_setHidden(&this->componentManager, sPopupComponent, 1);

  // Enable component boolean
  Page_setUserState(this, "is-popup", 0);
}

/**
 * Toggles a popup option.
 * 
 * @param   { Page * }  this          The page we want to modify.
 * @param   { char * }  sKey          An identifier for the component we want to disable.
*/
void Page_toggleComponentPopup(Page *this, char *sKey, char *sFGColorKey, char *sBGColorKey) {

  // Component keys and states
  char sPopupOption1Component[STRING_KEY_MAX_LENGTH];
  char sPopupOption2Component[STRING_KEY_MAX_LENGTH];
  char sPopupButtonCurrentKey[STRING_KEY_MAX_LENGTH]; 
  char sPopupButtonCountKey[STRING_KEY_MAX_LENGTH];
  char cPopupButtonCurrent;
  char cPopupButtonCount;

  // Define the component keys
  sprintf(sPopupOption1Component, "popup-option1-%s.aright-x.abottom-y", sKey);
  sprintf(sPopupOption2Component, "popup-option2-%s.aleft-x.abottom-y", sKey);

  // For handling the component
  sprintf(sPopupButtonCurrentKey, "popup-button-current-%s", sKey);
  sprintf(sPopupButtonCountKey, "popup-button-count-%s", sKey);

  // Get the page states and update them
  cPopupButtonCurrent = Page_getUserState(this, sPopupButtonCurrentKey);
  cPopupButtonCount = Page_getUserState(this, sPopupButtonCountKey);

  Page_setUserState(this, sPopupButtonCurrentKey, ((int) cPopupButtonCurrent + 1) % cPopupButtonCount);

  // Change the UI to reflect option change
  if(Page_getUserState(this, sPopupButtonCurrentKey) == 0) {
    Page_setComponentColor(this, sPopupOption1Component, sFGColorKey, sBGColorKey);
    Page_setComponentColor(this, sPopupOption2Component, sBGColorKey, sFGColorKey);
  } else {
    Page_setComponentColor(this, sPopupOption1Component, sBGColorKey, sFGColorKey);
    Page_setComponentColor(this, sPopupOption2Component, sFGColorKey, sBGColorKey);
  }
}

/**
 * Returns the response of the user to the popup.
 * In other words, which option they selected.
 * 
 * @param   { Page * }  this          The page we want to modify.
 * @param   { char * }  sKey          An identifier for the component we want to disable.
*/
int Page_readComponentPopup(Page *this, char *sKey) {
  char sPopupButtonCurrentKey[STRING_KEY_MAX_LENGTH];
  sprintf(sPopupButtonCurrentKey, "popup-button-current-%s", sKey);

  return (int) Page_getUserState(this, sPopupButtonCurrentKey);
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

  if(Page_update(pPage) && pPage->ePageStatus == PAGE_ACTIVE_RUNNING)
    Page_render(pPage);

  if(pPage->ePageStatus == PAGE_ACTIVE_IDLE && pPage->sNextName != NULL) {
    pPage->ePageStatus = PAGE_ACTIVE_INIT;
    PageManager_setActive(this, pPage->sNextName);
  }
}

/**
 * Gives a page a shared object.
 * 
 * @param   { PageManager * }   this            The page manager object.
 * @param   { char * }          sPageKey        The page we want to modify.
 * @param   { p_obj }           pSharedObject   The object we want to share to the page.
*/
void PageManager_givePage(PageManager *this, char *sPageKey, p_obj pSharedObject) {
  Page *pPage = HashMap_get(this->pPageMap, sPageKey);

  pPage->pSharedObject = pSharedObject;
}

#endif
