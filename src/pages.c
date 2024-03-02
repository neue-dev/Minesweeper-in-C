/**
 * @ Author: MMMM
 * @ Create Time: 2024-02-25 15:06:24
 * @ Modified time: 2024-03-02 23:11:26
 * @ Description:
 * 
 * This file defines page configurers so we can define the different pages of our application.
 */

#ifndef PAGES_
#define PAGES_

#include "./utils/utils.page.h"
#include "./utils/utils.animation.h"

#include "./animations.c"

/**
 * //
 * ////
 * //////    Page configurers
 * ////////
 * ////////// 
*/

/**
 * Configures the title page.
*/
void PageConfigurer_intro(p_obj pArgs_Page) {
  Page *this = (Page *) pArgs_Page;

  AnimationManager_createAnimation(
    &this->animationManager,
    "intro", AnimationHandler_intro, this, 0);
}

#endif