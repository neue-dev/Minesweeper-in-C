/**
 * @ Author: MMMM
 * @ Create Time: 2024-03-08 09:36:02
 * @ Modified time: 2024-03-09 17:02:36
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
 * A theme consists of five main colors.
*/
struct Theme {
  color colorPrimary;         // The primary color (usually for the background, etc.)
  color colorSecondary;       // The secondary color that supplements the primary
  color colorNeutralAccent;   // Used for highlighting stuff that neither affirm nor deny user actions
                              // Note that this will be the primary accent used for the UI
  color colorPositiveAccent;  // Used for positive feedback (ie, a good job popup or smth)
  color colorNegativeAccent;  // Used for negative feedback (when bad stuff happen in the UI)
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
Theme *Theme_init(Theme *this, color colorPrimary, color colorSecondary, color colorNeutralAccent, color colorPositiveAccent, color colorNegativeAccent) {

  // Store the provided colors
  this->colorPrimary = colorPrimary;
  this->colorSecondary = colorSecondary;
  this->colorNeutralAccent = colorNeutralAccent;
  this->colorPositiveAccent = colorPositiveAccent;
  this->colorNegativeAccent = colorNegativeAccent;

  return this;
}

/**
 * Creates an initialized instance of the Theme class.
 * 
 * @return	{ Theme * }		A pointer to the newly created initialized instance.
*/
Theme *Theme_create(color colorPrimary, color colorSecondary, color colorNeutralAccent, color colorPositiveAccent, color colorNegativeAccent) {
  return Theme_init(Theme_new(), colorPrimary, colorSecondary, colorNeutralAccent, colorPositiveAccent, colorNegativeAccent);
}

/**
 * Deallocates the memory of an instance of the Theme class.
 * 
 * @param		{ Theme * }		this	A pointer to the instance to deallocate.
*/
void Theme_kill(Theme *this) {
  free(this);
}


// ! create function Theme_getShade() which uses lerp between black and white

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

  color color1st;       // The primary color
  color color2nd;       // The secondary color
  color colorNeu;       // The neutral accent
  color colorPos;       // The positive accent
  color colorNeg;       // The negative accent
};

/**
 * Creates a default theme which the program uses.
*/
void ThemeManager_init(ThemeManager *this) {

  // Init some stuff
  this->sActiveTheme = "default";
  this->pThemeMap = HashMap_create();

  // Create and add the default theme
  Theme *pDefaultTheme = Theme_create(0xfef9ff, 0x111317, 0xfe9d0b, 0x4282b3, 0xf33016);
  HashMap_add(this->pThemeMap, this->sActiveTheme, pDefaultTheme);

  // Set the colors of the active theme
  this->color1st = pDefaultTheme->colorPrimary;
  this->color2nd = pDefaultTheme->colorSecondary;
  this->colorNeu = pDefaultTheme->colorNeutralAccent;
  this->colorPos = pDefaultTheme->colorPositiveAccent;
  this->colorNeg = pDefaultTheme->colorNegativeAccent;
}

// ! create themeManager_setActive
// ! if theme does not exist, just return

#endif