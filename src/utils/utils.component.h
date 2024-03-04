/**
 * @ Author: Mo David
 * @ Create Time: 2024-03-04 14:55:34
 * @ Modified time: 2024-03-04 15:33:40
 * @ Description:
 * 
 * This class defines a component which we append to the page class.
 * The page class stores a tree of components which are then rendered through a BFS traversal.
 */

#ifndef UTILS_COMPONENT_
#define UTILS_COMPONENT_

#include "./utils.buffer.h"

#define COMPONENT_MAX_CHILD_COUNT (1 << 4)
#define COMPONENT_MAX_ASSET_COUNT (1 << 4)

typedef struct Component Component;
typedef struct ComponentManager ComponentManager;

/**
 * A class that stores a UI component which we will eventually render to the screen.
 * 
 * @class
*/
struct Component {
  Component *pParent;                               // The parent component
  Component *pChildren[COMPONENT_MAX_CHILD_COUNT];  // The children components
  int dChildCount;                                  // How many children we currently have

  Buffer *pSharedBuffer;                            // The shared buffer to write to

  int x;                                            // The x-position of the component with respect to its parent
  int y;                                            // The y-position of the component with respect to its parent

  int w;                                            // The width of the component
  int h;                                            // The height of the component
  
  char **aAssetArray[COMPONENT_MAX_ASSET_COUNT];    // An array of assets to be printed by the component    
  int dAssetCount;                                  // How many assets we currently have
                                
  int dAssetXArray[COMPONENT_MAX_ASSET_COUNT];      // The x-positions of the assets
  int dAssetYArray[COMPONENT_MAX_ASSET_COUNT];      // The y-positions of the assets
  int dAssetWArray[COMPONENT_MAX_ASSET_COUNT];      // The width of the assets
  int dAssetHArray[COMPONENT_MAX_ASSET_COUNT];      // The height of the assets
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
 * @param		{ Component * }		this	          A pointer to the instance to initialize.
 * @param   { char * }        sName           An identifier for the component.
 * @param   { Component * }   pParent         The parent of the component. This can be NULL.
 * @param   { Buffer * }      pSharedBuffer   The buffer the component will write to.
 * @param   { int }           x               The x-coordinate of the component.
 * @param   { int }           y               The y-coordinate of the component.
 * @param   { int }           w               The width of the component.
 * @param   { int }           h               The height of the component.
 * @return	{ Component * }					          A pointer to the initialized instance.
*/
Component *Component_init(Component *this, char *sName, Component *pParent, Buffer *pSharedBuffer, int x, int y, int w, int h) {
  this->pParent = pParent;
  this->dChildCount = 0;
  
  this->pSharedBuffer = pSharedBuffer;

  this->x = x;
  this->y = y;
  this->w = w;
  this->h = h;

  return this;
}

/**
 * Creates an initialized instance of the Component class.
 * 
 * @param   { Component * }   pParent         The parent of the component. This can be NULL.
 * @param   { char * }        sName           An identifier for the component.
 * @param   { Buffer * }      pSharedBuffer   The buffer the component will write to.
 * @param   { int }           x               The x-coordinate of the component.
 * @param   { int }           y               The y-coordinate of the component.
 * @param   { int }           w               The width of the component.
 * @param   { int }           h               The height of the component.
 * @return	{ Component * }		                A pointer to the newly created initialized instance.
*/
Component *Component_create(char *sName, Component *pParent, Buffer *pSharedBuffer, int x, int y, int w, int h) {
  return Component_init(Component_new(), sName, pParent, pSharedBuffer, x, y, w, h);
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
 * 
 * @param   { Component * }   this      The component to modify.
 * @param   { Component * }   pChild    The component to append.
*/
void Component_add(Component *this, Component *pChild) {
  
  // Too many kids
  if(this->dChildCount >= COMPONENT_MAX_CHILD_COUNT)
    return;

  // Add to the array and set the parent of the child to the current component
  this->pChildren[this->dChildCount++] = pChild;
  pChild->pParent = this;
}

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

  Buffer *pSharedBuffer;    // The buffer which all components will share
  
  HashMap *pComponentMap;   // A hashmap with our components
  Queue *pRenderQueue;      // A queue we'll use for rendering
};

/**
 * Allocates memory for an instance of the ComponentManager class.
 * 
 * @return	{ ComponentManager * }		A pointer to the allocated memory.
*/
void ComponentManager_init(ComponentManager *this, Buffer *pSharedBuffer) {

  // Save the shared buffer
  this->pSharedBuffer = pSharedBuffer;

  // Create the root component with no parent and occupies the whole console
  this->pRoot = Component_create("root", NULL, 
    pSharedBuffer, 0, 0, 
    pSharedBuffer->dWidth, 
    pSharedBuffer->dHeight);

  // Init the queue and the hashmap
  this->pRenderQueue = Queue_create();
  this->pComponentMap = HashMap_create();

  // Add the root to the hashmap
  HashMap_add(this->pComponentMap, "root", this->pRoot);
}

/**
 * Initializes the component manager.
 * ! todo
 * @param		{ ComponentManager * }		this	A pointer to the instance to clean up.
*/
void ComponentManager_exit(ComponentManager *this) {

}

/**
 * Adds a component as a child of the specified component with the given id.
 * 
 * @param		{ ComponentManager * }		this        The component manager.
 * @param   { char * }                sParentKey  The key of the component to append to.
*/
void ComponentManager_add(ComponentManager *this, char *sParentKey, char *sKey) {
  
}


#endif
