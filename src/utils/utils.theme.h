/**
 * @ Author: MMMM
 * @ Create Time: 2024-03-08 09:36:02
 * @ Modified time: 2024-03-08 09:52:23
 * @ Description:
 * 
 * A class for handling themes so changing colors individially doesnt end up becoming a pain in the ass.
 */

#ifndef UTILS_THEME_
#define UTILS_THEME_

#include "./utils.graphics.h"
#include "./utils.types.h"

typedef struct Theme Theme;
typedef struct ThemeManager ThemeManager;

/**
 * An instantiable class that holds references to different colors.
 * A theme consists of five main colors.
*/
struct Theme {
  color colorPrimary;         // The primary color (usually for the background, etc.)
  color colorSecondary;       // The secondary color that supplements the primary
  color colorPositiveAccent;  // Used for positive feedback (ie, a good job popup or smth)
  color colorNegativeAccent;  // Used for negative feedback (when bad stuff happen in the UI)
  color colorNeutralAccent;   // Used for highlighting stuff that neither affirm nor deny user actions
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


// ! create function Theme_getShade() which uses lerp between black and white

#endif