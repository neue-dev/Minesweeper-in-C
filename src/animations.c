/**
 * @ Author: MMMM
 * @ Create Time: 2024-02-25 10:46:20
 * @ Modified time: 2024-02-27 10:34:19
 * @ Description:
 * 
 * This file contains definitions for animation handlers (basically,
 *    functions that increment the animation over time).
 */

#include "./utils/utils.animation.h"
#include "./utils/utils.types.h"

/**
 * //
 * ////
 * //////    Animation handlers
 * ////////
 * ////////// 
*/

void AnimationHandler_intro(p_obj Args_Animation) {
  Animation *this = (Animation *) Args_Animation;

  this->dStates[0]++;
}
