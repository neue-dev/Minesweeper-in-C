/**
 * @ Author: MMMM
 * @ Create Time: 2024-03-08 09:36:02
 * @ Modified time: 2024-03-11 18:54:40
 * @ Description:
 * 
 * A class for handling themes so changing colors individially doesnt end up becoming a pain in the ass.
 */

#ifndef UTILS_THEME_
#define UTILS_THEME_

#include "./utils.hashmap.h"
#include "./utils.graphics.h"
#include "./utils.types.h"

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
  free(this);
}

/**
 * Adds a new color to the theme given the provided key.
 * 
 * //! todo
*/
void Theme_addColor(Theme* this, char *sKey, color color) {
  
}

/**
 * Gets a darker version of a specific color from the current theme.
 * The number of steps is also indicated (how far to darken).
 * ! todo
*/
void Theme_getDarker() {

}

/**
 * Gets a lighter version of a specific color from the current theme.
 * The number of steps is also indicated (how far to lighten).
 * !todo
*/
void Theme_getLighter() {
  
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
*/
void ThemeManager_init(ThemeManager *this) {

  // Init some stuff
  this->sActiveTheme = "default";
  this->pThemeMap = HashMap_create();

  // Create and add the default theme
  Theme *pDefaultTheme = Theme_create();

  // ! add the five default colors here

  HashMap_add(this->pThemeMap, this->sActiveTheme, pDefaultTheme);
}

// ! create themeManager_setActive
// ! if theme does not exist, just return

#endif