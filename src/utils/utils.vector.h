/**
 * Creates a vector class which can help us deal with coordinates and stuff
*/

#ifndef UTILS_VECTOR
#define UTILS_VECTOR

#include <stdlib.h>

/**
 * The vector class helps us store info like screen coordinates or grid dimensions.
 * Note that the components of the vector are integers as we do not need float values
 *    for this use case.
 * 
 * @class
*/
typedef struct Vec2D {
  int x;
  int y;
} Vec2D;

/**
 * Allocates memory for an instance of the vector class.
 * The function returns NULL when the allocation fails.
 * 
 * @return  { Vec2D * }   An instance of the Vec2D class.
*/
Vec2D *Vec2D_new() {
  Vec2D *pVec2D = calloc(1, sizeof(*pVec2D));
  return pVec2D;
}

/**
 * Initializes a new instance of the Vec2D class.
 * 
 * @param   { Vec2D * }   this  A pointer to the instance to be initialized.
 * @param   { int }       x     The x-coordinate stored by the instance.
 * @param   { int }       y     The y-coordinate stored by the instance.
 * @return  { Vec2D * }         A pointer to the initialized class.
*/
Vec2D *Vec2D_init(Vec2D *this, int x, int y) {
  this->x = x;
  this->y = y;

  return this;
}

/**
 * Creates an initialized instance of the Vec2D class based on the passed values.
 * 
 * @param   { int }       x   The x-coordinate stored by the instance.
 * @param   { int }       y   The y-coordinate stored by the instance.
 * @return  { Vec2D * }       A pointer to the initialized class.
*/
Vec2D *Vec2D_create(int x, int y) {
  return Vec2D_init(Vec2D_new(), x, y);
}

/**
 * Destroys an instance of the Vec2D class.
 * Helps with garbage collection.
 * 
 * @param   { Vec2D * }   this  The instance to be freed from memory.
*/
void Vec2D_kill(Vec2D *this) {
  free(this);
}

#endif