/**
 * @ Author: Mo David
 * @ Create Time: 2024-03-04 14:55:34
 * @ Modified time: 2024-03-04 23:12:24
 * @ Description:
 * 
 * This class defines a component which we append to the page class.
 * The page class stores a tree of components which are then rendered through a BFS traversal.
 */

#ifndef UTILS_COMPONENT_
#define UTILS_COMPONENT_

#define COMPONENT_MAX_CHILD_COUNT (1 << 8)
#define COMPONENT_MAX_ASSET_COUNT (1 << 4)

typedef struct Component Component;
typedef struct ComponentManager ComponentManager;

/**
 * A class that stores a UI component which we will eventually render to the screen.
 * 
 * @class
*/
struct Component {
  char *sName;                                      // The name of the component
                                                    // This is important so we can get its states in the page class

  Component *pParent;                               // The parent component
  Component *pChildren[COMPONENT_MAX_CHILD_COUNT];  // The children components
  int dChildCount;                                  // How many children we currently have

  int x;                                            // The x-position of the component with respect to its parent
  int y;                                            // The y-position of the component with respect to its parent

  int w;                                            // The width of the component
  int h;                                            // The height of the component

  int dParentX;                                     // An x offset based on its parent
  int dParentY;                                     // A y offset based on its parent
  
  int dRenderX;                                     // The absolute x-coordinate where the component will actually be rendered
  int dRenderY;                                     // The absolute y-coordinate where the component will actually be rendered

  char **aAsset;                                    // An asset to be printed by the component    
  int dAssetHeight;                                 // The height of the asset

  int colorFG;                                      // A color for the foreground
  int colorBG;                                      // A color for the background
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
 * @param   { int }                   colorFG       A foreground color for the component.
 * @param   { int }                   colorBG       A background color for the component.
 * @return	{ Component * }					        A pointer to the initialized instance.
*/
Component *Component_init(Component *this, char *sName, Component *pParent, int x, int y, int w, int h, int dAssetHeight, char **aAsset, int colorFG, int colorBG) {
  this->sName = sName;
  
  this->pParent = pParent;
  this->dChildCount = 0;

  this->x = x;
  this->y = y;
  this->w = w;
  this->h = h;

  this->dParentX = 0;
  this->dParentY = 0;

  this->dRenderX = 0;
  this->dRenderY = 0;

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
 * @param   { int }           colorFG       A foreground color for the component.
 * @param   { int }           colorBG       A background color for the component.
 * @return	{ Component * }		              A pointer to the newly created initialized instance.
*/
Component *Component_create(char *sName, Component *pParent, int x, int y, int w, int h, int dAssetHeight, char **aAsset, int colorFG, int colorBG) {
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

  return 1;
}

/**
 * Computes the position of the component based on parent components.
*/
void Component_config(Component *this) {
  this->dParentX = this->pParent->dParentX + this->pParent->x;
  this->dParentY = this->pParent->dParentY + this->pParent->y;

  this->dRenderX = this->dParentX + this->x;
  this->dRenderY = this->dParentY + this->y;
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
 * Initializes the component manager.
 * ! todo
 * @param		{ ComponentManager * }		this	A pointer to the instance to clean up.
*/
void ComponentManager_exit(ComponentManager *this) {

}

/**
 * Adds a component as a child of the specified component with the given id.
 * 
 * @param		{ ComponentManager * }		this          The component manager.
 * @param   { char * }                sParentKey    The key of the component to append to.
 * @param   { char * }                sName         An identifier for the component.
 * @param   { int }                   x             The x-coordinate of the component.
 * @param   { int }                   y             The y-coordinate of the component.
 * @param   { int }                   w             The width of the component.
 * @param   { int }                   h             The height of the component.
 * @param   { int }                   dAssetHeight  The height of the asset. This can be 0.
 * @param   { char ** }               aAsset        The asset to be rendered by the component. This may be NULL.
 * @param   { int }                   colorFG       A foreground color for the component.
 * @param   { int }                   colorBG       A background color for the component.
*/
void ComponentManager_add(ComponentManager *this, char *sParentKey, char *sKey, int x, int y, int w, int h, int dAssetHeight, char **aAsset, int colorFG, int colorBG) {
  Component *pParent = (Component *) HashMap_get(this->pComponentMap, sParentKey);
  Component *pChild;

  // The parent doesn't exist
  if(pParent == NULL)
    return;

  // Create the child
  pChild = Component_create(sKey, pParent, x, y, w, h, dAssetHeight, aAsset, colorFG, colorBG);

  // Couldn't add the child because too many children
  if(!Component_add(pParent, pChild))
    Component_kill(pChild);

  // Otherwise, append it to the hashmap too
  HashMap_add(this->pComponentMap, sKey, pChild);
}

#endif
