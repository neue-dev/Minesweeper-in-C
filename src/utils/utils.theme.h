/**
 * @ Author: MMMM
 * @ Create Time: 2024-03-08 09:36:02
 * @ Modified time: 2024-03-29 12:10:00
 * @ Description:
 * 
 * A class for handling themes so changing colors individially doesnt end up becoming a pain in the ass.
 */

#ifndef UTILS_THEME_
#define UTILS_THEME_

#include "./utils.file.h"
#include "./utils.hashmap.h"
#include "./utils.graphics.h"
#include "./utils.types.h"

#include <stdlib.h>

#define THEME_FILE_MAX_LEN (1 << 12)

typedef struct Theme Theme;
typedef struct ThemeManager ThemeManager;

/**
 * //
 * ////
 * //////    Theme class
 * ////////
 * ////////// 
*/

/**
 * An instantiable class that holds references to different colors.
 * A theme usually consists of five main colors.
*/
struct Theme {
  HashMap *pColorMap;  // A mapping between color keys and color values
  int dColors;        // How many colors we have
};

/**
 * Allocates memory for an instance of the Theme class.
 * 
 * @return	{ Theme * }		A pointer to the allocated memory.
*/
Theme *Theme_new() {
  Theme *pTheme = calloc(1, sizeof(*pTheme));
  return pTheme;
}

/**
 * Initializes an instance of the Theme class.
 * 
 * @param		{ Theme * }		this	A pointer to the instance to initialize.
 * @return	{ Theme * }					A pointer to the initialized instance.
*/
Theme *Theme_init(Theme *this) {

  this->pColorMap = HashMap_create();
  this->dColors = 0;

  return this;
}

/**
 * Creates an initialized instance of the Theme class.
 * 
 * @return	{ Theme * }		A pointer to the newly created initialized instance.
*/
Theme *Theme_create() {
  return Theme_init(Theme_new());
}

/**
 * Deallocates the memory of an instance of the Theme class.
 * 
 * @param		{ Theme * }		this	A pointer to the instance to deallocate.
*/
void Theme_kill(Theme *this) {
  HashMap_kill(this->pColorMap);
  free(this);
}

/**
 * Adds a new color to the theme given the provided key.
 * 
 * @param   { Theme * }   this  The theme to modify.
 * @param   { char * }    sKey  The key of the theme to modify.
 * @param   { color }     c     The color we want to add to the theme.
*/
void Theme_addColor(Theme* this, char *sKey, color c) {
  
  // The color has already been added
  if(HashMap_get(this->pColorMap, sKey) != NULL)
    return;

  // Create a reference to the specified color
  color *pColor = calloc(1, sizeof(color));
  *pColor = c;

  // Store the reference in the hashmap
  HashMap_add(this->pColorMap, sKey, pColor);

  // Increment count
  this->dColors++;
}

/**
 * Gets a darker version of a specific color from the current theme.
 * The degree of darkening is also provided, where
 *    0.0 represents no darkening at all.
 *    0.5 is a 50-50 combination of the selected color and black.
 *    1.0 represnts total black.
 * 
 * @param   { Theme * }   this      The theme to modify.
 * @param   { char * }    sKey      The key of the theme to modify.
 * @param   { float }     fAmount   How much to lerp between the color and pure black.
 * @return  { color }               The resulting lerped color.
*/
color Theme_getDarker(Theme *this, char *sKey, float fAmount) {
  color *pColor = HashMap_get(this->pColorMap, sKey);

  // Color does not exist
  if(pColor == NULL)
    return -1;

  // Return the lerped color
  return Graphics_lerp(*pColor, 0x000000, fAmount);
}

/**
 * Gets a lighter version of a specific color from the current theme.
 * The degree of lightening is also provided, where
 *    0.0 represents no lightening at all.
 *    0.5 is a 50-50 combination of the selected color and white.
 *    1.0 represnts total white.
 * 
 * @param   { Theme * }   this      The theme to modify.
 * @param   { char * }    sKey      The key of the theme to modify.
 * @param   { float }     fAmount   How much to lerp between the color and pure white.
 * @return  { color }               The resulting lerped color.
*/
color Theme_getLighter(Theme *this, char *sKey, float fAmount) {
  color *pColor = HashMap_get(this->pColorMap, sKey);

  // Color does not exist
  if(pColor == NULL)
    return -1;

  // Return the lerped color
  return Graphics_lerp(*pColor, 0xffffff, fAmount);
}

/**
 * Simply returns the color referred to by the given key.
*/
color Theme_getColor(Theme *this, char *sKey) {
  color *pColor = HashMap_get(this->pColorMap, sKey);

  // Color does not exist
  if(pColor == NULL)
    return -1;

  return *pColor;
}

/**
 * //
 * ////
 * //////    ThemeManager struct
 * ////////
 * ////////// 
*/

/**
 * Manages all the themes of the program.
 * 
 * @struct
*/
struct ThemeManager {
  char *sActiveTheme;   // The key of the active theme
  HashMap *pThemeMap;   // Stores all the themes
};

/**
 * Creates a default theme which the program uses.
 * 
 * @param   { ThemeManager * }  this  The theme manager.
*/
void ThemeManager_init(ThemeManager *this) {

  // Init some stuff
  this->sActiveTheme = "default";
  this->pThemeMap = HashMap_create();

  // Create the default theme
  Theme *pDefaultTheme = Theme_create();
  Theme_addColor(pDefaultTheme, "primary", 0xfef9ff);
  Theme_addColor(pDefaultTheme, "secondary", 0x111317);
  Theme_addColor(pDefaultTheme, "accent", 0xf18f01);
  Theme_addColor(pDefaultTheme, "accent2", 0xaa1616);
  Theme_addColor(pDefaultTheme, "anti-accent", 0x4282b3);

  // Add it to hashmap
  HashMap_add(this->pThemeMap, "default", pDefaultTheme);
}

/**
 * Exits the theme manager.
 * 
 * @param   { ThemeManager * }  this  The theme manager.
*/
void ThemeManager_exit(ThemeManager *this) {
  HashMap_kill(this->pThemeMap);
}

/**
 * Changes the current active theme.
 * 
 * @param   { ThemeManager * }  this  The theme manager.
 * @param   { char * }          sKey  The key to the new active theme.
*/
void ThemeManager_setActive(ThemeManager *this, char *sKey) {
  
  // The theme does not exist
  if(HashMap_get(this->pThemeMap, sKey) == NULL)
    return;

  // Copy the key of the active theme
  this->sActiveTheme = sKey;
}

/**
 * Gets a color from the active theme referred to by the provided key. 
 * 
 * @param   { ThemeManager * }  this  The theme manager.
 * @param   { char * }          sKey  The key for the color.
*/
color ThemeManager_getActive(ThemeManager* this, char *sKey) {
  
  // The key components
  char sKeyColorName[STRING_KEY_MAX_LENGTH] = { 0 };
  char sKeyModifier[STRING_KEY_MAX_LENGTH] = { 0 };
  char sKeyParameter[STRING_KEY_MAX_LENGTH] = { 0 };
  int dKeySection = 0;

  // No color was selected
  if(sKey == NULL || !strlen(sKey))
    return -1;

  // The active theme
  Theme *pActiveTheme = HashMap_get(this->pThemeMap, this->sActiveTheme);

  // Parse the key first
  while(*sKey) {
    
    // Next part of the string
    if(*sKey == '-') {
      dKeySection++;
    
    // Copy the string
    } else {

      // Where to copy the string to
      switch(dKeySection) {
        case 0: sKeyColorName[strlen(sKeyColorName)] = *sKey; break;
        case 1: sKeyModifier[strlen(sKeyModifier)] = *sKey; break;
        case 2: sKeyParameter[strlen(sKeyParameter)] = *sKey; break;
        default: break;
      }
    }

    sKey++;
  }

  // No color was selected
  if(!strlen(sKeyColorName) || !strcmp(sKeyColorName, ""))
    return -1;

  // If the modifier doesn't have a value, just return the color as is
  if(!strlen(sKeyModifier))
    return Theme_getColor(pActiveTheme, sKeyColorName);

  // We lighten the color
  if(!strcmp(sKeyModifier, "lighten") || !strcmp(sKeyModifier, "lighter"))
    return Theme_getLighter(pActiveTheme, sKeyColorName, atof(sKeyParameter));

  // We darken the color
  if(!strcmp(sKeyModifier, "darken") || !strcmp(sKeyModifier, "darker"))
    return Theme_getDarker(pActiveTheme, sKeyColorName, atof(sKeyParameter));

  // Bad input
  return -1;
}

/**
 * Reads and parses the themes contained in a themes file.
 * 
 * @param   { ThemeManager * }  this        The theme manager.
 * @param   { char * }          sFilepath   Where to find the themes file.
*/
void ThemeManager_readThemeFile(ThemeManager *this, char *sFilepath) {

  // The length of the output buffer and the theme file
  int dThemeFileBufferLength = 0, dThemeStringLength = 0, i, j;
  char *sThemeFileBuffer[THEME_FILE_MAX_LEN];
  File *pThemeFile = File_create(sFilepath);

  // A theme instance and its associated details
  Theme *pTheme;
  char sThemeName[STRING_KEY_MAX_LENGTH] = { 0 };
  char sColorName[STRING_KEY_MAX_LENGTH] = { 0 };
  char sHexValue[STRING_KEY_MAX_LENGTH] = { 0 };
  int bIsValue = 0;

  // Read the file
  File_readText(pThemeFile, THEME_FILE_MAX_LEN, 
    &dThemeFileBufferLength, 
    sThemeFileBuffer);

  // Create a theme for each line
  for(i = 0; i < dThemeFileBufferLength; i++) {

    // Get the length of the theme string
    dThemeStringLength = strlen(sThemeFileBuffer[i]);

    // Reset the theme name first
    while(strlen(sThemeName)) sThemeName[strlen(sThemeName) - 1] = 0;

    // Parse the theme name
    j = 0;
    while(sThemeFileBuffer[i][++j] != ']')
      sThemeName[j - 1] = sThemeFileBuffer[i][j];

    // If the key wasn't duplicated
    if(HashMap_get(this->pThemeMap, sThemeName) == NULL) {

      // Create a theme object
      pTheme = Theme_create();
      HashMap_add(this->pThemeMap, sThemeName, pTheme);

      // Get the parameters
      bIsValue = 0;
      while(++j < dThemeStringLength) {
        
        // We have a comma character or its the end of the line
        if(sThemeFileBuffer[i][j] == ',' || 
          sThemeFileBuffer[i][j] == '\n' || 
          sThemeFileBuffer[i][j] == '\r') {
          
          // Add the color to the theme
          Theme_addColor(pTheme, sColorName, (int) strtol(sHexValue, NULL, 16));

          // Reset the parameters
          bIsValue = 0;
          while(strlen(sColorName)) sColorName[strlen(sColorName) - 1] = 0;
          while(strlen(sHexValue)) sHexValue[strlen(sHexValue) - 1] = 0;

        // We get the name
        } else {
          
          // Check whether we're parsing string or hex value
          if(sThemeFileBuffer[i][j] == ':') {

            // We're reading the hex value now
            bIsValue = 1;
          
          // Keep copying stuff
          } else {

            // Copy the name or the value
            if(!bIsValue) sColorName[strlen(sColorName)] = sThemeFileBuffer[i][j];
            else sHexValue[strlen(sHexValue)] = sThemeFileBuffer[i][j];
          } 
        }
      }
    }
  }

  // Kill the file if ever
  if(pThemeFile != NULL)
    File_kill(pThemeFile);
}

#endif