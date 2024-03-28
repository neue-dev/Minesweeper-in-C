/**
 * @ Author: Mo David
 * @ Create Time: 2024-03-04 14:55:34
 * @ Modified time: 2024-03-28 10:26:12
 * @ Description:
 * 
 * This class defines a component which we append to the page class.
 * The page class stores a tree of components which are then rendered through a BFS traversal.
 */

#ifndef UTILS_COMPONENT_
#define UTILS_COMPONENT_

#include "./utils.string.h"
#include "./utils.buffer.h"
#include "./utils.queue.h"

// Some useful constants
#define COMPONENT_FAR_RIGHT 1024  // Components we dont wanna see
#define COMPONENT_FAR_BOTTOM 256
#define COMPONENT_FAR_LEFT -1024
#define COMPONENT_FAR_TOP -256

// Integer values we don't wanna change
#define COMPONENT_NO_CHANGE -99999

// Max number of children per component
#define COMPONENT_MAX_CHILD_COUNT (1 << 8)

typedef enum ComponentType ComponentType;
typedef enum ComponentAlignmentX ComponentAlignmentX;
typedef enum ComponentAlignmentY ComponentAlignmentY;
typedef enum ComponentAnchorX ComponentAnchorX;
typedef enum ComponentAnchorY ComponentAnchorY;

typedef struct Component Component;
typedef struct ComponentManager ComponentManager;

enum ComponentType {
  COMPONENT_SINGLE_ROW,
  COMPONENT_MULTI_ROW,
  COMPONENT_SINGLE_COL,
  COMPONENT_MULTI_COL,
  COMPONENT_ROW_COL,
  COMPONENT_COL_ROW,
  COMPONENT_FIXED,
};

enum ComponentAlignmentX {
  COMPONENT_CENTER_ALIGN_X,
  COMPONENT_LEFT_ALIGN_X,
  COMPONENT_RIGHT_ALIGN_X,
};

enum ComponentAlignmentY {
  COMPONENT_CENTER_ALIGN_Y,
  COMPONENT_TOP_ALIGN_Y,
  COMPONENT_BOTTOM_ALIGN_Y,
};

enum ComponentAnchorX {
  COMPONENT_CENTER_ANCHOR_X,
  COMPONENT_LEFT_ANCHOR_X,
  COMPONENT_RIGHT_ANCHOR_X,
};

enum ComponentAnchorY {
  COMPONENT_CENTER_ANCHOR_Y,
  COMPONENT_TOP_ANCHOR_Y,
  COMPONENT_BOTTOM_ANCHOR_Y,
};

/**
 * //
 * ////
 * //////    Component class
 * ////////
 * ////////// 
*/

/**
 * A class that stores a UI component which we will eventually render to the screen.
 * 
 * @class
*/
struct Component {
  char sName[STRING_KEY_MAX_LENGTH];                // The name of the component
                                                    // This is important so we can get its states in the page class

  int zIndex;                                       // Default is 0; helps with layering
  int bIsHidden;                                    // Whether or not the component is hidden

  Component *pParent;                               // The parent component
  Component *pChildren[COMPONENT_MAX_CHILD_COUNT];  // The children components
  int dChildCount;                                  // How many children we currently have
  int dRowLength;                                   // The cumulative length of the children along a row
  int dColLength;                                   // Cumulative length along a column  

  int x;                                            // The x-position of the component with respect to its parent
  int y;                                            // The y-position of the component with respect to its parent

  int w;                                            // The width of the component
  int h;                                            // The height of the component

  int dParentX;                                     // An x offset based on its parent
  int dParentY;                                     // A y offset based on its parent

  int dOffsetX;                                     // An x offset for the element, based on siblings
  int dOffsetY;                                     // A y offset for the element, based on siblings

  int dPaddingX;                                    // The padding is used to compute center, left and right aligns
  int dPaddingY;                                    // yes
  
  int dRenderX;                                     // The absolute x-coordinate where the component will actually be rendered
  int dRenderY;                                     // The absolute y-coordinate where the component will actually be rendered
  int dRenderW;                                     // The rendering width
  int dRenderH;                                     // The rendering height

  char **aAsset;                                    // An asset to be printed by the component    
  int dAssetHeight;                                 // The height of the asset

  color colorFG;                                    // A color for the foreground
  color colorBG;                                    // A color for the background

  ComponentType eComponentType;                     // Determines how the component renders its children
  ComponentAlignmentX eComponentAlignmentX;         // Determiens the alignment of its children along the horizontal
  ComponentAlignmentY eComponentAlignmentY;         // Determiens the alignment of its children along the vertical
  ComponentAnchorX eComponentAnchorX;               // How the component is anchored horizontally (where its pivot point is)
  ComponentAnchorY eComponentAnchorY;               // How the component is anchored vertically (where its pivot point is)
};

/**
 * Allocates memory for an instance of the Component class.
 * 
 * @return	{ Component * }		A pointer to the allocated memory.
*/
Component *Component_new() {
  Component *pComponent = calloc(1, sizeof(*pComponent));
  return pComponent;
}

/**
 * Initializes an instance of the Component class.
 * 
 * @param		{ Component * }		this	        A pointer to the instance to initialize.
 * @param   { char * }        sName         An identifier for the component.
 * @param   { Component * }   pParent       The parent of the component. This can be NULL.
 * @param   { int }           x             The x-coordinate of the component.
 * @param   { int }           y             The y-coordinate of the component.
 * @param   { int }           w             The width of the component.
 * @param   { int }           h             The height of the component.
 * @param   { int }           dAssetHeight  The height of the asset.
 * @param   { char ** }       aAsset        The asset to be rendered by the component.
 * @param   { color }         colorFG       A foreground color for the component.
 * @param   { color }         colorBG       A background color for the component.
 * @return	{ Component * }					        A pointer to the initialized instance.
*/
Component *Component_init(Component *this, char *sName, Component *pParent, int x, int y, int w, int h, int dAssetHeight, char **aAsset, color colorFG, color colorBG) {
  int i = 0, j = 0, dNameLength = strlen(sName);
  char sNameSubpart[STRING_KEY_MAX_LENGTH];
  
  // Copy the name string
  strcpy(this->sName, sName);

  // Set the z index and the hidden boolean
  this->zIndex = 0;
  this->bIsHidden = 0;

  // Default component type
  this->eComponentType = COMPONENT_FIXED;
  this->eComponentAlignmentX = COMPONENT_LEFT_ALIGN_X;
  this->eComponentAlignmentY = COMPONENT_TOP_ALIGN_Y;
  this->eComponentAnchorX = COMPONENT_LEFT_ANCHOR_X;
  this->eComponentAnchorY = COMPONENT_TOP_ANCHOR_Y;

  i = 0, j = 0;

  // Parse the name and deduce the type from there
  // Note that this is <= dNameLength because we need to check for when !sName[i] inside the loop
  while(i <= dNameLength) {

    // It's a new subpart of the name
    if(sName[i] == '.' || !sName[i]) {
      
      // Determine what kind of component it is
      if(!strcmp(sNameSubpart, "row")) this->eComponentType = COMPONENT_SINGLE_ROW;
      else if(!strcmp(sNameSubpart, "col")) this->eComponentType = COMPONENT_SINGLE_COL;
      else if(!strcmp(sNameSubpart, "multirow")) this->eComponentType = COMPONENT_MULTI_ROW;
      else if(!strcmp(sNameSubpart, "multicol")) this->eComponentType = COMPONENT_MULTI_COL;
      else if(!strcmp(sNameSubpart, "row-col")) this->eComponentType = COMPONENT_ROW_COL;
      else if(!strcmp(sNameSubpart, "col-row")) this->eComponentType = COMPONENT_COL_ROW;
      else if(!strcmp(sNameSubpart, "fixed")) this->eComponentType = COMPONENT_FIXED;

      // Determine the alignment of its children
      else if(!strcmp(sNameSubpart, "left-x")) this->eComponentAlignmentX = COMPONENT_LEFT_ALIGN_X;
      else if(!strcmp(sNameSubpart, "center-x")) this->eComponentAlignmentX = COMPONENT_CENTER_ALIGN_X;
      else if(!strcmp(sNameSubpart, "right-x")) this->eComponentAlignmentX = COMPONENT_RIGHT_ALIGN_X;
      else if(!strcmp(sNameSubpart, "top-y")) this->eComponentAlignmentY = COMPONENT_TOP_ALIGN_Y;
      else if(!strcmp(sNameSubpart, "center-y")) this->eComponentAlignmentY = COMPONENT_CENTER_ALIGN_Y;
      else if(!strcmp(sNameSubpart, "bottom-y")) this->eComponentAlignmentY = COMPONENT_BOTTOM_ALIGN_Y;

      // Determine the alignment of its own asset / position
      else if(!strcmp(sNameSubpart, "aleft-x")) this->eComponentAnchorX = COMPONENT_LEFT_ANCHOR_X;
      else if(!strcmp(sNameSubpart, "acenter-x")) this->eComponentAnchorX = COMPONENT_CENTER_ANCHOR_X;
      else if(!strcmp(sNameSubpart, "aright-x")) this->eComponentAnchorX = COMPONENT_RIGHT_ANCHOR_X;
      else if(!strcmp(sNameSubpart, "atop-y")) this->eComponentAnchorY = COMPONENT_TOP_ANCHOR_Y;
      else if(!strcmp(sNameSubpart, "acenter-y")) this->eComponentAnchorY = COMPONENT_CENTER_ANCHOR_Y;
      else if(!strcmp(sNameSubpart, "abottom-y")) this->eComponentAnchorY = COMPONENT_BOTTOM_ANCHOR_Y;

      // Clear the name component
      while(--j >= 0) 
        sNameSubpart[j] = 0;

      i++;
      j = 0;
    } else {
      
      // Copy the component here
      sNameSubpart[j] = sName[i++];
      sNameSubpart[j + 1] = 0;

      j++;
    }

  };
  
  this->pParent = pParent;
  this->dChildCount = 0;
  this->dRowLength = 0;
  this->dColLength = 0;

  this->x = x;
  this->y = y;
  this->w = w;
  this->h = h;

  this->dParentX = 0;
  this->dParentY = 0;

  this->dOffsetX = 0;
  this->dOffsetY = 0;

  this->dRenderX = 0;
  this->dRenderY = 0;
  this->dRenderW = 0;
  this->dRenderH = 0;

  this->dAssetHeight = dAssetHeight;
  this->aAsset = aAsset;

  this->colorFG = colorFG;
  this->colorBG = colorBG;

  return this;
}

/**
 * Creates an initialized instance of the Component class.
 * 
 * @param   { Component * }   pParent       The parent of the component. This can be NULL.
 * @param   { char * }        sName         An identifier for the component.
 * @param   { int }           x             The x-coordinate of the component.
 * @param   { int }           y             The y-coordinate of the component.
 * @param   { int }           w             The width of the component.
 * @param   { int }           h             The height of the component.
 * @param   { int }           dAssetHeight  The height of the asset.
 * @param   { char ** }       aAsset        The asset to be rendered by the component.
 * @param   { color }         colorFG       A foreground color for the component.
 * @param   { color }         colorBG       A background color for the component.
 * @return	{ Component * }		              A pointer to the newly created initialized instance.
*/
Component *Component_create(char *sName, Component *pParent, int x, int y, int w, int h, int dAssetHeight, char **aAsset, color colorFG, color colorBG) {
  return Component_init(Component_new(), sName, pParent, x, y, w, h, dAssetHeight, aAsset, colorFG, colorBG);
}

/**
 * Deallocates the memory of an instance of the Component class.
 * 
 * @param		{ Component * }		this	A pointer to the instance to deallocate.
*/
void Component_kill(Component *this) {
  free(this);
}

/**
 * Adds a component child to the children array.
 * If there are too many children, nothing happens.
 * The function returns 1 on success and 0 on failure.
 * 
 * @param   { Component * }   this      The component to modify.
 * @param   { Component * }   pChild    The component to append.
*/
int Component_add(Component *this, Component *pChild) {
  
  // Too many kids
  if(this->dChildCount >= COMPONENT_MAX_CHILD_COUNT)
    return 0;

  // Add to the array and set the parent of the child to the current component
  this->pChildren[this->dChildCount++] = pChild;
  pChild->pParent = this;

  // How to display its children
  switch(this->eComponentType) {
    
    case COMPONENT_MULTI_ROW:
    
      // A new row starts
      if(this->dChildCount > 1)
        if(this->pChildren[this->dChildCount - 2]->y != pChild->y)
          this->dRowLength = 0;

    case COMPONENT_SINGLE_ROW:

      // Get the cumulative length of these guys
      pChild->dOffsetX = this->dRowLength;
      this->dRowLength += pChild->w + pChild->x;

      // If it's just a container, expand it to fit its kids
      if(this->aAsset == NULL)
        this->dRenderW = this->dRowLength;

    break;

    case COMPONENT_MULTI_COL:
    
      // A new row starts
      if(this->dChildCount > 1)
        if(this->pChildren[this->dChildCount - 2]->x != pChild->x)
          this->dColLength = 0;

    case COMPONENT_SINGLE_COL:

      // Get the cumulative length of these guys
      pChild->dOffsetY = this->dColLength;
      this->dColLength += pChild->h + pChild->y;

      // If it's just a container, expand it to fit its kids
      if(this->aAsset == NULL)
        this->dRenderH = this->dColLength;

    break;

    case COMPONENT_FIXED:

    break;

    case COMPONENT_ROW_COL:

      // Get the cumulative length along x
      pChild->dOffsetX = this->dRowLength;
      this->dRowLength += pChild->w + pChild->x;

      // Get the cumulative length along y
      this->dColLength += pChild->h + pChild->y;

      // If it's just a container, expand it to fit its kids
      if(this->aAsset == NULL) {
        this->dRenderW = this->dRowLength;
        this->dRenderH = this->dColLength;
      }
    break;

    case COMPONENT_COL_ROW:

      // Get the cumulative length along x
      this->dRowLength += pChild->w + pChild->x;

      // Get the cumulative length along y
      pChild->dOffsetY = this->dColLength;
      this->dColLength += pChild->h + pChild->y;

      // If it's just a container, expand it to fit its kids
      if(this->aAsset == NULL) {
        this->dRenderW = this->dRowLength;
        this->dRenderH = this->dColLength;
      }
    break;
  }

  return 1;
}

/**
 * Computes the position of the component based on parent components.
*/
void Component_config(Component *this) {
  int i;
  int dPaddingX = 0, dPaddingY = 0;
  int w = this->dRenderW ? this->dRenderW : this->w;
  int h = this->dRenderH ? this->dRenderH : this->h;

  this->dParentX = this->pParent->dRenderX;
  this->dParentY = this->pParent->dRenderY;

  this->dRenderX = this->dParentX + this->x + this->dPaddingX + this->dOffsetX;
  this->dRenderY = this->dParentY + this->y + this->dPaddingY + this->dOffsetY;

  // How to align its children along the horizontal
  switch(this->eComponentAlignmentX) {

    case COMPONENT_CENTER_ALIGN_X:
      if(this->eComponentType != COMPONENT_MULTI_ROW)
        dPaddingX = w / 2 - this->dRowLength / 2;
    break;

    case COMPONENT_LEFT_ALIGN_X:

    break;

    case COMPONENT_RIGHT_ALIGN_X:
      dPaddingX = w - this->dRowLength;
    break;
  }

  // Alignment of children along the vertical
  switch(this->eComponentAlignmentY) {

    case COMPONENT_CENTER_ALIGN_Y:
      if(this->eComponentType != COMPONENT_MULTI_COL)
        dPaddingY = h / 2 - this->dColLength / 2;
    break;

    case COMPONENT_TOP_ALIGN_Y:

    break;

    case COMPONENT_BOTTOM_ALIGN_Y:
      dPaddingY = h - this->dColLength;
    break;
  }

  // Self-alignment along horizontal
  switch(this->eComponentAnchorX) {
    
    case COMPONENT_CENTER_ANCHOR_X:
      this->dRenderX -= w / 2;
    break;

    case COMPONENT_LEFT_ANCHOR_X:

    break;

    case COMPONENT_RIGHT_ANCHOR_X:
      this->dRenderX -= w;
    break;
  }

  // Self alignment along vertical
  switch(this->eComponentAnchorY) {
    
    case COMPONENT_CENTER_ANCHOR_Y:
      this->dRenderY -= h / 2;
    break;

    case COMPONENT_TOP_ANCHOR_Y:

    break;

    case COMPONENT_BOTTOM_ANCHOR_Y:
      this->dRenderY -= h;
    break;
  }

  // Update the paddings
  for(i = 0; i < this->dChildCount; i++) {
    this->pChildren[i]->dPaddingX = dPaddingX;
    this->pChildren[i]->dPaddingY = dPaddingY;
  }
}

/**
 * //
 * ////
 * //////    ComponentManager struct
 * ////////
 * ////////// 
*/

/**
 * A struct that stores a tree and hashmap of components.
 * The tree helps us render the components later on.
 * The hashmap allows us to append components much more easily.
 * Note that this struct automatically creates a root node to which all other nodes
 *    may be appended.
 * 
 * @struct
*/
struct ComponentManager {
  Component *pRoot;         // The root node
  
  HashMap *pComponentMap;   // A hashmap with our components
  Queue *pRenderQueue;      // A queue we'll use for rendering
};

/**
 * Allocates memory for an instance of the ComponentManager class.
 * 
 * @return	{ ComponentManager * }		A pointer to the allocated memory.
*/
void ComponentManager_init(ComponentManager *this) {

  // Create the root component with no parent and occupies the whole console
  this->pRoot = Component_create("root", NULL, 0, 0, 0, 0, 0, NULL, -1, -1);

  // Init the queue and the hashmap
  this->pRenderQueue = Queue_create();
  this->pComponentMap = HashMap_create();

  // Add the root to the hashmap
  HashMap_add(this->pComponentMap, "root", this->pRoot);
}

/**
 * Exits the component manager.
 * 
 * @param		{ ComponentManager * }		this	A pointer to the instance to clean up.
*/
void ComponentManager_exit(ComponentManager *this) {
  Queue_kill(this->pRenderQueue);
  HashMap_kill(this->pComponentMap);
}

/**
 * Adds a component as a child of the specified component with the given id.
 * 
 * @param		{ ComponentManager * }		this          The component manager.
 * @param   { char * }                sName         An identifier for the component.
 * @param   { char * }                sParentKey    The key of the component to append to.
 * @param   { int }                   x             The x-coordinate of the component.
 * @param   { int }                   y             The y-coordinate of the component.
 * @param   { int }                   w             The width of the component.
 * @param   { int }                   h             The height of the component.
 * @param   { int }                   dAssetHeight  The height of the asset. This can be 0.
 * @param   { char ** }               aAsset        The asset to be rendered by the component. This may be NULL.
 * @param   { color }                 colorFG       A foreground color for the component.
 * @param   { color }                 colorBG       A background color for the component.
 * @return  { int }                                 Whether or not the component was added successfully.
*/
int ComponentManager_add(ComponentManager *this, char *sKey, char *sParentKey, int x, int y, int w, int h, int dAssetHeight, char **aAsset, color colorFG, color colorBG) {
  Component *pParent =  NULL;
  Component *pChild = NULL;

  if(sParentKey != NULL)
    pParent = HashMap_get(this->pComponentMap, sParentKey);

  // The parent doesn't exist
  if(pParent == NULL && sParentKey != NULL)
    return 0;

  // We have a duplicate component
  if(HashMap_get(this->pComponentMap, sKey) != NULL)
    return 0;

  // Create the child
  pChild = Component_create(sKey, pParent, x, y, w, h, dAssetHeight, aAsset, colorFG, colorBG);

  // Couldn't add the child because too many children
  if(!Component_add(pParent, pChild))
    Component_kill(pChild);

  // Otherwise, append it to the hashmap too
  HashMap_add(this->pComponentMap, sKey, pChild);

  return 1;
}

/**
 * Set the position of a specified component.
 * 
 * @param		{ ComponentManager * }		this          The component manager.
 * @param   { char * }                sKey          An identifier for the component.
 * @param   { int }                   x             The x-coordinate of the component.
 * @param   { int }                   y             The y-coordinate of the component.
*/
void ComponentManager_setPos(ComponentManager *this, char *sKey, int x, int y) {
  Component *pComponent = HashMap_get(this->pComponentMap, sKey);

  if(x != COMPONENT_NO_CHANGE)
    pComponent->x = x;

  if(y != COMPONENT_NO_CHANGE)
    pComponent->y = y;
}

/**
 * Set the size of a specified component.
 * 
 * @param		{ ComponentManager * }		this          The component manager.
 * @param   { char * }                sKey          An identifier for the component.
 * @param   { int }                   w             The width of the component.
 * @param   { int }                   h             The height of the component.
*/
void ComponentManager_setSize(ComponentManager *this, char *sKey, int w, int h) {
  Component *pComponent = HashMap_get(this->pComponentMap, sKey);

  if(w != COMPONENT_NO_CHANGE)
    pComponent->w = w;

  if(h != COMPONENT_NO_CHANGE)
    pComponent->h = h;
}

/**
 * Set the z index of a specified component.
 * 
 * @param		{ ComponentManager * }		this          The component manager.
 * @param   { char * }                sKey          An identifier for the component.
 * @param   { int }                   zIndex        The new zIndex.
*/
void ComponentManager_setZIndex(ComponentManager *this, char *sKey, int zIndex) {
  Component *pComponent = HashMap_get(this->pComponentMap, sKey);

  pComponent->zIndex = zIndex;
}

/**
 * Sets the visibility of a component.
 * 1 means hidden, 0 means visible.
 * 
 * @param		{ ComponentManager * }		this          The component manager.
 * @param   { char * }                sKey          An identifier for the component.
 * @param   { int }                   bIsHidden     The new value of bIsHidden.
*/
void ComponentManager_setHidden(ComponentManager *this, char *sKey, int bIsHidden) {
  Component *pComponent = HashMap_get(this->pComponentMap, sKey);

  pComponent->bIsHidden = bIsHidden;
}

/**
 * Set the color of a specified component.
 * 
 * @param		{ ComponentManager * }		this          The component manager.
 * @param   { char * }                sKey          An identifier for the component.
 * @param   { color }                 colorFG       The foreground color of the component.
 * @param   { color }                 colorBG       The background color of the component.
*/
void ComponentManager_setColor(ComponentManager *this, char *sKey, color colorFG, color colorBG) {
  Component *pComponent = HashMap_get(this->pComponentMap, sKey);

  if(colorFG != COMPONENT_NO_CHANGE)
    pComponent->colorFG = colorFG;

  if(colorBG != COMPONENT_NO_CHANGE)
    pComponent->colorBG = colorBG;
}

/**
 * Set the asset of the specified component.
 * 
 * @param		{ ComponentManager * }		this          The component manager.
 * @param   { char * }                sKey          An identifier for the component.
 * @param   { int }                   dAssetHeight  The height of the provided asset.
 * @param   { char ** }               aAsset        The actual information stored by the asset.
*/
void ComponentManager_setAsset(ComponentManager *this, char *sKey, int dAssetHeight, char **aAsset) {
  Component *pComponent = HashMap_get(this->pComponentMap, sKey);

  // Remove the original asset first
  free(pComponent->aAsset);

  // Store the new asset and its height
  pComponent->aAsset = aAsset;
  pComponent->dAssetHeight = dAssetHeight;
}

/**
 * Renders the components in the tree to the specified buffer.
 * 
 * @param   { ComponentManager * }  this      The component manager.
 * @param   { Buffer * }            pBuffer   The buffer to render components to.
*/
void ComponentManager_render(ComponentManager *this, Buffer *pBuffer) {
  int i, z = 0;
  Component *pComponent = NULL;
  Component *pChildComponent;

  // Initialize the queue with just the root component
  Queue_push(this->pRenderQueue, this->pRoot);

  // Prepare the buffer
  pBuffer = Buffer_create(
    IO_getWidth(), 
    IO_getHeight(), 
    0x000000,
    0x000000);

  // While we have components
  while(Queue_getHead(this->pRenderQueue) != NULL) {
    
    // Get the head component first
    pComponent = Queue_getHead(this->pRenderQueue);

    // If z index is greater than current
    if(pComponent->zIndex > z) {
      Queue_pop(this->pRenderQueue);
      Queue_push(this->pRenderQueue, pComponent);
    
      // Store the new z index
      // Note that this solution fails when two adjacent components have the same z index greater than 0
      // Thus, the program restricts that any z index > 0 must be unique
      z = pComponent->zIndex;

    // Otherwise, check if component is hidden
    } else if(pComponent->bIsHidden) {
      Queue_pop(this->pRenderQueue);

    // If not, render it
    } else {

      // Add its children to the render queue
      for(i = 0; i < pComponent->dChildCount; i++) {
        pChildComponent = pComponent->pChildren[i];

        // Compute its position based on parent offsets
        Component_config(pChildComponent);

        // Push the child to the queue
        Queue_push(this->pRenderQueue, pChildComponent);
      }

      // If the component has colors
      if(pComponent->colorFG > COMPONENT_NO_CHANGE || pComponent->colorBG > COMPONENT_NO_CHANGE) {
        Buffer_contextRect(
          pBuffer, 
          pComponent->dRenderX, 
          pComponent->dRenderY, 
          pComponent->dRenderW ? pComponent->dRenderW : pComponent->w, 
          pComponent->dRenderH ? pComponent->dRenderH : pComponent->h, 
          pComponent->colorFG, 
          pComponent->colorBG);
      }

      // If the component has an asset
      if(pComponent->aAsset != NULL) {
        Buffer_write(
          pBuffer, 
          pComponent->dRenderX, 
          pComponent->dRenderY, 
          pComponent->dAssetHeight, 
          pComponent->aAsset);
      }

      // Store the new z index
      z = pComponent->zIndex;

      // Remove the head component
      Queue_pop(this->pRenderQueue);

    }
  }

  // Reset the cursor home position
  IO_resetCursor();
  
  // Print the buffer
  Buffer_print(pBuffer);
  Buffer_kill(pBuffer);
}

#endif
