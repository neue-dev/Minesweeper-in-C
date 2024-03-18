/**
 * @ Author: MMMM
 * @ Create Time: 2024-03-02 15:52:53
 * @ Modified time: 2024-03-13 15:22:03
 * @ Description:
 * 
 * Defines an asset class and manager that lets us store assets in a modular manner.
 */

#ifndef UTILS_ASSET_
#define UTILS_ASSET_

#include "./utils.file.h"
#include "./utils.string.h"
#include "./utils.hashmap.h"

#include <string.h>

#define ASSET_MAX_COUNT (1 << 8)
#define ASSET_MAX_WIDTH (1 << 8)
#define ASSET_MAX_HEIGHT (1 << 6)
#define ASSET_FILE_MAX_LEN (1 << 12)

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
  char sName[STRING_KEY_MAX_LENGTH];      // An identifier for the asset

  int dHeight;                            // Stores the height of the text content
  int dWidth;                             // Stores the default width of the text content (the width of the first line)

  char *sContentArray[ASSET_MAX_HEIGHT];  // The actual content of the asset
                                          // We prefer to have this as a dynamically-allocated array because its easier to return
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
 * @param		{ Asset * }		this	            A pointer to the instance to initialize.
 * @param   { char * }    sName             The name of the asset
 * @param   { int }       dHeight           How many lines are stored by the asset.
 * @param   { char * }    sContentArray[]   Stores a 2d array of characters.
 * @return	{ Asset * }					            A pointer to the initialized instance.
*/
Asset *Asset_init(Asset *this, char *sName, int dHeight, char *sContentArray[]) {
  int i;

  strcpy(this->sName, sName);

  // Stroe the height
  this->dHeight = dHeight;
  this->dWidth = String_charCount(sContentArray[0]);
  
  // Store the content
  for(i = 0; i < dHeight; i++) {
    this->sContentArray[i] = String_alloc(ASSET_MAX_WIDTH);
    
    strcpy(this->sContentArray[i], sContentArray[i]);
  }

  return this;
}

/**
 * Creates an initialized instance of the Asset class.

 * @param   { char * }    sName             The name of the asset
 * @param   { int }       h                 How many lines are stored by the asset.
 * @param   { char * }    sContentArray[]   Stores a 2d array of characters.
 * @return	{ Asset * }		                  A pointer to the newly created initialized instance.
*/
Asset *Asset_create(char *sName, int h, char *sContentArray[]) {
  return Asset_init(Asset_new(), sName, h, sContentArray);
}

/**
 * Deallocates the memory of an instance of the Asset class.
 * 
 * @param		{ Asset * }		this	A pointer to the instance to deallocate.
*/
void Asset_kill(Asset *this) {
  int i;
  
  for(i = 0; i < this->dHeight; i++)
    String_kill(this->sContentArray[i]);
    
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
 * 
 * @param   { AssetManager * }  this  The AssetManager struct.
*/
void AssetManager_init(AssetManager *this) {
  this->pAssetMap = HashMap_create();
  this->dAssetCount = 0;
}

/**
 * Cleans up after the asset manager.
 *
 * @param   { AssetManager * }  this  The AssetManager struct.
*/
void AssetManager_exit(AssetManager *this) {
  HashMap_kill(this->pAssetMap);
}

/**
 * Creates a new asset which is then appended to its hash map.
 * 
 * @param   { AssetManager * }  this              The AssetManager struct.
 * @param   { char * }          sAssetKey         The name of the asset to create.
 * @param   { int }             h                 The height of the asset to create.
 * @param   { char * }          sContentArray[]   The content of the asset to create.
*/
void AssetManager_createAsset(AssetManager *this, char *sAssetKey, int h, char *sContentArray[]) {
  Asset *pAsset;

  // Too many assets
  if(this->dAssetCount >= ASSET_MAX_COUNT)
    return;

  // We have a duplicate
  if(HashMap_get(this->pAssetMap, sAssetKey) != NULL)
    return;

  // Create asset then append to map
  pAsset = Asset_create(sAssetKey, h, sContentArray);
  HashMap_add(this->pAssetMap, sAssetKey, pAsset);

  // Increment asset count
  this->dAssetCount++;
}

/**
 * Retrieves an asset from its database of assets.
 * It returns the content of that asset as opposed to a pointer to the asset object.
 * 
 * @param   { AssetManager * }  this            The AssetManager struct.
 * @param   { char * }          sAssetKey       The name of the asset to create.
 * @return  { char ** }                         A pointer to the 2d text array we're gonna use.
*/
char **AssetManager_getAssetText(AssetManager *this, char *sAssetKey) {
  return ((Asset *) HashMap_get(this->pAssetMap, sAssetKey))->sContentArray;
}

/**
 * Retrieves an asset from its database of assets.
 * It returns the height of the asset.
 * 
 * @param   { AssetManager * }  this            The AssetManager struct.
 * @param   { char * }          sAssetKey       The name of the asset to create.
 * @return  { int }                             The number of lines in the asset.
*/
int AssetManager_getAssetHeight(AssetManager *this, char *sAssetKey) {
  return ((Asset *) HashMap_get(this->pAssetMap, sAssetKey))->dHeight;
}

/**
 * Retrieves an asset from its database of assets.
 * It returns the default width of the asset.
 * 
 * @param   { AssetManager * }  this            The AssetManager struct.
 * @param   { char * }          sAssetKey       The name of the asset to create.
 * @return  { int }                             The number of characters on the first line of the asset.
*/
int AssetManager_getAssetWidth(AssetManager *this, char *sAssetKey) {
  return ((Asset *) HashMap_get(this->pAssetMap, sAssetKey))->dWidth;
}

/**
 * Creates a new asset which is then appended to its hash map.
 * This asset is a text asset drawn with the font provided.
 * The provided font just refers to a set of characters drawn in Unicode.
 * Note that the key is generated from both the text and the font. It is
 *    written as <sText>-<sFont>.
 * 
 * @param   { AssetManager * }  this              The AssetManager struct.
 * @param   { int }             sText             The text to convert into an asset.
 * @param   { char * }          sFont             The font we're going to use.
*/
void AssetManager_createTextAsset(AssetManager *this, char *sText, char *sFont) {
  
  Asset *pAsset;
  int i, dGlyphAssetHeight = 0;
  char **pGlyphText;
  char sAssetKey[STRING_KEY_MAX_LENGTH];
  char sGlyphKey[STRING_KEY_MAX_LENGTH];
  char *sContentArray[ASSET_MAX_HEIGHT];

  // Too many assets
  if(this->dAssetCount >= ASSET_MAX_COUNT)
    return;

  // Create the asset key
  String_keyAndStr(sAssetKey, sFont, sText);

  // We have a duplicate
  if(HashMap_get(this->pAssetMap, sAssetKey) != NULL)
    return;

  // For each character, we add their string contents to the content array
  while(*sText) {

    // Create the key
    String_keyAndChar(sGlyphKey, sFont, *sText);

    // Only if we haven't set the glyph asset height, we init the array
    if(!dGlyphAssetHeight) {
      dGlyphAssetHeight = AssetManager_getAssetHeight(this, sGlyphKey);

      for(i = 0; i < dGlyphAssetHeight; i++)
        sContentArray[i] = String_alloc(ASSET_MAX_WIDTH);
    }

    // Get the asset for the letter
    pGlyphText = AssetManager_getAssetText(this, sGlyphKey);

    // Copy the letter asset unto the content array
    for(i = 0; i < dGlyphAssetHeight; i++)
      strcat(sContentArray[i], pGlyphText[i]);

    // Increment pointer
    sText++;
  }

  // Create asset then append to map
  pAsset = Asset_create(sAssetKey, dGlyphAssetHeight, sContentArray);
  HashMap_add(this->pAssetMap, sAssetKey, pAsset);

  // Increment asset count
  this->dAssetCount++;

  // Kill the allocations
  for(i = 0; i < dGlyphAssetHeight; i++)
    String_kill(sContentArray[i]);
}

/**
 * Reads sprites from an asset text file and converts them into instances of the asset class above.
 * Honestly, it's okay if this file takes a while to execute because this is only run once and not
 *    during each frame anyway.
 * 
 * @param   { AssetManager * }  this        The AssetManager struct.
 * @param   { char * }          sDelimeter  What character sequence determines the separation of 
 *                                          different assets on the text file.
 * @param   { char * }          sFilepath   The path to the file we wish to read assets from.
*/
void AssetManager_readAssetFile(AssetManager *this, char *sDelimeter, char *sFilepath) {
  int i = 0, j = 0, k = 0;
  int dDelimeterLength = strlen(sDelimeter);
  int bIsComment = 0;
  
  // Stores a name for an asset
  char sName[STRING_KEY_MAX_LENGTH];

  // Stores the output of the file and the file itself
  int dAssetFileBufferLength = 0;
  char *sAssetFileBuffer[ASSET_FILE_MAX_LEN];
  File *pAssetFile = File_create(sFilepath);

  // The contents of a single asset instance and the asset itself
  int dAssetInstanceBufferLength = 0;
  char *sAssetInstanceBuffer[ASSET_MAX_HEIGHT];

  // Initialize the name buffer
  for(i = 0; i < STRING_KEY_MAX_LENGTH; i++)
    sName[i] = 0;

  // Initialize the asset instance buffer
  for(i = 0; i < ASSET_MAX_HEIGHT; i++)
    sAssetInstanceBuffer[i] = String_alloc(ASSET_MAX_WIDTH);

  // Read the file unto the buffer
  File_readText(pAssetFile, ASSET_FILE_MAX_LEN, 
    &dAssetFileBufferLength,
    sAssetFileBuffer);

  // Reset counter
  i = 0;
  
  // Process the file
  while(i < dAssetFileBufferLength) {
    
    // Always assume it's a comment
    bIsComment = 1;

    // Check if it isn't a comment
    k = strlen(sAssetFileBuffer[i]);
    for(j = 0; j < dDelimeterLength && j < k; j++)
      if(sDelimeter[j] != sAssetFileBuffer[i][j])
        bIsComment = 0;

    // If a comment
    if(bIsComment) {

      // It's the end of an asset sprite
      // Note the +2: THIS TOOK SO LONG TO DEBUG but apparently newlines on Windows are stored as /r/n (but they're)
      //    read as one and on Linux theyre only stored as /n so when migrating to Linux things broke when having just + 1
      //    (all the new lines became an explicit /r/n)
      if(strlen(sAssetFileBuffer[i]) <= dDelimeterLength + 2) {
        
        // Create the asset
        AssetManager_createAsset(this, sName, dAssetInstanceBufferLength, sAssetInstanceBuffer);

      // If it's the start of a new asset sprite
      } else {

        // Max name length is 256 bytes
        // We create a new string each time because we pass it to the asset instance anw
        k = 0;
        while(sName[k]) sName[k++] = 0;
        dAssetInstanceBufferLength = 0;
        
        // Read the name from the comment
        // This is also a contribution to the Linux bug: again new lines here explicity have the /r
        //    because the text file was saved on Windows
        k = 0;
        while( 
          sAssetFileBuffer[i][j] != '\n' && 
          sAssetFileBuffer[i][j] != '\r')
          sName[k++] = sAssetFileBuffer[i][j++];

        // Reset the asset instance buffer
        for(k = 0; k < ASSET_MAX_HEIGHT; k++)
          memset(sAssetInstanceBuffer[k], 0, ASSET_MAX_WIDTH);
      }

    // We are currently reading the lines of a sprite
    } else {
      j = strlen(sAssetFileBuffer[i]);

      // Copy the contents of the file
      strcpy(sAssetInstanceBuffer[dAssetInstanceBufferLength], sAssetFileBuffer[i]);

      while(
        sAssetInstanceBuffer[dAssetInstanceBufferLength][j - 1] == '\n' ||
        sAssetInstanceBuffer[dAssetInstanceBufferLength][j - 1] == '\r')
        sAssetInstanceBuffer[dAssetInstanceBufferLength][j-- - 1] = 0;

      // Increment the length of the instance buffer
      dAssetInstanceBufferLength++;
    }

    // Parse the next line
    i++;
  }

  // Clean up 
  for(i = 0; i < ASSET_MAX_HEIGHT; i++)
    String_kill(sAssetInstanceBuffer[i]);

  // Garbage collection
  if(pAssetFile != NULL)
    File_kill(pAssetFile);
}


#endif