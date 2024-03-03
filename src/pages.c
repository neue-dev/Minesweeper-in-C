/**
 * @ Author: MMMM
 * @ Create Time: 2024-02-25 15:06:24
 * @ Modified time: 2024-03-03 11:25:07
 * @ Description:
 * 
 * This file defines page configurers so we can define the different pages of our application.
 * Note that these functions are passed as callbacks to the pageManager.
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
 * 
 * @param   { p_obj }   pArgs_Page  The instance page we need to configure.
*/
void PageConfigurer_intro(p_obj pArgs_Page) {
  Page *this = (Page *) pArgs_Page;

  AnimationManager_createAnimation(
    &this->animationManager,
    "intro", AnimationHandler_intro, this, 0);
}

/**
 * Configures the main menu.
 * 
 * @param   { p_obj }   pArgs_Page  The page instance we need to configure.
*/
void PageConfigurer_menu(p_obj pArgs_Page) {
  Page *this = (Page *) pArgs_Page;

  AnimationManager_createAnimation(
    &this->animationManager,
    "menu", AnimationHandler_menu, this, 0);
}

#endif