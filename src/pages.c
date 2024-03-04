/**
 * @ Author: MMMM
 * @ Create Time: 2024-02-25 15:06:24
 * @ Modified time: 2024-03-04 12:31:40
 * @ Description:
 * 
 * This file defines page configurers so we can define the different pages of our application.
 * Note that these functions are passed as callbacks to the pageManager.
 * Also, a page may have more than one runner.
 */

#ifndef PAGES_
#define PAGES_

#include "./utils/utils.page.h"
#include "./utils/utils.runner.h"

#include "./runners.c"

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

  RunnerManager_createRunner(
    &this->runnerManager,
    "intro", RunnerHandler_intro, this, 0);
}

/**
 * Configures the main menu.
 * 
 * @param   { p_obj }   pArgs_Page  The page instance we need to configure.
*/
void PageConfigurer_menu(p_obj pArgs_Page) {
  Page *this = (Page *) pArgs_Page;

  RunnerManager_createRunner(
    &this->runnerManager,
    "menu", RunnerHandler_menu, this, 0);
}

#endif