/**
 * @ Author: MMMM
 * @ Create Time: 2024-03-02 15:52:53
 * @ Modified time: 2024-03-02 16:34:20
 * @ Description:
 * 
 * Defines an asset class and manager that lets us store assets in a modular manner.
 */

#ifndef UTILS_ASSET_
#define UTILS_ASSET_

#include "./utils.hashmap.h"

#include <string.h>

#define ASSET_MAX_COUNT (1 << 8)
#define ASSET_MAX_WIDTH (1 << 8)
#define ASSET_MAX_HEIGHT (1 << 6)

typedef struct Asset Asset;
typedef struct AssetManager AssetManager;

/**
 * //
 * ////
 * //////    Asset class
 * ////////
 * ////////// 
*/

/**
 * A class for storing 2d arrays of characters.
 * @class
*/
struct Asset {

  char *sName;                                                  // An identifier for the asset

  char cContentArray[ASSET_MAX_HEIGHT][ASSET_MAX_WIDTH << 2];   // The actual content of the asset

};


/**
 * Allocates memory for an instance of the Asset class.
 * 
 * @return	{ Asset * }		A pointer to the allocated memory.
*/
Asset *Asset_new() {
  Asset *pAsset = calloc(1, sizeof(*pAsset));
  return pAsset;
}

/**
 * Initializes an instance of the Asset class.
 * 
 * @param		{ Asset * }		this	          A pointer to the instance to initialize.
 * @param   { char * }    sName           The name of the asset
 * @param   { int }       h               How many lines are stored by the asset.
 * @param   { char ** }   cContentArray   Stores a 2d array of characters.
 * @return	{ Asset * }					          A pointer to the initialized instance.
*/
Asset *Asset_init(Asset *this, char *sName, int h, char **cContentArray) {
  int i, j;

  for(i = 0; i < h; i++)
    for(j = 0; j < strlen(cContentArray[i]); j++) 
      this->cContentArray[i][j] = cContentArray[i][j];

  return this;
}

/**
 * Creates an initialized instance of the Asset class.

 * @param   { char * }    sName           The name of the asset
 * @param   { int }       h               How many lines are stored by the asset.
 * @param   { char ** }   cContentArray   Stores a 2d array of characters.
 * @return	{ Asset * }		                A pointer to the newly created initialized instance.
*/
Asset *Asset_create(char *sName, int h, char **cContentArray) {
  return Asset_init(Asset_new(), sName, h, cContentArray);
}

/**
 * Deallocates the memory of an instance of the Asset class.
 * 
 * @param		{ Asset * }		this	A pointer to the instance to deallocate.
*/
void Asset_kill(Asset *this) {
  free(this);
}

/**
 * //
 * ////
 * //////    AssetManager struct
 * ////////
 * ////////// 
*/

/**
 * This struct manages the creation and storing of Asset instances.
 * 
 * @struct
*/
struct AssetManager {
  HashMap *pAssetMap;

  int dAssetCount;
};

/**
 * Initializes the asset manager.
*/
void AssetManager_init(AssetManager *this) {
  this->pAssetMap = HashMap_create();
  this->dAssetCount = 0;
}

/**
 * Cleans up after the asset manager.
*/
void AssetManager_exit(AssetManager *this) {
  int i;
  char *pAssetKeyArray[ASSET_MAX_COUNT];
  
  HashMap_getKeys(this->pAssetMap, pAssetKeyArray);

  // Delete all entries
  for(i = 0; i < this->dAssetCount; i++)
    HashMap_del(this->pAssetMap, pAssetKeyArray[i]);
}


#endif