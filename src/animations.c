/**
 * @ Author: MMMM
 * @ Create Time: 2024-02-25 10:46:20
 * @ Modified time: 2024-03-03 11:42:22
 * @ Description:
 * 
 * This file contains definitions for animation handlers (basically, functions that increment 
 *    the animation over time).
 * Note that these functions are passed as callbacks to animation instances.
 */

#ifndef ANIMATIONS_
#define ANIMATIONS_

#include "./utils/utils.asset.h"
#include "./utils/utils.animation.h"
#include "./utils/utils.buffer.h"
#include "./utils/utils.page.h"
#include "./utils/utils.types.h"

#include <math.h>
#include <stdlib.h>

/**
 * //
 * ////
 * //////    Animation handlers
 * ////////
 * ////////// 
*/

/**
 * Handles the intro animations.
 * 
 * @param   { p_obj }   Args_Animation  The animation object that contains our state data.
 * @param   { p_obj }   Args2_Page      The page object we're gonna update based on our state data.
*/
void AnimationHandler_intro(p_obj Args_Animation, p_obj Args2_Page) {
  Animation *this = (Animation *) Args_Animation;
  Page *pPage = (Page *) Args2_Page;
  Buffer *pBuffer = pPage->pPageBuffer;
 
  int dWidth = pBuffer->dWidth;
  int dHeight = pBuffer->dHeight;
  int i, dCumulativeLen = 0;

  char **aTitleGlyph;                     // We prepend asset variable names with a.
  char sTitleGlyphs[12] = "minesweeper";  // A list of the characters we need from the asset set.
  char *sGlyphName = String_alloc(16);    // We'll use this to get the specific name of the asset.

  // Initialization stage
  if(this->eAnimationState == ANIMATION_INIT) {

    // Init the states we need; these refer to the positions of each letter
    for(i = 0; i < 11; i++) {
      this->dStates[i * 2] = dCumulativeLen + 24;
      this->fStates[i * 2] = dCumulativeLen * 5.0 - 50.0;
      
      this->dStates[i * 2 + 1] = 12;
      this->fStates[i * 2 + 1] = i * i * i * 10.0 - 128.0;

      // Get the name of the asset
      sprintf(sGlyphName, "title-font-%c", sTitleGlyphs[i]);
      aTitleGlyph = AssetManager_getAssetText(pPage->pSharedAssetManager, sGlyphName);
      dCumulativeLen += String_length(aTitleGlyph[0]);
    }

    // You have to manually set these cuz u didnt pass a statecount in the constuctor
    this->dFloatStateCount = 22;
    this->dIntStateCount = 22;

  // Running the animation
  } else {

    // Make the fStates approach their targets
    for(i = 0; i < 22; i++)
      this->fStates[i] = Math_ease(this->fStates[i], this->dStates[i], 0.69);
    
    // Make sure we can display signature at the bottom
    Buffer_contextRect(pBuffer, 0, 0, dWidth, dHeight, 0xffffff, 0x000000);

    // Total black at the edges
    Buffer_contextRect(pBuffer, 0, 0, dWidth - 1, dHeight, 0x000000, 0x000000);

    // A red bounding box
    Buffer_contextRect(pBuffer, 10, 5, dWidth - 20, dHeight - 10, 0xdd2121, 0xdd2121);

    // White canvas in center
    Buffer_contextRect(pBuffer, 12, 6, dWidth - 24, dHeight - 12, 0x000000, 0xffffff);

    // For each letter
    for(i = 0; i < 11; i++) {
      sprintf(sGlyphName, "title-font-%c", sTitleGlyphs[i]);

      Buffer_write(pBuffer, 
        this->dStates[i * 2], this->dRoundStates[i * 2 + 1], 
        AssetManager_getAssetHeight(pPage->pSharedAssetManager, sGlyphName), 
        AssetManager_getAssetText(pPage->pSharedAssetManager, sGlyphName));
    }

    // This is our exit condition (this animation doesn't happen forever)
    if(round(this->fStates[0] * 1000.0) == this->dStates[0] * 1000) {
      this->eAnimationState = ANIMATION_EXIT;
      pPage->ePageState = PAGE_ACTIVE_IDLE;
    }
  }
}

/**
 * Handles the menu animations.
 * 
 * @param   { p_obj }   Args_Animation  The animation object that contains our state data.
 * @param   { p_obj }   Args2_Page      The page object we're gonna update based on our state data.
*/
void AnimationHandler_menu(p_obj Args_Animation, p_obj Args2_Page) {
  Animation *this = (Animation *) Args_Animation;
  Page *pPage = (Page *) Args2_Page;
  Buffer *pBuffer = pPage->pPageBuffer;
 
  int dWidth = pBuffer->dWidth;
  int dHeight = pBuffer->dHeight;
  int i;

  // We perform the initialization here
  if(this->eAnimationState == ANIMATION_INIT) {

  // This is where the animation updates happen
  } else {

    // Make sure we can display signature at the bottom
    Buffer_contextRect(pBuffer, 0, 0, dWidth, dHeight, 0xffffff, 0x000000);

    // Total black at the edges
    Buffer_contextRect(pBuffer, 0, 0, dWidth - 1, dHeight, 0x000000, 0x000000);

    // A red bounding box
    Buffer_contextRect(pBuffer, 10, 5, dWidth - 20, dHeight - 10, 0xdd2121, 0xdd2121);

    // White canvas in center
    Buffer_contextRect(pBuffer, 12, 6, dWidth - 24, dHeight - 12, 0x000000, 0xffffff);
    
  }
}

#endif
